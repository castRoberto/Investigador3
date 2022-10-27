#ifndef _POSIXHARDWARETIMER_H_
#define _POSIXHARDWARETIMER_H_

#ifndef __MINGW32__

#include "HardwareTimer.h"

#include <signal.h>

namespace posix
{

  /**
   * Implements a hardware timer with POSIX.1b
   */
  class PosixHardwareTimer : public HardwareTimer
  {
  public:
    static const int DEFAULT_TIMER_SIGNAL = SIGALRM;

    PosixHardwareTimer(int timerSignal = DEFAULT_TIMER_SIGNAL);
    virtual ~PosixHardwareTimer();

    /**
     * @see HardwareTimer
     */
    void setHardwareTimer(long interval_us);
    void setHardwareTimer(struct timespec ts);
    /**
     * @see HardwareTimer
     */
    long getHardwareTimer();
    /**
     * @see HardwareTimer
     */
    void stopHardwareTimer();

    /**
     * @see HardwareTimer
     */
    int catchHardwareInterrupt(Listener* listener);

    /**
     * @see HardwareTimer
     */
    void blockTimerInterrupt();
    /**
     * @see HardwareTimer
     */
    void unblockTimerInterrupt();
    /**
     * @see HardwareTimer
     */
    bool isTimerInterruptBlocked();

    /**
     * @see HardwareTimer
     */
    void waitForInterrupt();

  protected:
    timer_t getTimerId() { return timerId; }
    Listener* getListener() { return listener; }

  private:
    static void signalHandler(int signo, siginfo_t* info, void *context);

  private:
    /**
     * The signal used to receive the timer interrupt.
     */
    int timerSignal;

    /**
     * Flag to know about timer initialization.
     */
    bool initialized;
    /**
     * The POSIX timer itself.
     */
    timer_t timerId;
    /**
     * This method will be called when timer occurs.
     */
    Listener* listener;

  };

} // namespace

#endif

#endif /*_POSIXHARDWARETIMER_H_*/
