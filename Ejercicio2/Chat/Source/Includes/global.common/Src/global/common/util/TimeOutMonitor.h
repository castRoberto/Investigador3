#ifndef _TIMEOUTMONITOR_H_
#define _TIMEOUTMONITOR_H_

#include "TimerEntity.h"
#include "timestamp.h"

#include "../sys/sync/Mutex.h"

#include <string>
#include <iostream>

namespace util
{

  /**
   * Auxilliary class to keep track of timeouts.
   * A timeout has two values associated: the mean time and the time out time.
   * The mean time is the expected time between signals while the time out time
   * is the time that should trigger a time out warning.
   */
  class TimeOutMonitor : public TimerEntity
  {
  public:
    enum TimeOutStatus
    {
      /* The state of the time out is unknown, this could happen at start.
       */
      tosUNKNOWN = 0,
      /* Time elapsed from the last signal is less than the mean time.
       */
      tosOK,
      /* Time elapsed from the last signal (or the beginning) is between the mean
       * and the timeout value.
       */
      tosMEANTIME,
      /* Time elapsed from the las signal (or the beginning) exceded the timeout
       * value.
       */
      tosTIMEOUT
    };

    /**
     * Interface to time out status listeners.
     */
    class TimeOutMonitorListener
    {
    public:
      virtual ~TimeOutMonitorListener() {}
      /**
       * Callback with the timeout status, called when status change.
       */
      virtual void timeOutEvent(const TimeOutMonitor& to, const TimeOutStatus status) = 0;
    };

  private:
    static const int INVALID_HANDLER = -1;

    std::string name;

    long meanTime_ms;
    long timeoutTime_ms;
    TimeOutMonitorListener* listener;

    timestamp last;

    TimeOutStatus status;

    posix::Mutex mutex;

    /**
     * To store the parameter returned by the schedule method.
     */
    int handler;

    bool enabled;

  public:
    TimeOutMonitor(const std::string& name,
        long meanTime_ms, long timeoutTime_ms,
        TimeOutMonitorListener* listener);
    virtual ~TimeOutMonitor();

    bool operator == (const TimeOutMonitor& tom) const
    {
      return (name == tom.getName());
    }

    /**
     * Reset the timeout monitor (when, for instance, the heartbeat has been
     * detected).
     */
    void reset();

    /**
     * Allow enabling the timer set and callback generation.
     */
    void enable(bool enabled);
    bool isEnabled() const { return enabled; }

    const std::string& getName() const { return name; }
    long getMeanTime_ms() const { return meanTime_ms; }
    long getTimeoutTime_ms() const { return timeoutTime_ms; }
    TimeOutStatus getStatus() const { return status; }
    timestamp getTimestamp() const { return last; }

  protected:
    /**
     * Implementation of TimerEntity::execute
     */
    void timerExecute();

  protected:
    void resetTimeout(TimeOutStatus statusValue, long elapse_ms = 0);
    void setStatus(TimeOutStatus value);
  };

} // namespace

std::ostream& operator << (std::ostream& os, const util::TimeOutMonitor& tom);
std::ostream& operator << (std::ostream& os, const util::TimeOutMonitor* tom);

#endif /*_TIMEOUTMONITOR_H_*/
