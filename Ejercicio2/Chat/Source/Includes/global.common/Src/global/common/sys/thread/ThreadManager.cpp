
#include "ThreadManager.h"
#include "EndlessThread.h"

#include "../PosixError.h"

#include <unistd.h>

#include <iostream>

#ifdef __MINGW32__
#include <mingw.h>
#endif

namespace posix
{

  ThreadManager* getThreadManager()
  {
    static ThreadManager _threadManager;

    return &_threadManager;
  }


  ThreadManager::ThreadManager()
    : managedThreadsLock(Mutex::MUTEX_ATTR_NORMAL), releaseThreadsLock(),
      shutdown(false), terminated(false)
  {
    std::cout << "[ThreadManager]: starting" << std::endl;

  //  shutdown = false;

    releaser = new EndlessThread(this);
    releaser->start();
  }

  /**
   * The moment of the truth: all (remaining) managed threads should be added to
   * the to-be-release list, raise the shutdown signal and signal the list.
   */
  ThreadManager::~ThreadManager()
  {
    if (!terminated)
    {
      std::cout << "[ThreadManager::~ThreadManager]: terminating..." << std::endl;
      terminate();
    }
  }

  void ThreadManager::terminate()
  {
    std::cout << "[ThreadManager]: ending" << std::endl;

    /*
     * This was deprecated.
     * The only way to release all running pending threads is terminate each one
     * of them. When terminated, each (managed) thread should report the
     * TERMINATED status by which they are added to the release list, when they
     * are joined and free this memory.
     *
    // lock the release list
    {
      Mutex::MutexLock rltlock(releaseThreadsLock);

      // copy the managed list to the release list
      // lock the managed list
      {
        Mutex::MutexLock mntlock(managedThreadsLock);

        // copy all threads from the managed to the release list
        releaseList.insert(releaseList.begin(),
                           managedThreads.begin(), managedThreads.end());
        // clear the managed list
        managedThreads.clear();
        // all done with the managed list

        // auto unlock
      }
      // auto unlock
    }
     *
     */

    std::cout << "[ThreadManager]: terminating all managed threads" << std::endl;
    {
      Mutex::MutexLock mntlock(managedThreadsLock, ".ThreadManager::terminate");

      // terminate any thread in the managed list
      std::list<Thread*>::iterator it = managedThreads.begin();
      while (it != managedThreads.end())
      {
        if (!(*it)->terminated())
          (*it)->terminate();
        ++it;
      }
      // auto unlock
    }
    std::cout << "[ThreadManager]: waiting for all managed threads to go" << std::endl;
    // must wait for al threads in the managed list to evacuate
    while (managedThreads.size() > 0)
      ::sleep(1);
    std::cout << "[ThreadManager]: all managed threads has gone" << std::endl;

    // raise the shutdown signal
    shutdown = true;

    /*
     * This call is necessary to trigger the release procedure.
     */
    // finally signal the release list (with the lock taken)
    releaseThreadsLock.signal();

    /*
     * (do not really found the sense to this code segment...)
     *
    if (managedThreadsLock.tryLock()) managedThreadsLock.unlock();
    else
      std::cerr << "[~ThreadManager]: managedThreadsLock was not unlocked!" <<
                   std::endl;

    if (releaseThreadsLock.tryLock()) releaseThreadsLock.unlock();
    else
      std::cerr << "[~ThreadManager]: releaseThreadsLock was not unlocked!" <<
                   std::endl;
     *
     */

    std::cout << "[~ThreadManager]: terminating releaser" << std::endl;
    releaser->terminate();

    std::cout << "[~ThreadManager]: waiting 3 seconds..." << std::endl;
    int remaining = sleep(3);
    while (remaining > 0)
      sleep(remaining);
    std::cout << "[~ThreadManager]: ... waiting end" << std::endl;

//    releaser->join();

//    // (and really) finally, locks in join the releaser thread
//    std::cout << "[~ThreadManager]: joining releaser" << std::endl;
//    try
//    {
//      releaser->join();
//    }
//    catch (PosixError& pe)
//    {
//      std::cout << "[ThreadManager::terminate]: " << pe << std::endl;
//    }
//    catch (...)
//    {
//      std::cout << "[ThreadManager::terminate]: unknown exception join releaser" << std::endl;
//    }
    delete releaser;
//    std::cout << "[~ThreadManager]: joined and deleted" << std::endl;

    terminated = true;
    std::cout << "[~ThreadManager]: finished" << std::endl;
  }

