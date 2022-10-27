/*
 * BlockingQueue.h
 *
 *  Created on: Jul 22, 2016
 *      Author: ayachan
 */

#ifndef BLOCKINGQUEUE_H_
#define BLOCKINGQUEUE_H_

#include "../sys/sync/Monitor.h"
#include "../sys/sync/Mutex.h"

#include <deque>

namespace util
{

  /**
   * Essentially a translation of the ugly-named PushPullQueue.<br />
   * A queue where elements can be retrieved with time out.
   *
   * Need a not-an-element value to be returned when no elements are found after
   * the timeout.
   */
  template<class T>
  class BlockingQueue
  {
  public:
    BlockingQueue(const T& not_an_element)
      : not_an_element(not_an_element), m(), queue()
    {}
    virtual ~BlockingQueue() {}

  public:
    /*
     * (primary operations)
     */

    void push(const T& value)
    {
      posix::Mutex::MutexLock lock(m, ".BlockingQueue::push");
      queue.push_back(value);
      m.signal();
    }

    const T& front(long ms = 0)
    {
      posix::Mutex::MutexLock lock(m, ".BlockingQueue::front");
      if (queue.size() == 0)
        m.timedWait(ms);
      return (queue.size() > 0) ? queue.front() : not_an_element;
    }

    void pop()
    {
      posix::Mutex::MutexLock lock(m, ".BlockingQueue::pop");
      queue.pop_front();
    }

    int size() const
    {
      posix::Mutex::MutexLock lock(m, ".BlockingQueue::size");
      return queue.size();
    }

    void clear()
    {
      queue.clear();
    }

  private:
    T not_an_element;
    posix::Monitor m;

  protected:
    std::deque<T> queue;
  };

}

#endif /* BLOCKINGQUEUE_H_ */
