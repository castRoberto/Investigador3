//#pragma once

#ifndef _BYTERINGQUEUE_H_
#define _BYTERINGQUEUE_H_

/*
 * Jul 20, 2009 - ayachan
 */

#include "../sys/sync/Mutex.h"

#include <list>

namespace util
{

  /**
   * Define a buffer that works as a ring queue of chars.
   */
  class ByteRingQueue
  {
  public:
    ByteRingQueue(int capacity);
    virtual ~ByteRingQueue();

  public:
    /**
     * Get the next item in the queue.
     * Raise an exception if the queue is empty. Check with size() before.
     */
    int get();
    /**
     * Get as far as bufferlen items of the queue.
     * @return The number of items actually extracted from the queue, eventually
     *         zero.
     */
    int get(char* buffer, int bufferlen);

    /**
     * Add an item to the queue.
     * Raise an exception if the queue is full. Check with isFull() before.
     */
    void put(int uc);

    /**
     * Add as long as bufferlen items to the queue.
     * @return The number of items actually added to the queue, eventually less
     *         than the given number if the queue turns full.
     */
    int put(const char* buffer, int bufferlen);

    /**
     * Add to the front of the queue.
     */
    int putFront(const char* buffer, int offset, int bufferlen);

    int getCapacity() const { return capacity; }
    /**
     * Set a new capacity to the queue.
     * Existing data is preserved as far as possible.
     */
    void setCapacity(int capacity);

    unsigned int size() const { return ring.size(); }

    /**
     * Clear all items in the queue.
     */
    void clear();

  private:
    posix::Mutex mutex;
    std::list<char> ring;
    unsigned int capacity;
  };

} // namespace

#endif /* _BYTERINGQUEUE_H_ */
