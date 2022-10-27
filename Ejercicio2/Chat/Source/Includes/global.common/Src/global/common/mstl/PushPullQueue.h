#ifndef _PUSHPULLQUEUE_H_
#define _PUSHPULLQUEUE_H_

#include "../sys/sync/Monitor.h"
#include <queue>

#include <iostream>

/**
 * Implements a push-pull model queue to the produce-consume model.
 * <p>The consumer must call the retrieve <code>pull</code> method to get the
 * queue-buffered data, waiting with timeout for data.</p>
 * <p>This class is generic on the queue data type and can have a queue size
 * upper limit, working as a circular queue.</p>
 *
 * [2009.05.12] aya
 * Added methods front and pop to allow inspecting and remove of the first item
 * in the queue (and not both at once, as in pull).
 */
template <class T>
class PushPullQueue
{
public:
  PushPullQueue(unsigned maxSize = 0) : list(), m(), maxSize(maxSize) {}

  /**
   * The destructor dispose the contained elements.
   */
  ~PushPullQueue()
  {
    clear();
  }

  /**
   * Push an element onto the queue and signal a "not empty".
   */
  void push(T* data)
  {
    posix::Mutex::MutexLock lock(m, ".PushPullQueue::push");
    if ( (maxSize > 0) && (list.size() == maxSize))
      list.pop();
    list.push(data);
    m.signal();
  }

  /**
   * Pull an element from the queue, optionally waiting for data.
   */
  T* pull(long ms = 0)
  {
    posix::Mutex::MutexLock lock(m, ".PushPullQueue::pull");
    T* data = 0;

    if (list.size() == 0)
      m.timedWait(ms);
    if (list.size() > 0)
    {
      data = list.front();
      list.pop();
    }

    return data;
  }


  T* front(long ms = 0)
  {
    posix::Mutex::MutexLock lock(m, ".PushPullQueue::front");
    T* data = 0;

    if (list.size() == 0)
      m.timedWait(ms);
    if (list.size() > 0)
    {
      data = list.front();
    }

    return data;
  }
  void pop()
  {
    posix::Mutex::MutexLock lock(m, ".PushPullQueue::pop");

    if (list.size() > 0)
    {
      list.pop();
    }
  }

  /**
   * @return Current queue size.
   */
  int size()
  {
    posix::Mutex::MutexLock lock(m, ".PushPullQueue::size");
    return list.size();
  }

  /**
   * Dispose que queue content.
   */
  void clear()
  {
    posix::Mutex::MutexLock lock(m, ".PushPullQueue::clear");
    while (list.size() > 0)
    {
      T* data = list.front();
      list.pop();
      delete data;
    }
  }

private:
  std::queue<T*> list;
  posix::Monitor m;
  unsigned maxSize;
};

#endif /*_PUSHPULLQUEUE_H_*/
