//#pragma once

#ifndef _TIMEDCHECK_H_
#define _TIMEDCHECK_H_

/*
 * Jul 15, 2009 - ayachan
 */

#include "timestamp.h"

#include "../sys/thread/Thread.h"

namespace util
{

  /**
   * Utility class to check for timeouts.
   */
  class TimedCheck
  {
  public:
    /**
     * Wait for a given elapse time.
     */
    static void wait(long elapse_ms)
    {
      TimedCheck tchk(elapse_ms);
      while (tchk.isOnTime())
      {
        posix::Thread::sleep(1000);
      }
    }

  public:
    TimedCheck(long elapse_ms = 0) { reset(elapse_ms); }

    void reset(long elapse_ms)
    {
      this->elapse_ms = elapse_ms;
      timestamp::abstime(start);
    }

    bool isTimedOut() const
    { return (elapsed() >= elapse_ms); }

    bool isOnTime() const
    { return (elapsed() < elapse_ms); }

    long elapsed() const
    { return (long)(timestamp::getTimestamp() - start); }

  private:
    long elapse_ms;
    timestamp start;
  };

} // namespace

#endif /* TIMEDCHECK_H_ */
