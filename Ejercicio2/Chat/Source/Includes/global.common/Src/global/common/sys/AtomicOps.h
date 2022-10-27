#ifndef _ATOMICOPS_H_
#define _ATOMICOPS_H_

/*
 * Jun 15, 2009 - ayachan
 */

#include "sync/Mutex.h"

#include <string>
#include <iostream>

namespace posix
{

  /**
   * Just an object that includes atomic operations for test and set.
   * Use for that variables that reflects some status, as "started", as in the
   * following example:
   *
   * The following code is NOT thread safe:
   *
   *   bool started = false;
   *   ...
   *   void start()
   *   {
   *     if (started) return;
   *     started = true;
   *     ...
   *   }
   *   void stop()
   *   {
   *     if (!started) return;
   *     started = false;
   *     ...
   *   }
   *
   * The following code IS thread safe (I hope):
   *
   *   bool started = false;
   *   ...
   *   void start()
   *   {
   *     if (AtomicOps::testAndSet(started, true)) return;
   *     // here if not started, but now TRUE
   *     ...
   *   }
   *   void stop()
   *   {
   *     if (AtomicOps::testAndSet(started, false)) return;
   *     // here if started, but now FALSE
   *     ...
   *   }
   *
   */
  class AtomicOps
  {
  public:
    /**
     * @return TRUE if variable's value is the same as argument, and
     *         FALSE if not, but was changed to that.
     */
    static bool testAndSet(bool &variable, bool aValue, const std::string& owner)
    {
      static Mutex mutex;

//      std::cout << "==== [testAndSet]: locking for " << owner << std::endl;
      Mutex::MutexLock lock(mutex, ".testAndSet");
      bool result = (variable == aValue);
      variable = aValue;
//      std::cout << "==== [testAndSet]: releasing " << owner << std::endl;
      return result;
    }

    static void increment(int &variable, int inc = 1)
    {
      static Mutex mutex;

      Mutex::MutexLock lock(mutex, ".increment");
      variable += inc;
    }

    static void decrement(int &variable, int dec = 1)
    {
      static Mutex mutex;

      Mutex::MutexLock lock(mutex, ".decrement");
      variable -= dec;
    }
  };

}

#endif /* _ATOMICOPS_H_ */
