#ifndef _HARDWARETIMER_H_
#define _HARDWARETIMER_H_

#include "../../global/Listener.h"

#include <time.h>
#ifdef __MINGW32__
#include <pthread.h>
#endif

namespace posix
{

  /**
   * Defines an interface to a hardware timer.
   * Designed to be implemented with POSIX timers, either Spec 1170 or POSIX.1b.
   * Implementations manage JUST ONE hardware timer.
   *
   * Inspired on the book "Practical Unix Programming".
   *
   * 2008.10.31 aya
   * Refactored to enable it to be used under windows mingw: all (currently)
   * unused methods was leave as protected (to trigger compilation warnings).
   */
  class HardwareTimer
  {
  public:
    virtual ~HardwareTimer() {}

    /**
     * State a timer for the given time in timespec.
     */
    virtual void setHardwareTimer(struct timespec ts) = 0;
    /**
     * Stops the hardware timer.
     */
    virtual void stopHardwareTimer() = 0;
    /**
     * State the method to be called when timer comes.
     */
    virtual int catchHardwareInterrupt(Listener* listener) = 0;

  protected:
    /**
     * Retrieve remaining time of the current timer, in [us].
     */
    virtual long getHardwareTimer() = 0;

    /**
     * State a timer for the given time in [us].
     */
    virtual void setHardwareTimer(long interval_us) = 0;

    /**
     * Block the hardware timer interrupt.
     */
    virtual void blockTimerInterrupt() = 0;
    /**
     * Unblock the hardware timer interrupt.
     */
    virtual void unblockTimerInterrupt() = 0;
    /**
     * Returns true if timer interrupt is blocked.
     */
    virtual bool isTimerInterruptBlocked() = 0;

    /**
     * Waits for any interrupt.
     */
    virtual void waitForInterrupt() = 0;
  };

} // namespace

#endif /*_HARDWARETIMER_H_*/
