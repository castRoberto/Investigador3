/*
 * Jul 20, 2009 - ayachan
 */

#include "ByteRingQueue.h"

#include <stdexcept>

namespace util
{

  ByteRingQueue::ByteRingQueue(int capacity)
    : mutex(), ring(), capacity(capacity)
  {
  }

  ByteRingQueue::~ByteRingQueue()
  {
  }

  int ByteRingQueue::get()
  {
    posix::Mutex::MutexLock lock(mutex, ".ByteRingQueue::get");

    if (ring.size() == 0)
      throw std::underflow_error("Ring is empty");

    int item = ring.front();
    ring.pop_front();
    return item;
  }

  int ByteRingQueue::get(char* buffer, int bufferlen)
  {
    posix::Mutex::MutexLock lock(mutex, ".ByteRingQueue::get");

    int got = 0;
    while ( (got < bufferlen) && (ring.size() > 0))
    {
      buffer[got] = ring.front();
      ring.pop_front();
      got++;
    }

    return got;
  }

  void ByteRingQueue::put(int uc)
  {
    posix::Mutex::MutexLock lock(mutex, ".ByteRingQueue::put");

    if (ring.size() == capacity)
      throw std::overflow_error("Ring is full");

    ring.push_back(uc);
  }

  int ByteRingQueue::put(const char* buffer, int bufferlen)
  {
    posix::Mutex::MutexLock lock(mutex, ".ByteRingQueue::put");

    int added = 0;
    while ( (added < bufferlen) && (ring.size() < capacity))
    {
      ring.push_back(buffer[added++]);
    }
    return added;
  }

  int ByteRingQueue::putFront(const char* buffer, int offset, int bufferlen)
  {
    posix::Mutex::MutexLock lock(mutex, ".ByteRingQueue::put");

    int added = 0;
    while ( (added < bufferlen) && (ring.size() < capacity))
    {
      ring.push_front(buffer[offset + added++]);
    }
    return added;
  }

  /**
   * (is free, because the capacity limit is just a queue use convention)
   */
  void ByteRingQueue::setCapacity(int capacity)
  {
    posix::Mutex::MutexLock lock(mutex, ".ByteRingQueue::setCapacity");

    this->capacity = capacity;
  }

  void ByteRingQueue::clear()
  {
    posix::Mutex::MutexLock lock(mutex, ".ByteRingQueue::clear");

    while (!ring.empty())
      ring.pop_front();
  }

} // namespace
