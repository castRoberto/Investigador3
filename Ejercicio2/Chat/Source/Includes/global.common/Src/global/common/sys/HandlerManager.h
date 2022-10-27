#ifndef _HANDLERMANAGER_H_
#define _HANDLERMANAGER_H_

/*
 * Jan 28, 2009 - ayachan
 */

#include "../global/Exception.h"

#include "../sys/sync/Mutex.h"

#include <pthread.h>
#include <vector>

namespace posix
{

  /**
   * This class provides a way to produce handlers, that is, an integer that can
   * be reserved and released and that is related to a given thread.
   * Handlers are used associated to a given entity that should be freed when
   * the handler is released.
   *
   * Instances of this class are used by resource managers.
   */
  class HandlerManager
  {
  public:
    static const int INVALID_HANDLER = -1;
    static const int MAX_HANDLERS = 80;

    /**
     * A general handling exception.
     */
    class HandlerException : public util::Exception
    {
    public:
      /** Construct a new pthread error object. */
      explicit HandlerException(const std::string &desc, int err = 0)
        : util::Exception(desc, err, "HandlerException") {}
    };


  public:
    /**
     * Create an instance able to manage upto the given number of handlers.
     */
    explicit HandlerManager(int maxHandlers = MAX_HANDLERS, bool careThreadOrigin = false);
    ~HandlerManager();

  public:
    /**
     * Create a handler and reserve it associated to the calling thread.
     * Normally this should always work.
     */
    int createHandler();

    /**
     * Release the given handler.
     * This should generate an exception if the handler doesn't exist or if the
     * releasing is not comming from the thread that owns the handler.
     */
    void releaseHandler(int handler);

    /**
     * Release all handlers.
     */
    void reset();

    /**
     * Return TRUE if the given handler is valid, that is, is allocated and
     * assigned to the given thread.
     */
    bool isValid(int handler);

    bool isAllocated(int handler);

    int remaining() const { return available; }
    int allocated() const { return handlers.size() - available; }

  protected:
    /**
     * Return the next available handler.
     */
    int getNextAvailableHandler();

  private:
    struct HandlerReserve
    {
      bool assigned;
      pthread_t owner;

      HandlerReserve() : assigned(false) {}
    };
    std::vector<HandlerReserve> handlers;
    posix::Mutex mutex;
    bool careThreadOrigin;
    int nextHandler;

    int available;
  };

} // namespace

#endif /* _HANDLERMANAGER_H_ */
