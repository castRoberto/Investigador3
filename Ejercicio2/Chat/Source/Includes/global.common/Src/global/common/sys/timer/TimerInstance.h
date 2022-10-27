#ifndef TIMERINSTANCE_H_
#define TIMERINSTANCE_H_

/*
 * Define a facility to get a platform-independent timer instance.
 * Oct 31, 2008 - Alejandro
 */

#ifdef __MINGW32__
#include "MingwTimer.h"
#else
#include "PosixHardwareTimer.h"
#endif

#include "TimerImpl.h"
//#include "AnotherTimerImpl.h"
#include "TimerFactory.h"

class TimerInstance
{
public:

  /**
   * Get a timer instance.
   * Hard recomendation to use as a singleton, ie: GET ONLY ONE!
   *
   */
  static posix::Timer* getInstance()
  {
    posix::Timer* timer = 0;
    posix::HardwareTimer* hwtimer = 0;

#ifdef __MINGW32__
    hwtimer = new mingw::MingwTimer();
#else
    hwtimer = new posix::PosixHardwareTimer();
#endif

    if (hwtimer != 0)
      timer = new posix::TimerImpl(hwtimer);

    return timer;
  }

  /**
   * Get the other timer instance.
   * Hard recomendation to use as a singleton, ie: GET ONLY ONE!
   *
  static posix::AnotherTimer* getAnotherInstance()
  {
    posix::AnotherTimer* timer = 0;
    posix::HardwareTimer* hwtimer = 0;

#ifdef __MINGW32__
    hwtimer = new mingw::MingwTimer();
#else
    hwtimer = new posix::PosixHardwareTimer();
#endif

    if (hwtimer != 0)
      timer = new posix::AnotherTimerImpl(hwtimer);

    return timer;
  }
   *
   */

};

/**
 * Get the other timer instance, ever as a singleton.
 */
posix::TimerFactory* getTimerFactoryInstance();

#endif /* TIMERINSTANCE_H_ */
