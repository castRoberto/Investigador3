/*
 * DelayQueue.h
 *
 *  Created on: Nov 15, 2016
 *      Author: ayachan
 */

#ifndef _DELAYQUEUEEX_H_
#define _DELAYQUEUEEX_H_

#include "IBlockingQueue.h"

#include "../sys/sync/Monitor.h"
#include "../util/timestamp.h"

#include <algorithm>

#include <list>

#include "../util/omanips.h"
#include <iostream>

namespace util
{

  /**
   * Blocking queue where elements are sorted.
   * The T template should be an object type that can be asked for their
   * remaining time to be activated.
   * The Op template should provide:
   *  - the 'lt' method to compare two T elements
   *      boolean lt(const T* lhs, const T* rhs)
   *  - the 'delay' method to obtain the remaining delay of an T element
   *      long delay(const T* obj)
   */
  template <class T, class Op>
  class DelayQueueEx : public IBlockingQueue<T>
  {

  public:
    DelayQueueEx()
      : IBlockingQueue<T>(), m(), queue() {}

  public:
    void push(const T* value)
    {
      posix::Mutex::MutexLock lock(m, ".DelayQueueEx::push");
      typename std::list<const T*>::iterator next =
          upper_bound(queue.begin(), queue.end(), value, Op::lt);
      queue.insert(next, value);
      m.signal();
    }

    const T* front(long ms = 0)
    {
      posix::Mutex::MutexLock lock(m, ".DelayQueueEx::front");
      util::timestamp tsnow;

      long topDelay = queue.empty() ? LONG_MAX : Op::delay(queue.front());
//      std::cout << om_timef << " --> top delay is " << topDelay << " [ms]" << std::endl;
      while
        ( (tsnow.elapsed_ms() < ms) &&
          (queue.empty() || (Op::delay(queue.front()) > 0)))
      {
        /*
         * Someone could insert an element in the meantime!
         */
        topDelay = queue.empty() ? LONG_MAX : Op::delay(queue.front());
//        std::cout << om_timef << " --> loop top delay is " << topDelay << " [ms]" << std::endl;
        if (topDelay <= 0)
          break;

        long remaining = ms - (long) tsnow.elapsed_ms();
        if (remaining <= 0)
          break;

        long waitTime_ms = (topDelay < remaining) ? topDelay : remaining;
//        std::cout << om_timef << "     waiting " << waitTime_ms << " [ms]" << std::endl;
        m.timedWait(waitTime_ms);
      }

      const T* result = 0;
      if (queue.empty() || (Op::delay(queue.front()) > 0))
        result = 0;
      else
      {
        result = queue.front();
        queue.remove(result);
      }
      return result;
    }

    bool remove(const T* element)
    {
      posix::Mutex::MutexLock lock(m, ".DelayQueueEx::remove");

      bool result = false;

      for (
          typename std::list<const T*>::iterator iter = queue.begin();
          iter != queue.end(); ++iter)
      {
        if (*iter == element)
        {
          queue.erase(iter);
          result = true;
          break;
        }
      }
      if (!result)
//        std::cout << " missing removing element" << std::endl;

//      typename std::list<const T*>::iterator iter =
//          find(queue.begin(), queue.end(), element);
//      if (iter != queue.end())
//      {
//        queue.erase(iter);
//        result = true;
//      }
//      else
//        std::cout << " missing removing element" << std::endl;

      return result;
    }

    void pop()
    {
      if (queue.empty() || (Op::delay(queue.front()) > 0))
      {
        // not yet
      }
      else
      {
        posix::Mutex::MutexLock lock(m, ".DelayQueueEx::pop");
//        std::cout << om_timef << " --> pop top with delay " <<
//            Op::delay(queue.front()) << std::endl;
        queue.pop_front();
      }
    }

    int size() const
    {
      return queue.size();
    }

    void clear()
    {
      queue.clear();
    }

  private:
    posix::Monitor m;
    std::list<const T*> queue;
  };
}

#endif /* _DELAYQUEUEEX_H_ */
