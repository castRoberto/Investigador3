/*
 * Jun 20, 2009 - ayachan
 */

#include "TimerEntity.h"

#include "omanips.h"

namespace util
{

  TimerEntity::~TimerEntity()
  {
    getTimerFactoryInstance()->cancel(handler);
  }

  void* TimerEntity::run(void* args)
  {
    posix::TimerFactory::TIMER_HANDLER* ihandler =
      static_cast<posix::TimerFactory::TIMER_HANDLER*>(args);

//    std::cout << om_thread << " " << om_timef << " [TimerEntity::run]: " <<
//                 "called with handler " << *ihandler << std::endl;

    if ( (handler != posix::TimerFactory::INVALID_HANDLER_TIMER) &&
         (*ihandler == handler))
    {
      if (!periodic)
        handler = posix::TimerFactory::INVALID_HANDLER_TIMER;
      timerExecute();
    }
    return 0;
  }

  void TimerEntity::scheduleTimer(long delay_ms, long period_ms)
  {
    getTimerFactoryInstance()->cancel(handler);
    handler =
      getTimerFactoryInstance()->schedule(this, delay_ms, period_ms);
    periodic = (period_ms > 0);

//    std::cout << om_thread << om_time << "scheduleTimer: " <<
//                 " to " << delay_ms << "," << period_ms << " [ms]" <<
//                 " with handler " << handler << std::endl;
  }

  void TimerEntity::cancelTimer()
  {
    getTimerFactoryInstance()->cancel(handler);
    handler = posix::TimerFactory::INVALID_HANDLER_TIMER;
  }

}
