/*
 * QueueEx.h
 *
 *  Created on: Nov 17, 2016
 *      Author: ayachan
 */

#ifndef _BLOCKINGQUEUEEX_H_
#define _BLOCKINGQUEUEEX_H_

#include "IBlockingQueue.h"

#include "../sys/sync/Monitor.h"

#include <algorithm>
#include <list>

#include <iostream>

namespace util
{

  /**
   * Standard blocking queue that can retrieve the next element in the queue
   * waiting for a time.
   * Elements are retrieved exactly in the insertion order (FIFO).
   */
  template <class T>
  class BlockingQueueEx : public IBlockingQueue<T>
  {
  public:
    BlockingQueueEx() : IBlockingQueue<T>(), queue() {}

  public:
    void push(const T* value)
    {
      posix::Mutex::MutexLock lock(m, ".QueueEx::push");
      queue.push_back(value);
      m.signal();
    }

    const T* front(long ms = 0)
    {
      posix::Mutex::MutexLock lock(m, ".QueueEx::front");
      if (queue.size() == 0)
        m.timedWait(ms, "BlockingQueueEx");
      const T* result = 0;
      if (queue.size() > 0)
      {
        result = queue.front();
        queue.remove(result);
      }
      return result;
//      return (queue.size() > 0) ? queue.front() : 0;
    }

    bool remove(const T* element)
    {
      posix::Mutex::MutexLock lock(m, ".QueueEx::remove");

      bool result = false;
      typename std::list<const T*>::iterator iter =
          find(queue.begin(), queue.end(), element);
      if (iter != queue.end())
      {
        queue.erase(iter);
        m.signal();
        result = true;
      }
      else
        std::cout << " missing removing element" << std::endl;
      return result;
    }

    void pop()
    {
      posix::Mutex::MutexLock lock(m, ".QueueEx::pop");
      queue.pop_front();
    }

    int size() const
    {
      return queue.size();
    }

    void clear()
    {
      queue.clear();
    }

  protected:
    posix::Monitor m;
    std::list<const T*> queue;
  };

}

#endif /* _BLOCKINGQUEUEEX_H_ */
