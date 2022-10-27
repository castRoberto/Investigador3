#ifndef _MUTEX_H_
#define _MUTEX_H_

#include <assert.h>
#include <pthread.h>

#include <string>

namespace posix
{

  /**
   * Define a POSIX mutex.
   * All operations that involves an error generate an exception of type
   * PThreadError except the destructor.
   * To resist exceptions while using a mutex, the lock MUST BE DONE throw the use
   * of a MutexLock declaration in this way:
   *
   *   // mutex initialized elsewhere
   *   Mutex m;
   *
   *   {
   *     // adquire the lock or exception
   *     Mutex::MutexLock mlock(m);
   *     ...
   *     if something happens here (as an exception), the mlock variable will be
   *     automatically freed, so the mutex will be released also
   *     ...
   *     don't need to explicitly release de mutex
   *   }
   *
   */
  class Mutex
  {
    friend class CondVar;

  public:
    enum MutexAttr
    {
      MUTEX_ATTR_DEFAULT = 0,
      MUTEX_ATTR_RECURSIVE = PTHREAD_MUTEX_RECURSIVE,
      MUTEX_ATTR_NORMAL = PTHREAD_MUTEX_NORMAL // _NTO_SYNC_NONRECURSIVE|_NTO_SYNC_NOERRORCHECK
    };

  public:
    /*
     * [20160904] aya
     * Added a constructor with desired elapse time.
     * Look like this C++ version does not have "delegating constructor", so it
     * needs an additional method to share common code.
     */
    class MutexLock
    {
      static long DEFAULT_ELAPSE_ms;
    public:
      MutexLock(Mutex& mutex, const std::string& owner = ".untitled");
      MutexLock(Mutex& mutex, long elapse_ms, const std::string& owner = ".untitled");
      ~MutexLock();
    private:
      void perform();
    private:
      Mutex& mutex;
      long elapse_ms;
      std::string owner;
    };

  public:
//    // @deprecated, use MutexAttr
//    Mutex(bool recursive = true);

    Mutex(MutexAttr mutexAttr = MUTEX_ATTR_DEFAULT);
    virtual ~Mutex();

    /*
     * This methods are provided for compliance with the abstraction, but users
     * usually should lock mutexes in a safety way using MutexLock.
     */

    void lock();
    bool tryLock();
    bool timedLock(long elapse_ms);
    void unlock();

  private:
    // can't be copied or assigned
    Mutex(const Mutex& mutex) { assert(false); }
    Mutex operator= (const Mutex& mutex) { assert(false); return mutex; }

  private:
    pthread_mutex_t mutex;
  };

}

#endif /*_MUTEX_H_*/
