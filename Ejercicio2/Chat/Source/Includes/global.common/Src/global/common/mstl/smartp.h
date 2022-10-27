//#pragma once

#ifndef _SMARTPTR_H_
#define _SMARTPTR_H_

/*
 * Dec 22, 2010 - ayachan
 */

#include <iostream>

namespace mstl
{

  /**
   * Define a smart pointer with reference counting ownership strategy.
   * In brief, with this class you will be able to embrace any kind pointer with
   * this value-semantic object and use it (with some precautions) in standard
   * library containers.
   * The pointed object will move with assignments and creations and will be
   * deleted when the last smart pointer run out of context.
   *
   * Smart pointers should be managed as scalars (please do not use pointers to
   * them). The pointed object need to be special as far as the need for it; for
   * instance, if you are planning to use a (standard) priority queue with smart
   * pointers to some object, then that specific smart pointer need to have
   * defined the less operand onto the pointed object.
   * This behavior is expected to be explained with a lot of more details in our
   * intranet, but for all purposes, if you are planning to have no special use
   * with the smart pointer except expected creation, assignment and parameter
   * passing, then the pointed object needs nothing (except an appropriate
   * destructor).
   *
   * Rule of thumb:
   * Wherever a pointer must be returned and memory releasing could be
   * ambiguous, then a smart pointer (to the same original pointer) must be used
   * instead.
   *
   * Please note that this smart pointer does not copy its embedded content,
   * when you assign from or create a new smart pointer based on this pointer,
   * you will obtain a new smart pointer that points exactly to the same pointed
   * object, not a copy of it.
   * Remember to treat it always as scalar, never a pointer.
   *
   * Tipical use:
   *
   * If you have a method that return a pointer of self-allocated memory like
   * this:
   *
   *   Buffer* getData()
   *   {
   *     ...
   *     return new Buffer(...);
   *   }
   *   ...
   *   Buffer* myBuffer = getData();
   *   // ... use myBuffer content ...
   *   delete myBuffer;
   *
   * use instead:
   *
   *   mstl::smartp<Buffer> getData()
   *   {
   *     ...
   *     return mstl::smartp<Buffer>(new Buffer(...));
   *   }
   *   ...
   *   mstl::smartp<Buffer> myBuffer = getData();
   *   // ... use myBuffer content using -> and *...
   *   // ... forget about freeing, is automatically done when the variable run
   *   //     out of context
   *
   * Tested with valgrind (top memory leak detector) on Ubuntu with gcc 4.4 with
   * creation, assignment, parameters, std::vector (no special requirements) and
   * std::priority_queue (need a dedicated less binary predicate).
   */
  template<class T>
  class smartp
  {
  private:
    template<class U>
    struct smartp_object
    {
      U* pointee;
      int refcount;
      bool debug;

      smartp_object(U* pointee, bool debug = false)
        : pointee(pointee), refcount(1), debug(debug)
      {
        if (debug)
          std::cout << "[smartp_object]: create " << this << " for " << pointee << std::endl;
      }
      ~smartp_object()
      {
        if (debug)
          std::cout << "[smartp_object]: free " << this << " (with " << pointee << ")" << std::endl;
        delete pointee;
      }
    };

  public:
    smartp(bool debug = false)
      : object(0), debug(debug)
    {
      if (debug)
        std::cout << "[smartp]: create null " << this << std::endl;
    }

    smartp(const smartp& src)
      : object(0), debug(false)
    {
      if (debug)
        std::cout << "[smartp]: copy constructor " << this << " from " << &src << std::endl;
      object = src.object;
      object->refcount++;
      debug = src.debug;
    }

    smartp& operator=(const smartp& src)
    {
      if (debug)
        std::cout << "[smartp]: assignment " << this << " from " << &src << std::endl;
      if (src.object != object)
      {
        disposeObject();

        object = src.object;
        object->refcount++;
        debug = src.debug;
      }
      return *this;
    }

    /**
     * Traditional user constructor: pass the pointer to embrace.
     * @param pointee
     *          The pointer object to embrace.
     * @param debug
     *          When TRUE the smart pointer will dump to standard output all
     *          operations. For fun a lot and useful when using containers.
     */
    explicit smartp(T* pointee, bool debug = false)
      : object(0), debug(debug)
    {
      object = new smartp_object<T>(pointee, debug);
      if (debug)
        std::cout << "[smartp]: create " << this << " using " << object << std::endl;
    }
    ~smartp()
    {
      if (debug)
        std::cout << "[smartp]: free " << this << " using " << object << std::endl;
      disposeObject();
    }

    T& operator*() const
    {
      return *object->pointee;
    }

    T* operator->() const
    {
      return object->pointee;
    }

  public:
    T* getImpl() const { return object != 0 ? object->pointee : 0; }
    bool isDebug() const { return debug; }

  protected:
    smartp_object<T>* object;
    bool debug;

  private:
    void disposeObject()
    {
      if (object != 0)
      {
        --object->refcount;
        if (object->refcount == 0)
          delete object;
        object = 0;
      }
    }
  };

}

#endif /* _SMARTPTR_H_ */
