/*
 * Jan 20, 2009 - ayachan
 */

#include "TimeOutMonitor.h"

#include "../sys/timer/TimerInstance.h"
#include "omanips.h"

namespace util
{

  TimeOutMonitor::TimeOutMonitor(const std::string& name,
      long meanTime_ms, long timeoutTime_ms,
      TimeOutMonitorListener* listener)
    : TimerEntity(),
      name(name), meanTime_ms(meanTime_ms), timeoutTime_ms(timeoutTime_ms),
      listener(listener),
      last(), status(tosUNKNOWN), mutex(), handler(INVALID_HANDLER),
      enabled(false)
  {
//    std::cout << "[TimeOutMonitor." << name << "]: created for mean " << meanTime_ms <<
//                 ", timeout " << timeoutTime_ms << std::endl;

//    setTimer(meanTime_ms);
  }
  TimeOutMonitor::~TimeOutMonitor()
  {
////    std::cout << "[TimeOutMonitor." << name << "]: destroying " << std::endl;
//    if (enabled)
//      cancelTimer();

    // (now cancelled in all cases)
  }

  void TimeOutMonitor::enable(bool enabled)
  {
    posix::Mutex::MutexLock mlock(mutex, ".TimeOutMonitor::enable");

    if (this->enabled != enabled)
    {
//      std::cout << "[TimeOutMonitor." << name << "]: " << (enabled ? "ENABLED" : "DISABLED") << std::endl;

      this->enabled = enabled;

      if (enabled)
      {
        resetTimeout(tosUNKNOWN, meanTime_ms);
      }
      else
      {
        setStatus(tosUNKNOWN);
        cancelTimer();
      }
//
//      resetTimeout(tosUNKNOWN, meanTime_ms);
//      if (!this->enabled)
//        cancelTimer();
//
////      if (this->enabled)
////        setTimer(meanTime_ms);
////      else
////        cancelTimer();
    }
  }

  void TimeOutMonitor::reset()
  {
    posix::Mutex::MutexLock mlock(mutex, ".TimeOutMonitor::reset");
//    std::cout << "[TimeOutMonitor." << name << "]: waiting for MEANTIME for " << meanTime_ms << " [ms]" << std::endl;

    cancelTimer();
    timestamp::abstime(last);
    resetTimeout(tosOK, meanTime_ms);
  }

  void TimeOutMonitor::resetTimeout(TimeOutStatus statusValue, long elapse_ms)
  {
//    if (this->status == status)
//      return;

//    posix::Mutex::MutexLock mlock(mutex);

    setStatus(statusValue);

    if (enabled)
    {
      if (elapse_ms > 0)
        scheduleTimer(elapse_ms);
    }
    else
    {
      std::cout << "[TimeOutMonitor]: ERROR: invalid time to wait " <<
                   elapse_ms << std::endl;
    }
  }

  void TimeOutMonitor::setStatus(TimeOutStatus value)
  {
    if (status == value)
      return;

    status = value;
    if (listener != 0)
      listener->timeOutEvent(*this, status);
  }

  /**
   * Callback from timer.
   * If arrived, the timer was triggered and the handler becomes invalid.
   */
  void TimeOutMonitor::timerExecute()
  {
    posix::Mutex::MutexLock mlock(mutex, ".TimeOutMonitor::timerExecute");

//    int* incommingHandler = static_cast<int*>(args);
//    if (*incommingHandler != handler)
//    {
////      std::cout << "%%%%%% TE PILLE DESGRACIADO! %%%%%%" <<
////                   " (" << handler << "," << *incommingHandler << ")" <<
////                   std::endl;
//      return 0;
//    }
//
////    std::cout << "++++++ (timer." << name << " | " << handler << ") ---> " << *this << std::endl;
//    handler = INVALID_HANDLER;

    std::cout << om_thread << om_time << "[TimeOutMonitor::timerExecute]: " << this << std::endl;

    switch (status)
    {
      case tosUNKNOWN:
      case tosOK:
        // either unknown or ok, the timer for mean time has occur
        // (need to wait only the difference, already wait the meantime)
//          std::cout << "[TimeOutMonitor." << name <<
//                       "]: waiting for TIMEOUT for " <<
//                       (timeoutTime_ms - meanTime_ms) << " [ms] " <<
//                       util::timestamp::getTimestamp() << std::endl;
        resetTimeout(tosMEANTIME, timeoutTime_ms - meanTime_ms);
        break;
      case tosMEANTIME:
        // the timer for timeout has occur, no other timer necesary
//          std::cout << "[TimeOutMonitor." << name <<
//                       "]: TIMEOUT status " <<
//                       util::timestamp::getTimestamp() << std::endl;
        resetTimeout(tosTIMEOUT);
        break;
      case tosTIMEOUT:
      default:
        // no action
        /*
         * (not an error all times)
         *
        std::cerr << "Inconsistent timer when status " << status << std::endl;
         */
        ;
    }
  }

} // namespace

std::ostream& operator << (std::ostream& os, const util::TimeOutMonitor& tom)
{
  return os << &tom;
}
std::ostream& operator << (std::ostream& os, const util::TimeOutMonitor* tom)
{
  return os << '{' << tom->getName() <<
               ',' << tom->getMeanTime_ms() <<
               ',' << tom->getTimeoutTime_ms() <<
               ',' << tom->getStatus() /*<<
               ',' << (long)tom->getTimestamp()*/ << '}';
}
