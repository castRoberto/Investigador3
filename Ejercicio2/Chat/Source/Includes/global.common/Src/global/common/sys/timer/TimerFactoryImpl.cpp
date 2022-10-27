/*
 * Jan 27, 2009 - ayachan
 */

#include "TimerFactoryImpl.h"
#include "../../util/timespec.h"

namespace posix
{

  /*
   * TimerElement definition
   */

  TimerFactoryImpl::TimerElement::TimerElement(
      TimerFactory::TIMER_HANDLER handler, Runnable* task, long delay_ms, long period_ms)
    : handler(handler), task(task), delay_ms(delay_ms), period_ms(period_ms),
      cancelled(false), running(false),
      ts()
  {
    ts = ts + delay_ms;
  }

  bool TimerFactoryImpl::TimerElement::update()
  {
    bool result = false;
    if (period_ms > 0)
    {
      util::timestamp::abstime(ts);
      ts = ts + period_ms;
      result = true;
    }
    return result;
  }


  TimerFactoryImpl::TimerFactoryImpl(HardwareTimer* hwtimer)
    : hwtimer(hwtimer), hmgr(), timers(), pqueue(),
      mutex(Mutex::MUTEX_ATTR_RECURSIVE), thrdmgr()
  {
    this->hwtimer->catchHardwareInterrupt(this);
  }

  TimerFactoryImpl::~TimerFactoryImpl()
  {
    Mutex::MutexLock mlock(mutex, ".TimerFactoryImpl::(delete)");

    hwtimer->stopHardwareTimer();

    while (!pqueue.empty())
    {
      TimerElement* te = pqueue.top();
      terminateTimer(te);
      pqueue.pop();
    }
  }

  TimerFactory::TIMER_HANDLER TimerFactoryImpl::schedule(Runnable* task, long delay_ms, long period_ms)
  {
    Mutex::MutexLock mlock(mutex, ".TimerFactoryImpl::schedule");

    TimerFactory::TIMER_HANDLER handler = hmgr.createHandler();
    TimerElement* te = new TimerElement(handler, task, delay_ms, period_ms);
    timers.insert(std::make_pair(handler,te));

//    std::cout << "~~~~~~ [timer]: handler " << handler <<
//                 " for " << delay_ms << " [ms]" <<
//                 " at " << util::timestamp::getTimestamp() << std::endl;

    schedule(te);
    return handler;
  }

  void TimerFactoryImpl::schedule(TimerElement* te)
  {
    // always called from locked mutex

    hwtimer->stopHardwareTimer();
    pqueue.push(te);
    scheduleNext();
  }

  void TimerFactoryImpl::scheduleNext()
  {
    // always called from locked mutex

    hwtimer->stopHardwareTimer(); // this will not fail if already stopped
    if (pqueue.empty())
    {
      // no more to do
//      std::cout << "[scheduleNext]: no more entryes in the queue" << std::endl;
      return;
    }

    // skip cancelled elements
    TimerElement* te = pqueue.top();
    while ( (te != 0) && (te->cancelled))
    {
//      std::cout << "[scheduleNext]: skipping cancelled entry" << std::endl;
      terminateTimer(te);
      pqueue.pop();
      te = pqueue.empty() ? 0 : pqueue.top();
    }

    // if any, set the hardware timer
    if (te != 0)
    {
//      util::timestamp tsx;
//      std::cout << "[scheduleNext]: programming next timer to " << (te->ts - tsx) << " [ms]" << std::endl;

      // check if already elapsed and set to next in time
      long elapse = te->ts - util::timestamp();
      if (elapse <= 0)
      {
        util::timestamp::abstime(te->ts);
        te->ts = util::timestamp() + (long)1;
      }
//      std::cout << "[scheduleNext]: programming next timer to " << elapse << " [ms]" << std::endl;

      hwtimer->setHardwareTimer(te->ts.get_ts());
    }
//    else
//    {
//      std::cout << "[scheduleNext]: no enable entry found in queue" << std::endl;
//    }
  }

  /*
   * 20170403 aya
   * Due to unknown timeout errors trying to lock the mutex, test the approach
   * to just mark the item as canceled and deal with it when handling.
   */
  bool TimerFactoryImpl::cancel(TimerFactory::TIMER_HANDLER handler)
  {
    Mutex::MutexLock mlock(mutex, ".TimerFactoryImpl::cancel");

    std::map<int,TimerElement*>::iterator it = timers.find(handler);
    if (it == timers.end())
    {
      std::cout << "[cancel]: handler " << handler << " has no timer element!" << std::endl;
    }
    else
    {
      it->second->cancelled = true;
    }
    return true; // as if works
  }

  /**
   * Called *always* from inside.
   * The element must be outside the queue and should exist.
   * This is the only place where the timer element would be deleted.
   */
  void TimerFactoryImpl::terminateTimer(TimerElement* te)
  {
    // always called from locked mutex

    if (!hmgr.isValid(te->handler))
    {
      std::cout << "[terminateTimer.timer]: ERROR!: invalid handler " << te->handler << std::endl;
      throw TimerException("invalid handler");
    }

    std::map<int,TimerElement*>::iterator it = timers.find(te->handler);
    if (it == timers.end())
    {
      std::cout << "[terminateTimer.timer]: ERROR!: missing timer element!" << std::endl;
      throw TimerException("missing timer element");
    }

    if (it->second != te)
    {
      std::cout << "[terminateTimer.timer]: ERROR!: inconsistent timer element!" << std::endl;
      throw TimerException("inconsistent timer element");
    }

    hmgr.releaseHandler(it->first);
    delete it->second;
    timers.erase(it);
  }

  /**
   * Received from the hardware timer implementation: time to execute the timed
   * method.
   *
   * @see Listener
   */
  void TimerFactoryImpl::handle(void* source, const void* data)
  {
    Mutex::MutexLock mlock(mutex, ".TimerFactoryImpl::handle");

    if (pqueue.empty())
    {
//      std::cout << "[handle]: no more entryes in the queue" << std::endl;
      return;
    }

    TimerElement* te = pqueue.top(); pqueue.pop();
    if (te->cancelled)
    {
      // no more to do with this task
//      std::cout << "[handle]: current entry was canceled" << std::endl;
      terminateTimer(te);
    }
    else
    {
//      std::cout << "------ [timer]: handler " << te->handler << ", " << te->certeince() << std::endl;

      // run the timed method in a thread
      te->running = true;
      TimerTaskRunner* ttr = new TimerTaskRunner(te, this);
      thrdmgr.manage(ttr, true);
      ttr->start();
    }

    scheduleNext();
  }

  /**
   * Called when the timer's method is executed.
   * Is called from the enveloper thread just before the thread is auto-released.
   */
  void TimerFactoryImpl::timerTaskExecuted(TimerTaskRunner* ttr, void* result)
  {
    Mutex::MutexLock mlock(mutex, ".TimerFactoryImpl::timerTaskExecuted");

    if ( (!ttr->te->cancelled) && (ttr->te->update()))
      schedule(ttr->te);
    else
      // will be removed next cycle
      ttr->te->cancelled = true;
  }

} // namespace
