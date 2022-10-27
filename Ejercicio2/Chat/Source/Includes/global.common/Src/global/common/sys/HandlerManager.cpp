/*
 * Jan 28, 2009 - ayachan
 */

#include "HandlerManager.h"

#include <iostream>

namespace posix
{

  HandlerManager::HandlerManager(int maxHandlers, bool careThreadOrigin)
    : handlers(maxHandlers), mutex(Mutex::MUTEX_ATTR_DEFAULT), careThreadOrigin(careThreadOrigin),
      nextHandler(0), available(maxHandlers)
  {}

  HandlerManager::~HandlerManager()
  {
    // nothing to do here
  }

  int HandlerManager::getNextAvailableHandler()
  {
    bool found = false;
    int startHandler = nextHandler;
    do
    {
      if (!handlers[nextHandler].assigned)
        found = true;
      else
        nextHandler = (nextHandler + 1) % handlers.size();
    }
    while ( (!found) && (nextHandler != startHandler));

    if (!found)
      throw HandlerException("handler not found");

    int foundHandler = nextHandler;
    nextHandler = (nextHandler + 1) % handlers.size();
    return foundHandler;
  }

  int HandlerManager::createHandler()
  {
    posix::Mutex::MutexLock lock(mutex, ".HandlerManager::createHandler");

    // locate an available entry
    int handler = getNextAvailableHandler();

    handlers[handler].assigned = true;
    handlers[handler].owner = pthread_self();
    --available;

//    std::cout << "[HandlerManager]: handler " << handler << " allocated." <<
//                 " Remain " << available << std::endl;

    return handler;
  }

  void HandlerManager::releaseHandler(int handler)
  {
    posix::Mutex::MutexLock lock(mutex, ".HandlerManager::releaseHandler");

    /*
     * Looks that this is a lock-recursive call... assume it was previously
     * validated
     *
    if (!isValid(handler))
      throw HandlerException("invalid handler");
     */

    handlers[handler].assigned = false;
    ++available;

//    std::cout << "[HandlerManager]: handler " << handler << " released."  <<
//                 " Remain " << available << std::endl;
  }

  void HandlerManager::reset()
  {
    handlers.clear();
    nextHandler = 0;
    available = handlers.capacity();
  }

  bool HandlerManager::isValid(int handler)
  {
    posix::Mutex::MutexLock lock(mutex, ".HandlerManager::isValid");

    bool originFail =
      careThreadOrigin && (handlers[handler].owner != pthread_self());

//    if (originFail)
//      std::cout << "[HandlerManager::isValid]: " << handler <<
//                   " arrived from a different thread!" << std::endl;

    return (handler >= 0) && (handler < (int)handlers.size()) &&
           (handlers[handler].assigned) && !originFail;
  }

  bool HandlerManager::isAllocated(int handler)
  {
    posix::Mutex::MutexLock lock(mutex, ".HandlerManager::isAllocated");

    if ( (handler < 0) || (handler >= (int)handlers.size()))
      throw HandlerException("invalid handler");

    return handlers[handler].assigned;
  }

} // namespace
