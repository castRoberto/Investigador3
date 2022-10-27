/*
 * Jan 28, 2009 - ayachan
 */

#include "TimerInstance.h"
#include "TimerFactoryImplX.h"

posix::TimerFactory* getTimerFactoryInstance()
{
#ifdef __MINGW32__
  static mingw::MingwTimer hwtimer;
#else
  static posix::PosixHardwareTimer hwtimer;
#endif
  static posix::TimerFactoryImplX timer(&hwtimer);

  return &timer;
}
