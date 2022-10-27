#ifndef _TIMERFACTORYIMPL_H_
#define _TIMERFACTORYIMPL_H_

/*
 * Jan 27, 2009 - ayachan
 * Apr 3, 2017 - ayachan
 */

#include "TimerFactory.h"
#include "HardwareTimer.h"

#include "../../util/timestamp.h"

#include "../HandlerManager.h"
#include "../sync/Mutex.h"
#include "../thread/Thread.h"
#include "../thread/ThreadManager.h"
#include "../../global/Listener.h"

#include <queue>
#include <map>
#include <iostream>

namespace posix
{

  /**
   * Timer class implementation, currently based in POSIX hardware timer's.
   *
   * Schedules are stored on a priority queue with inverse order to get the next
   * task to be timed each time. The task plus the time parameters are store on
   * an internal struct TimerElement (with appropriate comparison function).
   * The single active hardware timer always refers to the first element in the
   * queue, that element is extracted and its associated task executed when the
   * timer expires. If the task was declared periodic, the element is inserted
   * again with a new timeout.
   * Tasks, extracted from the queue and currently under execution, are stored
   * on a single queue.
   *
   * This implementation resolve some bug conditions detected in the design of
   * the previous TimerImpl class. To details see TimerTask.
   *
   * [2009.02.03] aya
   * Redefine the handler type to a typo to (just the same int, to clarify the
   * code).
   * Replace the 'priority_queue' with a custom 'priority_queue_x', a simple
   * redefinition based in the original that provide the 'remove' method, so now
   * the 'cancel' algorithm effectively remove the element from the queue.
   * If the element is not on the queue means that is already executing, so the
   * cancel fails (that is not an error).
   */
  class TimerFactoryImpl : public TimerFactory, public Listener
  {
  private:
    /**
     * The object to be stored on the heap.
     * Contains a timestamp structure to hold the absolute time when the
     * associated timer must be scheduled.
     */
    struct TimerElement
    {
      TimerElement(TimerFactory::TIMER_HANDLER handler, Runnable* task,
                   long delay_ms, long period_ms);

      bool operator == (const TimerElement& te)
      { return (handler == te.handler); }

      /**
       * @return TRUE if this timer was updated and should be re-scheduled.
       */
      bool update();

      /**
       * @return The distance to the current time.
       */
      double certeince()
      {
        return util::doublediff(util::timestamp::getTimestamp(), ts);
      }

      TimerFactory::TIMER_HANDLER handler;
      Runnable* task;
      long delay_ms, period_ms;
      bool cancelled;
      bool running;

      util::timestamp ts;
    };
    /**
     * The comparison function of the object above to be inserted into the
     * priority_queue.
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
     *
     * This thread will be managed by a ThreadManager that will release it when
     * it ends.
     */
    struct TimerTaskRunner : public Thread
    {
      TimerElement* te;
      TimerFactoryImpl* timer;

      TimerTaskRunner(TimerElement* te, TimerFactoryImpl* timer)
        : te(te), timer(timer) {}

      bool operator== (const TimerTaskRunner& ttr) const
      { return (te == ttr.te); }

      void* run(void* args = 0)
      {
//        std::cout << "[TimerTaskRunner]: executing handler " << te->handler << std::endl;

        if ( (!terminated()) && (te != 0) && (!te->cancelled) )
        {
          void* result = te->task->run( (void *)&te->handler);
          timer->timerTaskExecuted(this, result);
        }
        return 0;
      }
    };

  public:
    TimerFactoryImpl(HardwareTimer* hwtimer);
    ~TimerFactoryImpl();

    /**
     * @inherited TimerFactory
     */
    TIMER_HANDLER schedule(Runnable* task, long delay_ms, long period_ms = 0);

    /**
     * @inherited TimerFactory
     */
    bool cancel(TIMER_HANDLER handler);

    const posix::HandlerManager* getHandlerManager() const { return &hmgr; }

  protected:
    void timerTaskExecuted(TimerTaskRunner* ttr, void* result);

  private:
    /**
     * Schedule the given timer element.
     */
    void schedule(TimerElement* te);
    /**
     * Schedule the next entry in the queue.
     */
    void scheduleNext();

    /**
     * Terminate from user.
     * @deprecated
     *
    void terminateTimer(int handler);
     */

    /**
     * Terminate from inside.
     */
    void terminateTimer(TimerElement* te);

  public:
    /**
     * @see Listener
     */
    void handle(void* source, const void* data);

  private:
    /**
     * The hardware timer to be used: given.
     */
    HardwareTimer* hwtimer;

    /**
     * A handler manager.
     */
    posix::HandlerManager hmgr;
    /**
     * And the elements association.
     */
    std::map<int,TimerElement*> timers;

    /**
     * The priority queue to store pending timers.
     * This is an extension to priority_queue that allow removing of elements.
     */
    std::priority_queue<TimerElement*, std::vector<TimerElement*>, lstrTimerElement> pqueue;

    /**
     * A mutex to synchronize the access to methods.
     */
    Mutex mutex;

    /**
     * A thread manager to care about all those threads.
     */
    ThreadManager thrdmgr;
  };

} // namespace

#endif /* _TIMERFACTORYIMPL_H_ */
