//#pragma once

#ifndef _EXECUTOR_H_
#define _EXECUTOR_H_

/*
 * Jul 31, 2009 - ayachan
 */

#include "../global/Runnable.h"
#include "thread/ThreadPool.h"

namespace posix
{

  /**
   * Facility to execute single tasks.
   */
  class Executor
  {
  public:
    static void execute(Runnable* task);
    static bool detach(Runnable* task);

  private:
    static ThreadPool* getThreadPool();
  };

} // namespace

#endif /* _EXECUTOR_H_ */
