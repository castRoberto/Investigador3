#ifndef _THREADMANAGER_H_
#define _THREADMANAGER_H_

#include "Thread.h"
#include "../sync/Mutex.h"
#include "../sync/Monitor.h"

#include <list>

namespace posix
{

  /**
   * Utility class intended to manage a set of threads to assure appropriate
   * resource release.
   * In brief each thread that wish to be managed must be added to this class and
   * stored on a list.
   * Threads that are known be terminated are alerted to this manager that join
   * them to definitely release its resources.
   *
   * This class has a thread that cares only on the above release activity.
   * At destruction time, this class try to release the resources off all
   * registered threads that are not already released.
   *
   * WARNING
   * Note that this class has a thread that will join to each thread that it was
   * supposed be terminated. If the thread is not in that state, the thread will
   * hang there forever.
   */
  class ThreadManager : public Thread::ThreadStatusListener, public Runnable
  {
  public:
    class ThreadManagerException {};

  public:
    ThreadManager();

    /**
     * At this time, all pending managed threads will be added to the to-join
     * list of threads to be released.
     */
    virtual ~ThreadManager();

  public:
    /**
     * Adds a thread to be manage by this class.
     * This pointers are stored on a simple list.
     * If activeMonitoring is set, this class will subscribe to the thread events
     * to auto-detect termination.
     */
    void manage(Thread* thread, bool activeMonitoring = false);

    /**
     * Call to release the given thread.
     * This pointer will be stored on another list to be join'ed on the inner
     * thread cycle.
     * After the release, the given thread will be removed from the managed
     * threads list if exists and the freed.
     */
    void release(Thread* thread);

    /**
     * The thread events handler.
     */
    void handleThreadstatus(Thread* thread, Thread::ThreadStatus status);

    /**
     * The method to be executed in the inner thread.
     */
    void* run(void* args = 0);

    /**
     * This method executes the finalization of the manager, raising the
     * 'terminated' condition at end. No other action except destroy would be
     * executed in this manager.
     */
    void terminate();

    bool isTerminated() const { return terminated; }

  private:
    /**
     * The list of managed threads.
     */
    std::list<Thread*> managedThreads;

    /**
     * The list of threads to be released.
     */
    std::list<Thread*> releaseList;

    /**
     * Mutex to control the access to the managed threads list.
     */
    Mutex managedThreadsLock;

    /**
     * Synchronization of the to-be-release list of threads.
     */
    Monitor releaseThreadsLock;

    /**
     * The thread that executes the method that release the threads.
     */
    Thread* releaser;

    /**
     * Local variable to warn the releaser thread that should terminate instead of
     * lock waiting threads to release.
     */
    bool shutdown;

    /**
     * Condition that was raised once the manager release all its resources,
     * including managed threads.
     * No action is permitted once this flag is raised.
     */
    bool terminated;
  };

  ThreadManager* getThreadManager();

}

#endif /*_THREADMANAGER_H_*/
