#ifndef _CONDVAR_H_
#define _CONDVAR_H_

#include "Mutex.h"
#include <pthread.h>

namespace posix
{

  /*
   * Define a POSIX condition variable, a synchronization point for threads.
   * One or more threads can lock on one of this objects and be released
   * individually or all at once.
   * <p>Conditions ALWAYS are used with a mutex to control de access to the
   * conditioned variable. That mutex is given in this class construction.</p>
   * 
   * <p>Note: don't use this class/concept unless you know how to use it,
   * monitors are an already abstraction of mutex and condvar's.</p>
   * 
   * <p><b>How conditions works</b></p>
   * <p>Condition's should always be associated with a mutex. The condition
   * assumes that the mutex is locked <b>before</b> the wait (family of)
   * instruction is called. The instruction automatically unlock the mutex at
   * start and relock it when returns.</p>
   */
  class CondVar
  {
  public:
    CondVar(Mutex& mutex);
    virtual ~CondVar();
  
    /**
     * Threads call this method to lock on the condition.
     */
    void wait();
    /**
     * One waiting thread, if exist, will be unlocked with this call.
     */
    void signal();
    /**
     * All locked threads waiting on this condition will be released with this
     * call.
     */
    void broadcast();
  
    /**
     * Wait with timeout.
     * Returns true if the condition was raised or false if the timeout was
     * reached.
     */
    bool timedWait(long ms, const std::string& from = "");
  
  private:
  
  private:
    pthread_cond_t condition;
    Mutex& mutex;
  };

} // namespace

#endif /*_CONDVAR_H_*/
