#ifndef _THREADPOOL_H_
#define _THREADPOOL_H_

#include "Thread.h"
#include "../sync/Monitor.h"

#include <vector>
#include <deque>

namespace posix
{

  /**
   * Implements a thread pool; an utility where tasks can be left for execution.
   * The pool defines a number of threads that waits for task to execute. No more
   * than this number can be executed at the same time, pending tasks are store
   * on a queue and executed as thread availability.
   * The pool itself cares about allocation and release of the threads.
   *
   * [20160829] aya
   * Added 'removeJob'
   */ 
  class ThreadPool
  {
  public:
    ThreadPool(int numThreads);
    ~ThreadPool();

    /**
     * Add a new job to be executed by the threads.
     */
    void addJob(Runnable* job);
    bool removeJob(Runnable* job);

    /**
     * Retrieve all completed jobs out of the pool's internal list of completed
     * jobs. Completed jobs will be appended to the argument vector.
     * Return the number of jobs added to the queue.
     */
    int getCompletedJobs(std::vector<Runnable*> &jobs);

    /**
     * Remove any jobs that haven't been gotten to yet.
     * Return the number of jobs that were actually canceled.
     */
    int cancelPendingJobs();

    int pendingJobsSize();

  private:
    std::vector<Thread*> threads;
    std::deque<Runnable*> pendingJobs;
    std::vector<Runnable*> completedJobs;

    Monitor pendingJobsMonitor;

    Mutex completedJobsMutex;

    bool shutdown;

  private:
    /**
     * The thread to be run in the pool: as long there are pending jobs, fetch the
     * next and execute it.
     * If there are no jobs, must wait on the condition.
     */
    class PoolThread : public Thread
    {
    public:
      PoolThread(ThreadPool& pool) : pool(pool) {};
      void* run(void* args = 0);
    private:
      ThreadPool& pool;
    };

  public:
    friend class PoolThread;

  };

}

#endif /*_THREADPOOL_H_*/
