#ifndef _POSIX_THREAD_H_
#define _POSIX_THREAD_H_

#include "../../global/Runnable.h"

#include <assert.h>
#include <pthread.h>

namespace posix
{

  /**
   * Encapsulates a pthread.
   * In general just work as an adaptor to the pthread's calling interface.
   *
   * The thread class inherit the 'run' method of the Runnable interface. That
   * method is called just once from this adaption.
   *
   * Add a little management facility with the "terminated" flag.
   *
   * This thread executes the inner 'run' method JUST ONE TIME. It's the inherance
   * responsability to make the necessary to operate in a endless cycle if apply,
   * BUT remember to check for termination and cancellation! in each cycle as a
   * minimum.
   * In this case, the following figure should help:
   *
   * void run()
   * {
   *   while (!terminated())
   *   {
   *     ... my loop ...
   *   }
   * }
   *
   * This figure is already implemented on EndlessThread, where you need to
   * provide the runnable to execute inside the cycle.
   */
  class Thread : public Runnable
  {
  public:
    enum ThreadStatus { UNKNOWN, READY, RUNNING, TERMINATED };
    /**
     * Interface to those objects that wants to know about the thread status.
     */
    class ThreadStatusListener
    {
    public:
      virtual ~ThreadStatusListener() {}
      virtual void handleThreadstatus(Thread* thread, ThreadStatus status) = 0;
    };

  public:
    /**
     * Create the envelope for the thread to be run with the given attributes.
     * The thread is not started.
     */
    Thread(ThreadStatusListener* threadStatusListener = 0,
           int schedPolicy = SCHED_RR,
           int detachState = PTHREAD_CREATE_JOINABLE);

    /**
     * PThread's are automatically released when some (other) thread join it.
     */
    virtual ~Thread() { /* MUST DO NOTHING! */ }

    /**
     * @see Runnable
     *
    virtual void* run(void* args) = 0;
     */

    /**
     * Returns an id of this thread (happens to be the pthread_t address).
     *
    unsigned int getId() { return (int)&thread; }
     */

    /**
     * Returns the current's thread id (also happens to be the corresponding
     * pthread_t address).
     * This method has compatibility issues.
     */
    static unsigned long getCurrentThread()
    {
#ifdef __MINGW32__
      pthread_t t = pthread_self();
      return (unsigned long)t.p;
#else
      return (unsigned long)pthread_self();
#endif
    }

    /**
     * Starts the thread.
     * Actually it does not admit parameters, but it would be added.
     */
    void start();

    /**
     * Locks on thread termination.
     */
    void* join();

    /**
     * Cancels this thread.
     */
    void cancel();

    void setThreadStatusListener(ThreadStatusListener* threadStatusListener);

  public:

    /**
     * Set the cancellation state for the calling thread.
     * Values would be PTHREAD_CANCEL_ENABLE (normally by default) or
     * PTHREAD_CANCEL_DISABLE (cancel requests are ignored).
     *
     * @see pthread_setcancelstate
     */
    static void setCancelState(int cancelState);

    /**
     * Set the type of responses to cancellation requests for the calling
     * thread.
     * Values would be PTHREAD_CANCEL_ASYNCHRONOUS (honour the request in the
     * moment) or PTHREAD_CANCEL_DEFERRED (normally by default).
     * If deferred, the thread terminates when reaching a cancellation point,
     * like join, conditional waits AND test cancel.
     *
     * @see pthread_setcanceltype
     */
    static void setCancelType(int cancelType);

    /**
     * In brief an artificial cancelation point. The thread terminates if was
     * already cancelled in deferred mode.
     *
     * @see pthread_testcancel
     */
    static void testCancel()
    {
    	pthread_testcancel();
    }

    /**
     * Sleeps the current thread, if it's running, for the given interval in [us].
     * It returns 0 if all ok or an error code if anything else.
     */
    static int sleep(long interval_us);

    /**
     * Makes this thread to release the CPU in its current turn.
     *
     * @see sched_yield
     */
    static void yield();

  private:
    // can't be copied or assigned (has no clear sense)
    Thread(const Thread& thread)
      : thread(), attrs(), terminatedFlag(true), status(UNKNOWN),
        threadStatusListener(NULL)
      { assert(false); }
    /* has problems with pure methods
    Thread operator= (const Thread& thread) { assert(false); }
     */

  private:
    pthread_t thread;
    pthread_attr_t attrs;
    bool terminatedFlag;

    ThreadStatus status;
    ThreadStatusListener* threadStatusListener;

//  public:
//    void setThreadStatusListener(ThreadStatusListener* threadStatusListener)
//    { this->threadStatusListener = threadStatusListener; }

  private:
    void setStatus(ThreadStatus status);

  public:
    /**
     * Raise the flag signal to terminate the thread.
     */
    virtual void terminate() { terminatedFlag = true; }
    /**
     * Check if the flag signal is raised.
     * This signal MUST BE CHECKED by the inner task (in the run method) and exit
     * if apply.
     */
    bool terminated() const { return terminatedFlag; }

  public:
    /**
     * This status is set when the thread really terminate, so it is a state
     * that users can wait to be sure the thread will no execute their cycle
     * again.
     */
    bool isTerminated() const { return (status == TERMINATED); }

  private:
    /**
     * This (static) method will be called by the pthread to efectively run the
     * associated method.
     */
    static void *threadProc(void* params);

    /**
     * This (static) method is set to be called when the thread was cancelled or
     * exit. It just set (and eventually propagate) the TERMINATED status.
     */
    static void threadTermination(void* params);
  };

}

#endif /*_THREAD_H_*/
