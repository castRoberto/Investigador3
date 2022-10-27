#ifndef _MONITOR_H_
#define _MONITOR_H_

#include "Mutex.h"
#include "CondVar.h"

namespace posix
{

  /**
   * Composition of a mutex and a condition variable.
   * Be smart, use the associated mutex to lock the condition, as in:
   * 
   *   Monitor m;
   *   ...
   *   m.lock();
   *   ...
   *   m.wait();
   *   ...
   *   m.unlock();
   * 
   * 2008.10.28 aya
   * Changed for advice to don't use a recursive mutex with condition variables
   */
  class Monitor : public Mutex, public CondVar
  {
  public:
//    Monitor(bool recursive = true) : Mutex(recursive), CondVar( (Mutex&)*this) {}
    Monitor() : Mutex(Mutex::MUTEX_ATTR_NORMAL), CondVar( (Mutex&)*this) {}
  };

}

#endif /*_MONITOR_H_*/