  /**
   * Adds the given thread class to the managed list.
   */
  void ThreadManager::manage(Thread* thread, bool activeMonitoring)
  {
    Mutex::MutexLock mlock(managedThreadsLock, ".ThreadManager::manage");

//    std::cout << "[ThreadManager]: manage " <<
//                 std::hex << thread << std::dec << std::endl;

    managedThreads.push_back(thread);
    if (activeMonitoring)
      thread->setThreadStatusListener(this);
  }

  /**
   * Adds the given thread to the to-be-release list and activates the lock
   * condition.
   * First remove the given thread from the managed list, if exist.
   */
  void ThreadManager::release(Thread* thread)
  {
    // (in blocks to release automatic variables)

//    std::cout << "[ThreadManager]: release " <<
//                 std::hex << thread << std::dec << std::endl;
    {
      Mutex::MutexLock mlock(managedThreadsLock, ".ThreadManager::release(1)");
      managedThreads.remove(thread);
    }

    {
      Mutex::MutexLock mlock(releaseThreadsLock, ".ThreadManager::release(2)");
      releaseList.push_back(thread);
    }
    releaseThreadsLock.signal();
  }

  void ThreadManager::handleThreadstatus(Thread* thread, Thread::ThreadStatus status)
  {
    switch (status)
    {
      case Thread::TERMINATED:
//        std::cout << "[ThreadManager::handleThreadstatus]: terminated " <<
//                     std::hex << thread << std::dec << std::endl;
        release(thread);
        break;

      // all following not interesting (for this purposes)
      case Thread::UNKNOWN:
      case Thread::READY:
      case Thread::RUNNING:
      default:
        break;
    }
  }

  /**
   * Get the threads of the to-be-release list and join them in turns.
   * If no thread are present locks on the list unless the shutdown flag is
   * raised.
   *
   * This method will be called inside an EndlessThread, and the argument will be
   * the calling thread itself
   */
  void* ThreadManager::run(void* args)
  {
    posix::Thread* callingThread = static_cast<posix::Thread*>(args);
    if (terminated || callingThread->terminated())
    {
      terminated = true;
      if (!callingThread->terminated())
        callingThread->terminate();

      return 0;
    }

    releaseThreadsLock.lock();
    if (releaseList.size() > 0)
    {
      // get and remove the first thread on the list and free the lock
      Thread* thread = releaseList.front();
      releaseList.pop_front();
      releaseThreadsLock.unlock();

      // and join the thread to release its resources
//      std::cout << "[ThreadManager]: (run) to join ..." <<
//                   std::hex << thread << std::dec << std::endl;
      thread->join();
//      std::cout << "[ThreadManager]: (run) ... joined " <<
//                   std::hex << thread << std::dec << std::endl;

      // after joined, the released thread can be freed
      delete thread;

//      std::cout << "[ThreadManager]: one more thread done " << std::endl;
    }
    else if (!shutdown)
    {
      // if no shutdown flag is raised must wait for threads to be released,
      // note that the lock must be taken, as is.
      // (this call also would be timed)
      releaseThreadsLock.wait();
      // (not very efficient but allow clear coding: the wait returns with the
      //  lock locked)
      releaseThreadsLock.unlock();
    }
    else
    {
      // (MUST UNLOCK!)
      releaseThreadsLock.unlock();

      // no more threads to release and shutdown signal: time to leave
      Thread* callingThread = static_cast<Thread*>(args);
      // (this will signal the outer cycle to terminate)
      callingThread->terminate();
    }

    return 0;
  }

}
