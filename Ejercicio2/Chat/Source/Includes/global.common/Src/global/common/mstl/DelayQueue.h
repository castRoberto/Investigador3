/*
 * DelayQueue.h
 *
 *  Created on: Jul 22, 2016
 *      Author: ayachan
 */

#ifndef DELAYQUEUE_H_
#define DELAYQUEUE_H_

#include "Delayed.h"

#include "../sys/sync/Monitor.h"
#include "../util/timestamp.h"
#include "../util/omanips.h"

#include <iostream>

#include <climits>
#include <algorithm>

#include <list>

namespace util
{

  /**
   * A queue that hold <code>Delayed</code>'s and release them in the time of
   * the event. It works like a blocking queue with the exception that it
   * returns something only if the time of that event expired.
   */
  class DelayQueue
  {
  private:
    static bool listDelayQueueCompare(const Delayed* first, const Delayed* second)
    {
      return *first < *second;
    }

  public:
    DelayQueue() : m(), queue() {}

  public:
    void push(const Delayed* value)
    {
      posix::Mutex::MutexLock lock(m, ".DelayQueue::push");

      // just add (orderly)
      std::list<const Delayed*>::iterator next =
          upper_bound(queue.begin(), queue.end(), value, listDelayQueueCompare);
      queue.insert(next, value);

//      std::cout << om_timef << " --> pushing delay " << value->getDelay() <<
//          ", size = " << queue.size() << std::endl;

      m.signal();
    }

    const Delayed* front(long ms = 0)
    {
      posix::Mutex::MutexLock lock(m, ".DelayQueue::front");
      util::timestamp tsnow;

      long topDelay = queue.empty() ? LONG_MAX : queue.front()->getDelay();
//      std::cout << om_timef << " --> top delay is " << topDelay << " [ms]" << std::endl;
      while
        ( (tsnow.elapsed_ms() < ms) &&
          (queue.empty() || (queue.front()->getDelay() > 0)))
      {
        /*
         * Someone could insert an element in the meantime!
         */
        topDelay = queue.empty() ? LONG_MAX : queue.front()->getDelay();
//        std::cout << om_timef << " --> loop top delay is " << topDelay << " [ms]" << std::endl;
        if (topDelay <= 0)
          break;

        long remaining = ms - (long) tsnow.elapsed_ms();
        if (remaining <= 0)
          break;

        long waitTime_ms = (topDelay < remaining) ? topDelay : remaining;
        if (waitTime_ms <= 0)
          break;
//        std::cout << om_timef << "     waiting " << waitTime_ms << " [ms]" << std::endl;
        m.timedWait(waitTime_ms);
      }

      const Delayed* result =
          (queue.empty() || (queue.front()->getDelay() > 0)) ? 0 : queue.front();
      return result;
    }

    void pop()
    {
      if (queue.empty() || (queue.front()->getDelay() > 0))
        // not yet
        ;
      else
      {
//        std::cout << om_timef << " --> pop top with delay " <<
//            queue.front()->getDelay() << std::endl;
        posix::Mutex::MutexLock lock(m, ".DelayQueue::pop");
        queue.pop_front();
      }
    }

    int size() const
    {
//      posix::Mutex::MutexLock lock(m, ".DelayQueue::size");
      return queue.size();
    }

    const Delayed* remove(const Delayed* value)
    {
      posix::Mutex::MutexLock lock(m, ".DelayQueue::remove");

      const Delayed* result = 0;

      std::list<const Delayed*>::iterator elem =
          find(queue.begin(), queue.end(), value);
      if (elem != queue.end())
      {
        result = *elem;
        queue.erase(elem);
      }

      return result;
    }

  private:
    posix::Monitor m;

  private:
    std::list<const Delayed*> queue;
  };
}

#endif /* DELAYQUEUE_H_ */
