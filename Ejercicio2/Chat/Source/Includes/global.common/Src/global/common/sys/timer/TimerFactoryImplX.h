#ifndef _TIMERFACTORYIMPLX_H_
#define _TIMERFACTORYIMPLX_H_

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

#include <list>
#include <map>
#include <iostream>

namespace posix
{

  /**
   * Alternative implementation based on a simple list (plus some other little
   * differences).
   *
   * All timers are created as TimerElement's and stored in a map indexed by
   * their (created) handler. Also the TimerElement is (sorted) inserted in a
   * list so the list front is the nearest timer.
   * A POSIX hardware timer is used to match the time of the nearest timer. It
   * is stopped when a timer is created or updated and reschedule to the current
   * nearest timer.
   *
   *
   */
  class TimerFactoryImplX : public TimerFactory, public Listener
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
    struct TimerElementOps
    {
      static bool lt(const TimerElement* first, const TimerElement* second)
      {
        return first->ts < second->ts;
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
      TimerFactoryImplX* timer;

      TimerTaskRunner(TimerElement* te, TimerFactoryImplX* timer)
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
    TimerFactoryImplX(HardwareTimer* hwtimer);
    ~TimerFactoryImplX();

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
    std::map<int, TimerElement*> timers;

    /**
     * The list to store pending timers.
     */
    std::list<TimerElement*> list;

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

#endif /* _TIMERFACTORYIMPLX_H_ */
