
#include "Thread.h"
#include "../PosixError.h"
#include "../../util/timespec.h"
#include "../../util/TimedCheck.h"

#include <time.h>
#include <errno.h>

namespace posix
{

  Thread::Thread(ThreadStatusListener* threadStatusListener,
                 int schedPolicy, int detachState)
    : terminatedFlag(false),
      status(UNKNOWN), threadStatusListener(threadStatusListener)
  {
	thread = 0;
    int result;
    if ( (result = pthread_attr_init(&attrs)) != 0)
      throw PosixError("Can't initialize thread attribute", result);
    if ( (result = pthread_attr_setdetachstate(&attrs, detachState)) != 0)
      throw PosixError("Can't initialize thread detach state attribute", result);
    if ( (result = pthread_attr_setschedpolicy(&attrs, schedPolicy)) != 0)
      throw PosixError("Can't initialize thread schedule policy attribute", result);

    status = READY;
  }

  /*
   * Sep 24, 2017.
   * Modified by AYA to report status when setting listener, assuming that this
   * call can arrive after the status was changed (for instance, the thread
   * could be terminated).
   */
  void Thread::setThreadStatusListener(ThreadStatusListener* threadStatusListener)
  {
    this->threadStatusListener = threadStatusListener;
    if (threadStatusListener != 0)
      threadStatusListener->handleThreadstatus(this, status);
  }

  void Thread::setStatus(ThreadStatus status)
  {
    if (this->status != status)
    {
      this->status = status;
      if (threadStatusListener != 0)
        threadStatusListener->handleThreadstatus(this, status);
    }
  }

  /**
   * Structure that may be used when starting the thread with parameters.
   */
  struct ThreadProcArgs
  {
    Thread* thread;
    void *args;

    ThreadProcArgs(Thread* thread, void* args = 0)
    { this->thread = thread; this->args = args; }
  };

  void* Thread::threadProc(void* params)
  {
    /*
     * If parameters to the thread are used, the received params should be a
     * structure like 'ThreadProcArgs'.
     *
    ThreadProcArgs* tpargs = (ThreadProcArgs*) params;
     */
    Thread* thread = static_cast<Thread*>(params);

    /*
     * Set a cleanup method just in case the thread is exit or cancelled.
     * That means that the state change command (to TERMINATE) will always be
     * performed.
     */
    void* result = 0;
    pthread_cleanup_push(Thread::threadTermination, thread);

    thread->setStatus(RUNNING);
    result = thread->run(0);

    /*
     * If arrived here means that the run method ends naturally, so we need to
     * change the (artificial) state to TERMINATED. This state change is key
     * when the thread is under the control of a ThreadManager.
     */
    pthread_cleanup_pop(0);
    thread->setStatus(TERMINATED);
    /*
     * (would be done also with 'pthread_cleanup_pop(1)')
     */

    return result;
  }

  void Thread::threadTermination(void* params)
  {
    Thread* thread = static_cast<Thread*>(params);
    thread->setStatus(TERMINATED);
  }

  void Thread::start()
  {
    /*
     * This (commented) version could be used to give parameters to the thread
     * execution.
     *
    int result;
    if ( (result = pthread_create(&thread, &attrs, Thread::threadProc,
                                  new ThreadProcArgs(this))) != 0)
      throw PosixError("Can't create thread", result);
     */

//    int result;
//    if ( (result = pthread_create(&thread, &attrs, Thread::threadProc, this)) != 0)
//      throw PosixError("Can't create thread", result);

    int result = pthread_create(&thread, &attrs, Thread::threadProc, this);
    int count = 5;
    while ( (count-- > 0) && (result == EINTR))
    {
      util::TimedCheck::wait(100);
      result = pthread_create(&thread, &attrs, Thread::threadProc, this);
    }
    if (result != 0)
    {
      throw PosixError("Can't create thread", result);
    }
  }

  void* Thread::join()
  {
    // check if current thread, to avoid join deadlock
    if (thread == pthread_self())
    {
      std::cout << "joining thread from itself!" << std::endl;
      return 0;
    }

    int result;
    void* threadResult = 0;
    if ( (result = pthread_join(thread, &threadResult)) != 0)
      throw PosixError("Fail to join thread ", result);
    return threadResult;
  }

  int Thread::sleep(long interval_us)
  {
#ifdef __MINGW32__
    Sleep(interval_us * 1000);
#else
    struct timespec ts = us_to_timespec(interval_us);
    return nanosleep(&ts, 0);
#endif
  }

  void Thread::yield()
  {
    int result;
    if ( ( result = sched_yield()) != 0)
      throw PosixError("Fail to yield", errno);
  }

  void Thread::cancel()
  {
    int result;
    if ( ( result = pthread_cancel(thread)) != 0)
      throw PosixError("Fail to cancel thread", result);
  }

  void Thread::setCancelState(int cancelState)
  {
    int result;
    if ( ( result = pthread_setcancelstate(cancelState, 0)) != 0)
      throw PosixError("Fail to set cancel state", errno);
  }

  void Thread::setCancelType(int cancelType)
  {
    int result;
    if ( ( result = pthread_setcanceltype(cancelType, 0)) != 0)
      throw PosixError("Fail to set cancel type", errno);
  }

}
