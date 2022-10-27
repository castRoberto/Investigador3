#ifndef _TIMER_H_
#define _TIMER_H_

#include "TimerTask.h"

namespace posix
{

  /**
   * Defines the interface for a timer class.
   */
  class Timer
  {
  public:
    virtual ~Timer() {}

    /**
     * Cancel the given task or all.
     */
    virtual void cancel(TimerTask* task = 0) = 0;
    /**
     * Programs a timer to be executed in 'delay_ms' [ms]. If 'period_ms' is non
     * zero, the task will be reprogrammed with that period.
     */
    virtual void schedule(TimerTask* task, long delay_ms,
                          long period_ms = 0) = 0;
  };

} // namespace

#endif /*_TIMER_H_*/
