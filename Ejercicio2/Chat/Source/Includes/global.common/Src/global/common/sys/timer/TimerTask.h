#ifndef _TIMERTASK_H_
#define _TIMERTASK_H_

#include "../../global/Runnable.h"
#include <assert.h>

namespace posix
{

  class Timer;

  /**
   * The class that represents the task to be executed when the timer comes up.
   * The method Runnable::run will be called and must be implemented.
   *
   * [2009.01.20]
   * Bug found: the canceled condition cannot be in this instance.
   * Suppose that this task was schedule for, say, 10 seconds ahead and then
   * canceled and re-schedule to 6 seconds. The current cancel algorithm leave
   * the task on the timer queue, so we will have TWO entries on the
   * queue: the first at 6 seconds and the seconds at 10 seconds.
   *
   * The correction choice is to associate the cancel condition to the queue
   * entry in place to the task, but, in this case, the schedule method should
   * return that entry in order the caller have the choice to cancel it.
   *
   * Also, if we extract the cancel condition from this class, it remains only
   * the Runnable interface, so the TimerTask become unnecessary.
   *
   * @deprecated HAVE A BUG IF RE-SCHEDULED
   */
  class TimerTask : public Runnable
  {
    friend class timer;
  public:
    TimerTask() : timer(0), cancelled(false) {}
    /**
     * Cancel the current task, as apply.
     */
    void cancel();

    /**
     * This method will be called by the timer itself when appropriate.
     */
    void setTimer(Timer* timer) { this->timer = timer; }

    void setCancelled(bool value = true) { cancelled = value; }
    bool isCancelled() { return cancelled; }
  protected:
  private:
    Timer* timer;
    bool cancelled;
  };

} // namespace

#endif /*_TIMERTASK_H_*/
