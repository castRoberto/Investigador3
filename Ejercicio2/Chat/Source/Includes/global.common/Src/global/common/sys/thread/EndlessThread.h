#ifndef _ENDLESSTHREAD_H_
#define _ENDLESSTHREAD_H_

#include "../thread/Thread.h"
#include "../sync/Mutex.h"

#include "../../util/omanips.h"

#include <string>
#include <iostream>

namespace posix
{

  /**
   * Defines an endless thread.
   * In this case the argument to the runnable run method will be the calling
   * thread, so the called method has a way to terminate the thread from the
   * inside:
   *
   *   void* run(void* args)
   *   {
   *     ...
   *     Thread* callingThread = static_cast<Thread*>(args);
   *     callingThread->terminate();
   *     ...
   *   }
   *
   * RECOMMENDATIONS
   *
   * 1. Declare the EndlessThread dynamically (as pointers) and give its
   *    administration to a ThreadManager. The Runnable argument would be static
   *    or dynamic.
   * 2. Terminate explicitly the EndlessThread unless was the end of the program
   *    (when the ThreadManager terminate them all).
   * 3. As expected, the Runnable method must be very, very careful in its
   *    running cycle time and their use of sync artifacts! The 'run' method is
   *    called inside a locked mutex.
   *
   */
  class EndlessThread : public Thread
  {
  private:
#ifdef __MINGW32__
    static const ptw32_handle_t NULL_THREAD;
#else
    static const unsigned long int NULL_THREAD = (unsigned long int) -1;
#endif

  public:
    static const long DEFAULT_CYCLE_DELAY_us = 1000;

    EndlessThread(Runnable* runnable, const std::string& name = ".untitled",
                  long delay_us = DEFAULT_CYCLE_DELAY_us)
      : runnable(runnable), name(name), delay_us(delay_us), thisThread(NULL_THREAD),
        mutex(Mutex::MUTEX_ATTR_RECURSIVE) {}

    void* run(void* args)
    {
      thisThread = Thread::getCurrentThread();

      void* result = 0;

      while (!terminated())
      {
        /*
         * Artificial cancellation point if the thread was cancelled.
         */
        Thread::testCancel();

        if (mutex.tryLock())
        {
          // (local cancellation point)
          if ( (!terminated()) &&
               ( ( (unsigned long)thisThread) == Thread::getCurrentThread()))
          {
            if (*(name.c_str()) != '.')
              std::cout << om_thread << om_time << "<<<< " << name << std::endl;
            result = runnable->run(this);
            if (*(name.c_str()) != '.')
              std::cout << om_thread << om_time << ">>>> " << name << std::endl;
          }
          mutex.unlock();
        }

        if (!terminated())
        {
          if (delay_us > 0)
            sleep(delay_us);
          else
            yield();
        }
      }
      return result;
    }

    void terminate()
    {
//      Mutex::MutexLock mlock(mutex, "EndlessThread::terminate");
      Mutex::MutexLock mlock(mutex, name);
      std::cout << "terminating endless thread" << std::endl;
      Thread::terminate();
#ifndef __MINGW32__
      thisThread = 0;
#endif
    }

  private:
    Runnable* runnable;
    std::string name;
    long delay_us;
    pthread_t thisThread;

    Mutex mutex;
  };

}

#endif /*_ENDLESSTHREAD_H_*/
