/*
 * EventQueueEx.h
 *
 *  Created on: Nov 17, 2016
 *      Author: ayachan
 */

#ifndef EVENTQUEUEEX_H_
#define EVENTQUEUEEX_H_

#include "IBlockingQueue.h"

#include "../sys/thread/Thread.h"

//#include "../util/timestamp.h"
#include "../util/omanips.h"
//
//#include <time.h>

#include <iostream>

namespace util
{

  /**
   * Facility to continuously request a blocking queue calling a callback method
   * each time an element is available. Users must provide the 'EventProcess'
   * instance to process the records.
   *
   * IMPORTANT:
   * The available element is extracted from the queue, but the callback process
   * must free the object, as apply.
   *
   * EXAMPLE:
   *
   * // suppose our type to store are int's'
   *
   * // the delayed ops
   * struct DelayedOps
   * {
   *   static bool lt(const int* first, const int* second)
   *   {
   *     return *first < *second;
   *   }
   *
   *   static long delay(const int* d)
   *   {
   *     return *d;
   *   }
   * };
   *
   * // define the processor callback
   * struct IntProcFunction
   *     : public util::EventQueueEx<int>::EventProcess
   * {
   *   void process(const int* value)
   *   {
   *     // do here my process
   *     delete value;
   *   }
   * };
   *
   * // inside the program
   *
   *   // ... the processor instance
   *   IntProcFunction eproc;
   *
   *   // ... the queue
   *   // ... ... the event processor
   *   IntProcFunction eproc;
   *   // ... ... the queue itself
   *   util::RuledDelayQueueEx<util::Delayed, DelayedOps> rdq;
   *   // ... ... the processor
   *   util::EventQueueEx<util::Delayed> eq(&rdq, &eproc);
   *
   *
   */
  template <class T>
  class EventQueueEx
  {
  public:
    /**
     * The event callback definition.
     */
    struct EventProcess
    {
      virtual ~EventProcess() {}
      virtual void process(const T* event) = 0;
    };

  private:
    static const long DEFAULT_QUEUE_ms = 1000;

    /**
     * The poll thread.
     */
    struct PollThread : public posix::Thread
    {
      IBlockingQueue<T>* bq;
      EventProcess* eproc;
      std::string title;

      PollThread(IBlockingQueue<T>* bq, EventProcess* eproc,
        const std::string& title = "untitled")
        : posix::Thread(0, 0, 0), bq(bq), eproc(eproc), title(title) {}

      void* run(void* args = 0)
      {
        while (!terminated())
        {
//          std::cout << om_timef << "[" << title << "] looking event queue..." << std::endl;
          const T* dl = bq->front(DEFAULT_QUEUE_ms);
          if (dl != 0)
          {
//            std::cout << om_timef << " found " << dl << std::endl;
            // first remove from the queue because it will be deleted in 'process'
//            std::cout << "[" << title << "]" << " ~~~~~ removing and sending: " << dl << std::endl;
//            bq->remove(dl);
            eproc->process(dl);
          }
        }
        return 0;
      }
    };

  public:
    EventQueueEx(IBlockingQueue<T>* bq, EventProcess* eproc,
        const std::string& title = "untitled")
      : pollThrd(bq, eproc, title)
    {
      pollThrd.start();
    }
    ~EventQueueEx()
    {
      terminate();
    }

    void terminate()
    {
      pollThrd.terminate();
    }

  private:
    PollThread pollThrd;
  };
}

#endif /* EVENTQUEUEEX_H_ */
