#ifndef _TIMERENTITY_H_
#define _TIMERENTITY_H_

/*
 * Jun 20, 2009 - ayachan
 */

#include "../global/Runnable.h"
#include "../sys/timer/TimerInstance.h"

namespace util
{

  /**
   * This class encapsulates a timer requirement to assure that things occurs in
   * the right way.
   * Objects that wants to receive timers should derive from this class and use
   * this class method's to manage the timing.
   * TimerEntity's can have only one schedule at each time, so if a new schedule
   * is called, the existing one, if any, is cancelled.
   */
  class TimerEntity : public Runnable
  {
  public:
    virtual ~TimerEntity();

    /**
     * @inherited Runnable: DO NOT INHERIT THIS
     */
    void* run(void* args = 0);

    /**
     * Schedule the timer with the given parameters, cancelling the existing one
     * if any.
     */
    void scheduleTimer(long delay_ms, long period_ms = 0);
    /**
     * Cancel the current scheduled timer.
     */
    void cancelTimer();

  protected:
    /**
     * Only can be used from implementers.
     */
    TimerEntity()
      : handler(posix::TimerFactory::INVALID_HANDLER_TIMER), periodic(false) {}

    /**
     * Called when the timer was triggered.
     * Must be overloaded by implementers.
     */
    virtual void timerExecute() = 0;

  private:
    posix::TimerFactory::TIMER_HANDLER handler;
    bool periodic;
  };
}

#endif /* TIMERENTITY_H_ */
