
#include "TimerImpl.h"

namespace posix
{

  TimerImpl::TimerImpl(HardwareTimer* hwtimer)
  {
    this->hwtimer = hwtimer;

    this->hwtimer->catchHardwareInterrupt(this);
  }

  TimerImpl::~TimerImpl()
  {
  }

  /**
   * The task to be cancelled could be either:
   *  1. at the heap, waiting to be executed in some future,
   *  2. actually executing
   * Actually the task can't just be removed either from the heap or que list, the
   * best chances are just to mark the task as cancelled and remove it at a better
   * moment.
   */
  void TimerImpl::cancel(TimerTask* task)
  {
    Mutex::MutexLock mlock(mutex, ".TimerImpl::cancel");

    if (task != 0)
    {
//      cout << "[TimerImpl]: wants to cancel " << task << endl;
      task->setCancelled(true);
    }
    else
    {
      std::cout << "[TimerImpl]: cancel all tasks" << task << std::endl;
    }
  }

  /**
   * Create the TimerElement and schedule.
   */
  void TimerImpl::schedule(TimerTask* task, long delay_ms, long period_ms)
  {
    task->setCancelled(false);
    task->setTimer(this);
    TimerElement* te = new TimerElement(task, delay_ms, period_ms);
    schedule(te);
  }

  /**
   * To add the timer, first stop the hardware timer, put the new element and set
   * again the hardware timer with the nearest timer.
   * Always will be at least one element in the queue.
   */
  void TimerImpl::schedule(TimerElement* te)
  {
    Mutex::MutexLock mlock(mutex, ".TimerImpl::schedule");

    hwtimer->stopHardwareTimer();
    pqueue.push(te);
    hwtimer->setHardwareTimer(pqueue.top()->ts);
  }

  /**
   * (Hardware) Timer completed: get the task to do, encapsulated in a
   * TimerTaskRunner and store it in the vector. Then execute the task in an
   * independent thread.
   * Finally set the hardware timer to the next time, if exists.
   * May happen that no timer exists in the queue (cleaning or cancellation). Also
   * can happen that the timer that produce the timeout is not there any more, for
   * the same reasons.
   */
  void TimerImpl::handle(void* source, const void* data)
  {
    Mutex::MutexLock mlock(mutex, ".TimerImpl::handle");

    if (pqueue.empty())
      return;

    TimerElement* te = pqueue.top(); pqueue.pop();
    if (te->task->isCancelled())
      // no more to do with this task
      delete te;
    else
    {
      TimerTaskRunner* ttr = new TimerTaskRunner(te, this);
      thrdmgr.manage(ttr, true);
      ttr->start();
    }

    if (pqueue.size() > 0)
      hwtimer->setHardwareTimer(pqueue.top()->ts);
  }

  /**
   * TimerElement's task executed.
   * Check if the timer is periodic and put it again in the queue, otherwise free
   * the timer element.
   * The host TimerTaskRunner is freed by the ThreadManager.
   */
  void TimerImpl::timerTaskExecuted(TimerTaskRunner* ttr, void* result)
  {
    Mutex::MutexLock lock(mutex, ".TimerImpl::timerTaskExecuted");

  //  executing.remove(ttr);
    if ( (!ttr->te->task->isCancelled()) && (ttr->te->update()))
      schedule(ttr->te);
    else
      delete ttr->te;
  //  delete ttr;
  }

} // namespace
