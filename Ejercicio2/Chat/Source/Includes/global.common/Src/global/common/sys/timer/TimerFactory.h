#ifndef _TIMERFACTORY_H_
#define _TIMERFACTORY_H_

/*
 * Jan 27, 2009 - ayachan
 */

#include "../../global/Runnable.h"
#include "../../global/Exception.h"

namespace posix
{

  /**
   * Interface to the timer generator.
   */
  class TimerFactory
  {
  public:
    /**
     * Any timer exception.
     */
    class TimerException : public util::Exception
    {
    public:
      /** Construct a new pthread error object. */
      explicit TimerException(const std::string &desc, int err = 0)
        : util::Exception(desc, err, "TimerException") {}
    };


    typedef int TIMER_HANDLER;
    static const TIMER_HANDLER INVALID_HANDLER_TIMER = -1;

  public:
    virtual ~TimerFactory() {}

    /**
     * Schedule the given task to be executed after the given delay_ms [ms],
     * optionally repeating each period_ms [ms].
     * It returns a handler that should be used by the caller to post-operate
     * onto the timer programming.
     * The handler will become invalid when the timer was cancelled or
     * executed withouth repetitions.
     */
    virtual TIMER_HANDLER schedule(Runnable* task, long delay_ms, long period_ms = 0) = 0;

    /**
     * Cancel the given timer entry.
     * Returns TRUE if the cancel succeed (the item is removed from the queue
     * and so) or FALSE otherwise.
     */
    virtual bool cancel(TIMER_HANDLER handler) = 0;

  };

} // namespace

#endif /* _TIMERFACTORY_H_ */
