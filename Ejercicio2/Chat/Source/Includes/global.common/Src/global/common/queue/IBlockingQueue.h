/*
 * IBlockingQueue.h
 *
 *  Created on: Nov 15, 2016
 *      Author: ayachan
 */

#ifndef _IBLOCKINGQUEUE_H_
#define _IBLOCKINGQUEUE_H_

namespace util
{

  /**
   * Alternative family of classes around blocking queue that include
   * BlockingQueueEx, DelayQueueEx, RuledDelayQueueEx and EventQueueEx
   */
  template <class T>
  struct IBlockingQueue
  {
    virtual ~IBlockingQueue() {}

    /**
     * Add an element to the queue.
     */
    virtual void push(const T* value) = 0;
    /**
     * Get an element to the queue waiting for it up to the given elapse.
     * Returns 0 if no element is retrieved from the queue.
     * IMPORTANT
     * As opposed to traditional C++ queues, the element *is* removed from the
     * queue.
     */
    virtual const T* front(long ms = 0) = 0;
    /**
     * Remove the given element from the queue.
     * Returns 0 if the element is not found in the queue.
     */
    virtual bool remove(const T* element) = 0;
    /**
     * Remove the top element in the queue.
     */
    virtual void pop() = 0;

    /**
     * Returns the current size of the queue.
     */
    virtual int size() const = 0;
    /**
     * Clear all elements in the queue.
     */
    virtual void clear() = 0;
  };
}

#endif /* _IBLOCKINGQUEUE_H_ */
