//#pragma once

#ifndef _DECOUPLER_H_
#define _DECOUPLER_H_

/*
 * Jan 29, 2010 - ayachan
 */

#include "../global/Runnable.h"
#include "../sys/sync/Monitor.h"
#include "../sys/thread/Thread.h"
//#include "../sys/thread/ThreadManager.h>

#include "../util/omanips.h"

#include <unistd.h>

#include <list>

#include <iostream>

namespace util
{

  enum DecouplerEventType
  {
    /**
     * It can exist only one of this event in one moment in the queue.
     * Events are compared with '==' operand.
     * Remains the first event posted.
     */
    detEXCLUSIVE
    /**
     * This event can appear multiple times on the queue.
     */
    , detMULTIPLE

    /**
     * The queue can have just one element, always the last inserted.
     */
    , detSINGLE_LAST
  };

  /**
   * A generic decoupler.
   * Objects are placed in a queue and later retrieved to be processed in
   * another thread.
   * The decoupled process is performed using a callback.
   */
  template <class E>
  class Decoupler
  {
//    friend class posix::ThreadManager;

  public:
    struct DecouplerListener
    {
      virtual ~DecouplerListener() {}
      virtual void handle(Decoupler* decoupler, E data) = 0;
    };

  private:
    struct DecouplerThread : public posix::Thread
    {
      Decoupler* decoupler;
      DecouplerThread(Decoupler* decoupler) : decoupler(decoupler) {}
      /**
       * @inherited from posix::Thread / Runnable
       */
      /**
       * The cycle where the decoupler detects incoming data and call back the
       * registered listener.
       */
      void* run(void* args = 0)
      {
        while (!terminated())
        {
          bool result = false;
// VSATEST         E object = decoupler->defaultValue;
        E object;
          {
            // (the MutexLock auto var will release the monitor at the end)
            posix::Mutex::MutexLock lock(decoupler->monitor, ".Decoupler::run");
            if (decoupler->queue.size() == 0)
              result = decoupler->monitor.timedWait(1);
            else
              result = true;
            if (result && (decoupler->queue.size() > 0))
            {
              object = decoupler->queue.front();
              decoupler->queue.pop_front();
              if ( (decoupler->listener != 0) && !terminated() )
              {
                decoupler->listener->handle(decoupler, object);
              }
            }
          }

          /**
           * Test previous way instead of the following to free the condition var
           * while processing the event.
           * It coud be a problem depending on the template class.
           *
          // (the MutexLock auto var will release the monitor at the end)
          posix::Mutex::MutexLock lock(monitor);
          bool result = monitor.timedWait(100);
          if (result)
          {
            E object = queue.front();
            queue.pop_front();

            if (listener != 0)
              listener->handle(this, object);
          }
           */
        }

        if ( (decoupler->listener != 0) && (decoupler->queue.size() > 0))
        {
          std::cout << om_timef << " [Decoupler::run]: purging queue..." << std::endl;
          while (decoupler->queue.size() > 0)
          {
            E object = decoupler->queue.front();
            decoupler->queue.pop_front();
            decoupler->listener->handle(decoupler, object);
          }
          std::cout << om_timef << " [Decoupler::run]: ...end purge" << std::endl;
        }

        std::cout << om_timef << " [Decoupler::run]: run terminated" << std::endl;
        return 0;
      }

    };
    DecouplerThread* decouplerThread;

  public:
    Decoupler(DecouplerListener* listener, const E& defaultValue)
      : queue(), monitor(), listener(listener), defaultValue(defaultValue)
    {
      decouplerThread = new DecouplerThread(this);
//      posix::getThreadManager()->manage(decouplerThread, true);
      decouplerThread->start();
    }

    ~Decoupler()
    {
      std::cout << "[Decoupler::~Decoupler]: terminating..." << std::endl;
      if (decouplerThread != 0)
      {
        if (!decouplerThread->terminated())
        {
          decouplerThread->terminate();
          monitor.signal();
          std::cout << om_timef << " waiting for decouplerThread termination ..." << std::endl;
          while (!decouplerThread->isTerminated())
            sleep(1);
          std::cout << om_timef << " ... decouplerThread terminated" << std::endl;
          std::cout << om_timef << " waiting for decouplerThread join ..." << std::endl;
          decouplerThread->join();
          std::cout << om_timef << " ... decouplerThread joined" << std::endl;
        }
        delete decouplerThread;
      }
    }

    /**
     * Push an item into de decoupler.
     */
    bool push(E object, DecouplerEventType detype = detMULTIPLE)
    {
      if (decouplerThread->terminated())
        return false;

      // (the MutexLock auto var will release the monitor at the end)
      posix::Mutex::MutexLock lock(monitor, ".Decoupler::push");

      bool addok = true;

      if (detype == detEXCLUSIVE)
      {
        bool found = false;
        // look if the event already exists
        typename std::list<E>::const_iterator it = queue.begin();
        while ( (!found) && (it != queue.end()))
        {
          found = (object == *it);
          ++it;
        }
        // add only if the element does not exist in the queue
        addok = !found;
      }
      else if (detype == detSINGLE_LAST)
      {
        queue.clear();
        addok = true; // always
      }

      if (addok)
        queue.push_back(object);

      monitor.signal();
      return addok;
    }

    int size() const { return queue.size(); }

  private:
    std::list<E> queue;
    posix::Monitor monitor;

    DecouplerListener* listener;

  protected:
    const E& defaultValue;
  };

}

#endif /* _DECOUPLER_H_ */
