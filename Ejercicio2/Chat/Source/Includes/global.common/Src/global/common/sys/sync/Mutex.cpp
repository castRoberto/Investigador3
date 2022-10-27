
#include "Mutex.h"
#include "../PosixError.h"

#include "../../util/timespec.h"
#include "../../util/omanips.h"
#include "../../util/TimedCheck.h"

#include <errno.h>

#include <string.h>
//#if defined(__QNXNTO__)
//#endif

#include <iostream>

#if defined(__QNXNTO__)
#define MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE
#else
#define MUTEX_RECURSIVE PTHREAD_MUTEX_RECURSIVE_NP
#endif

namespace posix
{
  Mutex::Mutex(MutexAttr mutexAttr)
  {
    memset(&mutex, 0, sizeof(mutex));
//    std::cout << "[Mutex]: creating (" << mutex.count << "," << mutex.owner << ")" << std::endl;

    int result;
    pthread_mutexattr_t attr;

    memset(&attr, 0, sizeof(attr));
    if ( (result = pthread_mutexattr_init(&attr)) != 0)
      throw PosixError("Can't initialize mutex attribute", result);

    if ( (result = pthread_mutexattr_settype(&attr, mutexAttr)) != 0)
      throw PosixError("Can't set mutex attribute", result);

    result = pthread_mutex_init(&mutex, &attr);
    int count = 5;
    while ( (count-- > 0) && (result == EINTR))
    {
      util::TimedCheck::wait(100);

      // first fault correction: need to reset the structure again
      memset(&attr, 0, sizeof(attr));
      // second fault correction: also need to re-initialize the dumm-structure
      if ( (result = pthread_mutexattr_init(&attr)) != 0)
        throw PosixError("Can't initialize mutex attribute", result);
      if ( (result = pthread_mutexattr_settype(&attr, mutexAttr)) != 0)
        throw PosixError("Can't set mutex attribute", result);

      result = pthread_mutex_init(&mutex, &attr);
    }

    if (result != 0)
    {
      throw PosixError("Can't create mutex", result);
    }
  }

//  Mutex::Mutex(bool recursive)
//  {
//    /*
//     * It happens that the pthread_mutex_init call fails with "already
//     * initialized mutex". Very strange.
//     * Empirically it shows that the "pthread_mutex_t" structure is almost
//     * always in zero at start, but sometimes not. When is not zero, it happens
//     * the above error.
//     *
//     * So, the structure is initialized in zero to avoid that condition.
//     * Strictly it would be necessary because is a variable...
//     */
//    memset(&mutex, 0, sizeof(mutex));
////    std::cout << "[Mutex]: creating (" << mutex.count << "," << mutex.owner << ")" << std::endl;
//
//    int result;
//    pthread_mutexattr_t attr;
//
//    memset(&attr, 0, sizeof(attr));
//    if ( (result = pthread_mutexattr_init(&attr)) != 0)
//      throw PosixError("Can't initialize mutex attribute", result);
//    if (recursive)
//    {
//      if ( (result = pthread_mutexattr_settype(&attr, MUTEX_RECURSIVE)) != 0)
//        throw PosixError("Can't set mutex attribute", result);
//      else
//      {
//        // set ((int)(_NTO_SYNC_NONRECURSIVE|_NTO_SYNC_NOERRORCHECK))
//        if ( (result = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL)) != 0)
//          throw PosixError("Can't set mutex attribute", result);
//      }
//    }
//
//    result = pthread_mutex_init(&mutex, &attr);
//    int count = 5;
//    while ( (count-- > 0) && (result == EINTR))
//    {
//      util::TimedCheck::wait(100);
//
//      // first fault correction: need to reset the structure again
//      memset(&attr, 0, sizeof(attr));
//      // second fault correction: also need to re-initialize the dumm-structure
//      if ( (result = pthread_mutexattr_init(&attr)) != 0)
//        throw PosixError("Can't initialize mutex attribute", result);
//      if (recursive)
//      {
//        if ( (result = pthread_mutexattr_settype(&attr, MUTEX_RECURSIVE)) != 0)
//          throw PosixError("Can't set mutex attribute", result);
//      }
//      else
//      {
//        // set ((int)(_NTO_SYNC_NONRECURSIVE|_NTO_SYNC_NOERRORCHECK))
//        if ( (result = pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_NORMAL)) != 0)
//          throw PosixError("Can't set mutex attribute", result);
//      }
//
//      result = pthread_mutex_init(&mutex, &attr);
//    }
//    if (result != 0)
//    {
//      throw PosixError("Can't create mutex", result);
//    }
//  }

  Mutex::~Mutex()
  {
    int result;
    if ( (result = pthread_mutex_destroy(&mutex)) != 0)
      std::cerr << "Can't destroy mutex. Error " << result << ": " <<
                   strerror(result) << std::endl;
  }

  void Mutex::lock()
  {
  //  cout << "lock " << this << endl;
    int result;
    if ( (result = pthread_mutex_lock(&mutex)) != 0)
      throw PosixError("Can't lock mutex", result);
  }

  bool Mutex::tryLock()
  {
    int result = pthread_mutex_trylock(&mutex);
    if (result == 0)
      return true;
    else if (result == EBUSY)
      return false;
    else
      throw PosixError("Can't tryLock mutex", result);
  }

  bool Mutex::timedLock(long elapse_ms)
  {
    bool f_result = false;

    timespec start_ts = abstime();
    timespec ts = start_ts + (elapse_ms * 1000); // timespec's '+' adds [usec]

    bool keepTrying = true;
    while (keepTrying)
    {
      int result = pthread_mutex_timedlock(&mutex, &ts);
      switch (result)
      {
        case 0:
        {
          // no error
          f_result = true;
          keepTrying = false;
        }
          break;
        case ETIMEDOUT:
        {
          // time out
          f_result = false;
          keepTrying = false;
        }
          break;
        case EAGAIN:
          // keep trying
          break;
        case EDEADLK:
        {
          // the current thread already owns the mutex
          // keep trying
        }
          break;
        default:
          throw PosixError("Can't timedLock mutex", result);
      }
    }
    return f_result;
  }

  void Mutex::unlock()
  {
  //  cout << "unlock " << this << endl;
    int result;
    if ( (result = pthread_mutex_unlock(&mutex)) != 0)
      throw PosixError("Can't lock mutex", result);
  }

  long Mutex::MutexLock::DEFAULT_ELAPSE_ms = 3000;

  Mutex::MutexLock::MutexLock(Mutex& mutex, const std::string& owner)
    : mutex(mutex), elapse_ms(DEFAULT_ELAPSE_ms), owner(owner)
  {
    perform();
  }
  Mutex::MutexLock::MutexLock(Mutex& mutex, long elapse_ms, const std::string& owner)
    : mutex(mutex), elapse_ms(elapse_ms), owner(owner)
  {
    perform();
  }

  void Mutex::MutexLock::perform()
  {
    if (!mutex.timedLock(elapse_ms))
    {
      std::cout << om_thread << om_timef << "**** ALERT!: mutex can't be lock for " << owner << std::endl;
      throw 0;
    }
    if (*(owner.c_str()) != '.')
      std::cout << om_thread << om_timef << "[[[[ " << owner << std::endl;
  }

  Mutex::MutexLock::~MutexLock()
  {
    try
    {
      mutex.unlock();
      if (*(owner.c_str()) != '.')
        std::cout << om_thread << om_timef << "]]]] " << owner << std::endl;
    }
    catch (PosixError pthe)
    {
      std::cerr << pthe << std::endl;
    }
  }

}
