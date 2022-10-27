
#include "ThreadPool.h"

namespace posix
{

  ThreadPool::ThreadPool(int numThreads)
    : threads(), pendingJobs(), completedJobs(),
      pendingJobsMonitor(), completedJobsMutex(),
      shutdown(Mutex::MUTEX_ATTR_NORMAL)
  {
    // create, initialize and start threads in the pool
    // (as far we know, all threads must end waiting on the condition, because
    // there is no jobs yet)
  
    for (int n=0; n<numThreads; n++)
    {
      Thread* thrd = new PoolThread(*this);
      thrd->start();
      threads.push_back(thrd);
    }
  }
  
  ThreadPool::~ThreadPool()
  {
    // first raise the shutdown signal and release the pending threads
    
    // lock the pending jobs queue
    pendingJobsMonitor.lock();
    // raise the global signal of shutdown
    shutdown = true;
    // broadcast news
    pendingJobsMonitor.broadcast();
    pendingJobsMonitor.unlock();
  
    // then join on each thread to ensure will release is resources
    for (unsigned int n=0; n<threads.size(); n++)
    {
      threads[n]->join();
      delete threads[n];
    }
  }
  
  int ThreadPool::pendingJobsSize()
  {
    // get the pending jobs mutex
    pendingJobsMonitor.lock();
    // get the number of elements in the queue
    int result = pendingJobs.size();
    // release the lock
    pendingJobsMonitor.unlock();
  
    return result;
  }
  
  void ThreadPool::addJob(Runnable* job)
  {
    // get the pending jobs mutex
    pendingJobsMonitor.lock();
    // add the job to the (tail of the) list
    pendingJobs.push_back(job);
    // signal the condition (with lock taken, that release the mutex)
    pendingJobsMonitor.signal();
  //  pendingJobsMonitor.broadcast();
    pendingJobsMonitor.unlock();
  }

  bool ThreadPool::removeJob(Runnable* job)
  {
    bool result = false;

    // get the pending jobs mutex
    pendingJobsMonitor.lock();

    // iterate looking for the job in the list
    std::deque<Runnable*>::iterator loc = pendingJobs.end();
    for (std::deque<Runnable*>::iterator it = pendingJobs.begin();
         it != pendingJobs.end(); ++it)
    {
      Runnable* qjob = *it;
      if (qjob == job)
      {
        loc = it;
        break;
      }
    }
    if (loc != pendingJobs.end())
    {
      pendingJobs.erase(loc);
      result = true;
    }

    pendingJobsMonitor.unlock();
    return result;
  }

  int ThreadPool::getCompletedJobs(std::vector<Runnable*> &jobs)
  {
    // get the completed jobs mutex
    completedJobsMutex.lock();
    // remember the size
    int result = completedJobs.size();
    // iterate over all items pushing them to the incomming vector
    for (unsigned int n=0; n<completedJobs.size(); n++)
      jobs.push_back(completedJobs[n]);
    // clear the completed jobs vector
    completedJobs.clear();
    // release the lock and return
    completedJobsMutex.unlock();
  
    return result;
  }
  
  int ThreadPool::cancelPendingJobs()
  {
    // get the pending jobs mutex
    pendingJobsMonitor.lock();
    // remember the size
    int result = pendingJobs.size();
    // release all jobs in the list
    for (unsigned int n=0; n<pendingJobs.size(); n++)
      delete pendingJobs[n];
    // clear the list
    pendingJobs.clear();
    // release the mutex
    pendingJobsMonitor.unlock();
  
    return result;
  }
  
  void* ThreadPool::PoolThread::run(void* args)
  {
    while (true)
    {
      // get the mutex for the pending jobs queue
      pool.pendingJobsMonitor.lock();
      while ( (!pool.shutdown) && (pool.pendingJobs.size() == 0))
      {
        // if no pending jobs, wait on condition, that need the lock but was
        // released soon at wait
        //
        // upgrade: instead of just wait, now waits with a timeout to allow
        // wake up of job-enabled threads
        //
        pool.pendingJobsMonitor.timedWait(100, "ThreadPool");
      }
  
      // at this point, the lock was re-acquired: either there is some item at the
      // queue (of pending jobs) or the shutdown signal was raised
      if (pool.shutdown)
      {
        pool.pendingJobsMonitor.unlock();
        return 0;
      }
  
      if (pool.pendingJobs.size() > 0)
      {
        // fetch the next job and process it
        Runnable* job = pool.pendingJobs.front();
        pool.pendingJobs.pop_front();
        // (free the lock on the pending jobs queue)
        pool.pendingJobsMonitor.unlock();
  
        // run the job
        job->run();
  
        // add the completed job to the corresponding list
        pool.completedJobsMutex.lock();
        pool.completedJobs.push_back(job);
        pool.completedJobsMutex.unlock();
      }
      else
      {
        // just free the lock and try again
        pool.pendingJobsMonitor.unlock();
      }
    }
  
    return 0;
  }

}
