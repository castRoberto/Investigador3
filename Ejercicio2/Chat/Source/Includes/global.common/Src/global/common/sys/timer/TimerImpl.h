#ifndef _TIMERIMPL_H_
#define _TIMERIMPL_H_

#include "HardwareTimer.h"
#include "../sync/Mutex.h"
#include "../thread/Thread.h"
#include "../thread/ThreadManager.h"
#include "../../global/Listener.h"

#include "Timer.h"
#include "TimerTask.h"
#include "../../util/timespec.h"

#include <time.h>

#include <queue>
#include <list>
#include <iostream>

namespace posix
{

  /**
   * Timer class implementation, currently based in POSIX hardware timer's.
   *
   * Schedules are store on a priority queue with inverse order to get the next
   * task to be timed each time. The task plus the time parameters are store on an
   * internal struct TimerElement (with appropiate comparison function).
   * The single active hardware timer always refers to the first element in the
   * queue, that it's extracted and executed its associated task when the timer
   * expires. If the task was declared periodic, the element is inserted again
   * with a new timeout.
   * Tasks, extracted from the queue and currently under execution, are stored on
   * a single queue.
   *
   * [2009.01.20] BUG FOUND
   * The design concept of this class will produce a bug under certain
   * conditions. To details see TimerTask.
   *
   * @deprecated HAVE A BUG WHEN RE-SCHEDULING TASK's. USE AnotherTimerImpl.
   */
  class TimerImpl : public Timer, public Listener
  {

  public:
    TimerImpl(HardwareTimer* hwtimer);
    ~TimerImpl();

  private:
    /**
     * The object to be stored on the heap.
     * Contains a timespec structure to hold the absolute time when the associated
     * timer must be schedule.
     */
    struct TimerElement
    {
      TimerElement(TimerTask* task, long delay_ms, long period_ms)
      { this->task = task; this->delay_ms = delay_ms; this->period_ms = period_ms;
        ts = abstime() + (delay_ms * 1000); }

      bool operator== (const TimerElement& te)
      { return (task == te.task); }

      bool update()
      {
        bool result = false;
        if (period_ms > 0)
        {
          ts = abstime() + (period_ms * 1000);
          result = true;
        }
        return result;
      }

      TimerTask* task;
      long delay_ms, period_ms;

      struct timespec ts;
    };
    /**
     * The comparison function of the object above.
     */
    struct lstrTimerElement
    {
      bool operator()(const TimerElement* te1, const TimerElement* te2) const
      {
        // reverse compare to sort the queue less-first
        return te2->ts < te1->ts;
      }
    };

    /**
     * This class serves as envelope to the TimerElement task execution, warning
     * the parent class when it ends to continue the corresponding procedures.
     */
    struct TimerTaskRunner : public Thread
    {
      TimerElement* te;
      TimerImpl* timer;

      TimerTaskRunner(TimerElement* te, TimerImpl* timer)
        : te(te), timer(timer) {}

      bool operator== (const TimerTaskRunner& ttr)
      { return (te == ttr.te); }

      void* run(void* args = 0)
      {
        void* result = te->task->run(args);
        timer->timerTaskExecuted(this, result);
        return 0;
      }
    };

  public:
    friend struct posix::TimerImpl::TimerTaskRunner;

    /**
     * @see Timer
     */
    void cancel(TimerTask* task = 0);
    /**
     * @see Timer
     */
    void schedule(TimerTask* task, long delay_ms, long period_ms = 0);

    /**
     * @see Listener
     */
    void handle(void* source, const void* data);

  protected:
    void schedule(TimerElement* te);

    void timerTaskExecuted(TimerTaskRunner* ttr, void* result);

  private:
    // the hardware timer to be used: given
    HardwareTimer* hwtimer;

    // the priority queue to store pending timers
    std::priority_queue<TimerElement*, std::vector<TimerElement*>, lstrTimerElement> pqueue;

    /*
    // the list of currently executing tasks
    list<TimerTaskRunner*> executing;
    */

    // a mutex to synchronize the access to methods
    Mutex mutex;

    // a thread manager to care about all those threads
    ThreadManager thrdmgr;
  };

} // namespace

#endif /*_TIMERIMPL_H_*/
