/*
 * Delayed.h
 *
 *  Created on: Jul 22, 2016
 *      Author: ayachan
 */

#ifndef DELAYED_H_
#define DELAYED_H_

#include "../util/timestamp.h"

#include <iostream>

namespace util
{

  /**
   * Something to be store on queues and be able to be retrieved in specific
   * times.
   */
  struct Delayed
  {
	  virtual ~Delayed() {}
    /**
     * Returns the remaining delay associated with this object.
     */
    virtual long getDelay() const = 0;

    /*
     * (to work with priority queues (the greatest the first) we need to inverse
     * the order)
     *
     * Not any more, we work now with lists.
     */

    /*
     * This overload does not work (at least for delayed's and priority queues).
     * In that case, it needs to use the external comparator DelayedComparator.
     */
    bool operator <(const Delayed& other) const
    { return getDelay() < other.getDelay(); }

    virtual std::ostream& print(std::ostream& os) const = 0;
  };

  struct DelayedComparison
  {
    bool operator() (const Delayed& lhs, const Delayed& rhs) const
    {
      return (rhs.getDelay() < lhs.getDelay()); // inverse
    }
    bool operator() (const Delayed* lhs, const Delayed* rhs) const
    {
      return (rhs->getDelay() < lhs->getDelay()); // inverse
    }
  };

  /**
   * Structure of template operations suitable to be used in DelayQueueEx.
   */
  struct DelayedOps
  {
    static bool lt(const util::Delayed* first, const util::Delayed* second)
    {
//      std::cout << " -----? comparing " << first << " < " << second << std::endl;
      return first->getDelay() < second->getDelay();
    }

    static long delay(const util::Delayed* d)
    {
//      long delay = d->getDelay();
//      std::cout << " -----> retrieving delay " << delay << std::endl;
//      return delay;

      return d->getDelay();
    }
  };

  class TimedDelayed : public Delayed
  {
  public:
    TimedDelayed(const timestamp& ts) : ts(ts) {}
//    TimedDelayed(long elapse_ms) : ts(timestamp() + elapse_ms) {}
//    TimedDelayed(long elapse_ms) : ts(timestamp::timestampIn(elapse_ms))
    TimedDelayed(long elapse_ms)
    {
      struct timespec tspec;
      timestamp::abstime(tspec);

      double secs = 1.0 * tspec.tv_sec + tspec.tv_nsec / 1000000000.0;
      secs += (1.0 * elapse_ms / 1000.0);

      ts = secs;
//
//      std::cout <<
//          "[" << (void *)this << "]" <<
//          " generating TimedDelayed for " << elapse_ms << " [ms]" <<
//          ": generate delay of " << getDelay() <<
//          std::endl;
    }

    long getDelay() const
    {
      /*
       * Do not confuse by the minus, it is OK.
       * The delay time is in the future, so the elapse_ms() result is negative
       * prior their occurrence.
       */
      return -1 * (long)(ts.elapsed_ms());
    }

    /**
     * Utilities when managing delayed queues.
     */
    const timestamp getTimestamp()
    { return ts; }
    void setTimestamp(const timestamp& ts)
    { this->ts = ts; }

    virtual std::ostream& print(std::ostream& os) const
    {
      return os << '{' <<
          " TimedDelayed " << getDelay() << " [ms]" <<
          '}';
    }
  private:
    timestamp ts;
  };
}

std::ostream& operator<< (std::ostream& os, const util::Delayed& t);
std::ostream& operator<< (std::ostream& os, const util::Delayed* pt);

#endif /* DELAYED_H_ */
