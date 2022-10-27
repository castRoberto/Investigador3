/*
 * Jan 27, 2009 - ayachan
 * Oct 16, 2017 - ayachan
 */

#include "TimerFactoryImplX.h"
#include "../../util/timespec.h"

#include <algorithm>

namespace posix
{

  /*
   * TimerElement definition
   */

  TimerFactoryImplX::TimerElement::TimerElement(
      TimerFactory::TIMER_HANDLER handler,
      Runnable* task, long delay_ms, long period_ms)
    : handler(handler), task(task), delay_ms(delay_ms), period_ms(period_ms),
      cancelled(false), running(false),
      ts()
  {
    ts = ts + delay_ms;
  }

  bool TimerFactoryImplX::TimerElement::update()
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


  TimerFactoryImplX::TimerFactoryImplX(HardwareTimer* hwtimer)
    : hwtimer(hwtimer), hmgr(), timers(), list(),
      mutex(Mutex::MUTEX_ATTR_NORMAL), thrdmgr()
  {
    this->hwtimer->catchHardwareInterrupt(this);
  }

  TimerFactoryImplX::~TimerFactoryImplX()
  {
    Mutex::MutexLock mlock(mutex, ".TimerFactoryImplX::(delete)");

    hwtimer->stopHardwareTimer();

    while (!list.empty())
    {
      TimerElement* te = list.front();
      terminateTimer(te);
      list.pop_front();
    }
  }

  TimerFactory::TIMER_HANDLER TimerFactoryImplX::schedule(Runnable* task, long delay_ms, long period_ms)
  {
    Mutex::MutexLock mlock(mutex, ".TimerFactoryImplX::schedule");

    TimerFactory::TIMER_HANDLER handler = hmgr.createHandler();
    TimerElement* te = new TimerElement(handler, task, delay_ms, period_ms);
    timers.insert(std::make_pair(handler,te));

//    std::cout << "~~~~~~ [timer]: handler " << handler <<
//                 " for " << delay_ms << " [ms]" <<
//                 " at " << util::timestamp::getTimestamp() << std::endl;

    schedule(te);
    return handler;
  }

  void TimerFactoryImplX::schedule(TimerElement* te)
  {
    // always called from locked mutex

    hwtimer->stopHardwareTimer();

    std::list<TimerElement*>::iterator next =
        upper_bound(list.begin(), list.end(), te, TimerElementOps::lt);
    list.insert(next, te);

    scheduleNext();
  }

  void TimerFactoryImplX::scheduleNext()
  {
    // always called from locked mutex

    hwtimer->stopHardwareTimer(); // this will not fail if already stopped
    if (list.empty())
    {
      // nothing to do in the meantime
//      std::cout << "[scheduleNext]: no more entries in the queue" << std::endl;
      return;
    }

    // remove all canceled timers
    for (
        std::list<TimerElement*>::iterator it = list.begin();
        it != list.end(); ++it)
    {
      TimerElement* te = *it;
      if (te->cancelled)
      {
        terminateTimer(te);
        it = list.erase(it);
      }
    }

    // retrieve next (assumed nearest)
    TimerElement* te = list.empty() ? 0 : list.front();

    // if any, set the hardware timer
    if (te != 0)
    {
//      util::timestamp tsx;
//      std::cout << "[scheduleNext]: programming next timer to " << (te->ts - tsx) << " [ms]" << std::endl;

      // check if already elapsed and set to next in time
      long elapse = te->ts - util::timestamp();
      if (elapse < 10)
      {
//        util::timestamp::abstime(te->ts);
        te->ts = util::timestamp() + (long)10;
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
  bool TimerFactoryImplX::cancel(TimerFactory::TIMER_HANDLER handler)
  {
    Mutex::MutexLock mlock(mutex, ".TimerFactoryImplX::cancel");

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
  void TimerFactoryImplX::terminateTimer(TimerElement* te)
  {
    // always called from locked mutex

    if (!hmgr.isValid(te->handler))
    {
      std::cout << "[terminateTimer.timer]: ERROR!: invalid handler " << te->handler << std::endl;
      throw TimerException("invalid handler");
    }

    std::map<int, TimerElement*>::iterator it = timers.find(te->handler);
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
  void TimerFactoryImplX::handle(void* source, const void* data)
  {
    std::list<TimerElement*> ready;
    {
      Mutex::MutexLock mlock(mutex, ".TimerFactoryImplX::handle");

      if (list.empty())
      {
//        std::cout << "[handle]: no more entryes in the queue" << std::endl;
        return;
      }

      /*
       * Approach that take all 'ready' timers...
       */

      bool loop = true;
      while (loop && !list.empty())
      {
        TimerElement* te = list.front();
        if (te->cancelled)
        {
          // no more to do with this task
  //        std::cout << "[handle]: current entry was canceled" << std::endl;
          terminateTimer(te);
          list.pop_front();
        }
        else
        {
          long elapse = te->ts - util::timestamp();
          if (elapse < 10)
          {
            ready.push_back(te);
            list.pop_front();
          }
          else
            loop = false;
        }
      }

      // set next hardware timer
      scheduleNext();
    }

    // start selected outside the lock
    for (
        std::list<TimerElement*>::iterator it = ready.begin();
        it != ready.end(); ++it)
    {
//      std::cout << "------ [timer]: handler " << te->handler << ", " << te->certeince() << std::endl;

      TimerElement* te = *it;

      // run the timed method in a thread
      te->running = true;
      TimerTaskRunner* ttr = new TimerTaskRunner(te, this);
      thrdmgr.manage(ttr, true);
      ttr->start();
    }
    ready.clear(); // not really needed
  }

  /**
   * Called when the timer's method is executed.
   * Is called from the enveloper thread just before the thread is auto-released.
   */
  void TimerFactoryImplX::timerTaskExecuted(TimerTaskRunner* ttr, void* result)
  {
    Mutex::MutexLock mlock(mutex, ".TimerFactoryImplX::timerTaskExecuted");

    if ( (!ttr->te->cancelled) && (ttr->te->update()))
      schedule(ttr->te);
    else
    {
      // terminate now
      terminateTimer(ttr->te);
//      // will be removed next cycle
//      ttr->te->cancelled = true;
    }

    // the TimerTaskRunner pointer is released by the thread manager
  }

} // namespace
