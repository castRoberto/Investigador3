/*
 * Jul 31, 2009 - ayachan
 */

#include "Executor.h"

namespace posix
{

  void Executor::execute(Runnable* task)
  {
    getThreadPool()->addJob(task);
  }
  bool Executor::detach(Runnable* task)
  {
    return getThreadPool()->removeJob(task);
  }

  ThreadPool* Executor::getThreadPool()
  {
    static ThreadPool threadPool(10);

    return &threadPool;
  }

}
