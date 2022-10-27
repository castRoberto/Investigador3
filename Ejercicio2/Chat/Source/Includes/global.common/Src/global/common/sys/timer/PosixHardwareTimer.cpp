
#ifndef __MINGW32__

#include <time.h>
#include "PosixHardwareTimer.h"
#include "../../util/timespec.h"

#include <assert.h>

//#include <unistd.h>
//#include <signal.h>

#include <string.h>
#include <stdio.h>

namespace posix
{

  PosixHardwareTimer::PosixHardwareTimer(int timerSignal)
    : timerSignal(timerSignal), initialized(false), timerId(0), listener(0)
  {
  }

  PosixHardwareTimer::~PosixHardwareTimer()
  {
    if (initialized)
    {
      // first cancel cancel if active
      stopHardwareTimer();
      // then dis-associate the timer
      if (timer_delete(timerId) != 0)
        perror("[PosixHardwareTimer]: can't delete timer");
      // finally dis-associate the handler
      sigaction(timerSignal, 0, 0);
    }
  }

  /**
   * This is the method/handler called by the signal.
   * Because is static knows about the calling instance in the arguments.
   */
  void PosixHardwareTimer::signalHandler(int signo, siginfo_t* info, void *context)
  {
    PosixHardwareTimer* myself = (PosixHardwareTimer*)info->si_value.sival_ptr;
    if (myself == 0)
    {
      std::cout << "**** PosixHardwareTimer is missing!" << std::endl;
      return;
    }
//    else
//      std::cout << "**** PosixHardwareTimer is at " <<
//                   std::hex << myself << std::dec <<
//                   std::endl;
    if (myself->getListener() != 0)
      myself->getListener()->handle(myself, 0);
  }

  void PosixHardwareTimer::setHardwareTimer(long interval_us)
  {
    assert(initialized);

  //  printf("[PosixHardwareTimer::setHardwareTimer]: %ld [us]\n", interval_us);
    setHardwareTimer(abstime() + interval_us);
  }

  void PosixHardwareTimer::setHardwareTimer(struct timespec ts)
  {
    assert(initialized);

  //  printf("[PosixHardwareTimer::setHardwareTimer]: %ld,%ld\n",
  //         (long)ts.tv_sec, ts.tv_nsec);

    struct itimerspec value = { { 0 }, { 0 } };
    value.it_value = ts;
    if (timer_settime(timerId, TIMER_ABSTIME, &value, 0) != 0)
      perror("[PosixHardwareTimer::setHardwareTimer]: can't set timer");
  }

  long PosixHardwareTimer::getHardwareTimer()
  {
    assert(initialized);

    struct itimerspec value = { { 0 }, { 0 } };
    if (timer_gettime(timerId, &value) != 0)
      perror("[PosixHardwareTimer::getHardwareTimer]: can't get time");
    return (long)timespec_to_us(value.it_value);
  }

  void PosixHardwareTimer::stopHardwareTimer()
  {
    assert(initialized);

    struct itimerspec value = { { 0 }, { 0 } };
    if (timer_settime(timerId, 0, &value, 0) != 0)
      perror("[PosixHardwareTimer::stopHardwareTimer]: can't stop timer");
  }


  int PosixHardwareTimer::catchHardwareInterrupt(Listener* listener)
  {
    this->listener = listener;

    /*
     * With the current behavior, this method can be called just to change the
     * listener withouth affect the timer itself.
     */
    if (initialized)
      return 0;

    struct sigaction act; memset(&act, 0, sizeof(act));
    struct sigevent evp; memset(&evp, 0, sizeof(evp));

    int success = 0;

    sigemptyset(&act.sa_mask);
    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = PosixHardwareTimer::signalHandler;
    if ( (success = sigaction(timerSignal, &act, NULL)) < 0)
    {
      perror("[PosixHardwareTimer]: sigaction failed");
    }
    if (success == 0)
    {
      evp.sigev_notify = SIGEV_SIGNAL;
      evp.sigev_signo = SIGALRM;
      evp.sigev_value.sival_ptr = this; // le paso mi instancia!
      if ( (success = timer_create(CLOCK_REALTIME, &evp, &timerId)) < 0)
      {
        perror("[PosixHardwareTimer]: Could not create a timer");
      }
      else
        initialized = true;
    }
    return success;
  }


  void PosixHardwareTimer::blockTimerInterrupt()
  {
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, timerSignal);
    if (sigprocmask(SIG_BLOCK, &mask, 0) < 0)
      perror("[PosixHardwareTimer]: can't block signal");
  }

  void PosixHardwareTimer::unblockTimerInterrupt()
  {
    sigset_t mask;

    sigemptyset(&mask);
    sigaddset(&mask, timerSignal);
    if (sigprocmask(SIG_UNBLOCK, &mask, 0) < 0)
      perror("[PosixHardwareTimer]: can't unblock signal");
  }

  bool PosixHardwareTimer::isTimerInterruptBlocked()
  {
    sigset_t mask;

    sigemptyset(&mask);
    if (sigprocmask(SIG_BLOCK, 0, &mask) < 0)
      perror("[PosixHardwareTimer]: can't read blocked signals");

    return (sigismember(&mask, timerSignal) == 1);
  }


  void PosixHardwareTimer::waitForInterrupt()
  {
    // don't used
  }

} // namespace

#endif
