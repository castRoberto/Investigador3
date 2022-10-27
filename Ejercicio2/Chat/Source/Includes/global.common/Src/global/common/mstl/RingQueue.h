#ifndef _RINGQUEUE_H_
#define _RINGQUEUE_H_

#include "../sys/sync/Mutex.h"

/*
 * Simple array implementation of a circular queue (or ring).
 * A ring is a data structure that store elements in a limited space; if new
 * elements are added when space is exhausted, the incomming elements overload
 * existing oldest elements.
 * Always the oldest element was retrieved first.
 * 
 * This implementation includes an specialization for pointers that free the
 * overloaded elements, if happens.
 */

template<typename _Tp>
class RingQueue
{
public:
  /**
   * Initialize the queue with the given capacity.
   */
  RingQueue(int capacity) : rq_capacity(capacity)
  { ring = new _Tp[capacity];
    rq_size = rq_in = rq_out = 0; }

  virtual ~RingQueue()
  { delete[] ring; }

  /**
   * Push an element to the end of the queue.
   * 
   * [2009-04-23] aya
   * Add a parameter that will be filled with true if the operation overloads
   * the queue.
   */
  void push(const _Tp& __x, bool* overloaded = 0);
  /**
   * Pop the oldest element.
   */
  _Tp get();

  int size() const { return rq_size; }
  int capacity() const { return rq_capacity; }

  void clear() { rq_size = rq_in = rq_out = 0; }

protected:
  /**
   * Called when an element will be overloaded.
   * By default do nothing. Would be redefined for pointer types.
   */
  virtual void onOverload() {}

private:
  _Tp* ring;
  int rq_capacity, rq_size;
  unsigned int rq_in, rq_out;

  posix::Mutex mutex;
};

template<typename _Tp>
void RingQueue<_Tp>::push(const _Tp& __x, bool* overloaded)
{
  posix::Mutex::MutexLock lock(mutex, ".RingQueue::push");

  if (overloaded != 0) *overloaded = false; // intialization
  if (rq_size == rq_capacity)
  {
  	if (overloaded != 0) *overloaded = true; // overloaded
    onOverload();
  }

  ring[rq_in] = __x;
  rq_in = (rq_in + 1) % rq_capacity;
  
  if (rq_size < rq_capacity)
    ++rq_size;
  else
    rq_out = rq_in;
}

template<typename _Tp>
_Tp RingQueue<_Tp>::get()
{
  assert(rq_size > 0);

  posix::Mutex::MutexLock lock(mutex, ".RingQueue::get");

  _Tp value = ring[rq_out];
  rq_out = (rq_out + 1) % rq_capacity;
  --rq_size;

  return value;
}


template<typename _Tp>
class RingQueue<_Tp*>
{
public:
  /**
   * Initialize the queue with the given capacity.
   */
  RingQueue(int capacity) : rq_capacity(capacity)
  { ring = new _Tp*[capacity];
    rq_size = rq_in = rq_out = 0; }

  virtual ~RingQueue()
  { clear();
    delete[] ring; }

  /**
   * Push an element to the end of the queue.
   */
  void push(_Tp* __x, bool* overloaded = 0);
  /**
   * Pop the oldest element.
   */
  _Tp* get();

  int size() const { return rq_size; }
  int capacity() const { return rq_capacity; }

  void clear();

protected:
  /**
   * Called when an element will be overloaded.
   * By default do nothing. Would be redefined for pointer types.
   */
  virtual void onOverload() { delete ring[rq_in]; }

private:
  _Tp** ring;
  int rq_capacity, rq_size;
  unsigned int rq_in, rq_out;

  posix::Mutex mutex;
};

template<typename _Tp>
void RingQueue<_Tp*>::push(_Tp* __x, bool* overloaded)
{
  posix::Mutex::MutexLock lock(mutex, ".RingQueue::push");

  if (overloaded != 0) *overloaded = false; // intialization
  if (rq_size == rq_capacity)
  {
    if (overloaded != 0) *overloaded = true; // overload detected
    onOverload();
  }

  ring[rq_in] = __x;
  rq_in = (rq_in + 1) % rq_capacity;
  
  if (rq_size < rq_capacity)
    ++rq_size;
  else
    rq_out = rq_in;
}

template<typename _Tp>
_Tp* RingQueue<_Tp*>::get()
{
  assert(rq_size > 0);

  posix::Mutex::MutexLock lock(mutex, ".RingQueue::get");

  _Tp* value = ring[rq_out];
  rq_out = (rq_out + 1) % rq_capacity;
  --rq_size;

  return value;
}

template<typename _Tp>
void RingQueue<_Tp*>::clear()
{
  while (rq_size > 0)
  {
    _Tp* __x = get();
    delete __x;
  }
  rq_size = rq_in = rq_out = 0;
}

#endif /*_RINGQUEUE_H_*/
