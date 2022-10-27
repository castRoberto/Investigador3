/*
 * EventQueue.h
 *
 *  Created on: May 2, 2016
 *      Author: ayachan
 */

#ifndef _EVENTQUEUE_H_
#define _EVENTQUEUE_H_

#include "../sys/thread/Thread.h"
#include "BlockingQueue.h"

//#include "../util/omanips.h"
//#include <iostream>

namespace util
{

  /**
   * An object that listen to blocking queues and report available events in a
   * call back method.<br />
   * Instances should provide the event process listener.
   */
  template <class T>
  class EventQueue
  {
    static const long DEFAULT_QUEUE_ms = 200;

  public:
    struct EventProcess
    {
      virtual ~EventProcess() {}
      virtual void process(T event) = 0;
    };

  private:
    struct EventQueueThread : public posix::Thread
    {
      EventQueue* eventQueue;
      const T not_an_element;
      EventQueueThread(EventQueue* eventQueue, const T& not_an_element)
        : eventQueue(eventQueue), not_an_element(not_an_element) {}
      /**
       * The cycle where the thread detects new events and call back the
       * registered listener.
       */
      void* run(void* args = 0)
      {
        while (!terminated())
        {
          T pevent = eventQueue->ppqueue.front(eventQueue->timeout_ms);
          if (pevent != not_an_element)
          {
//            std::cout << om_timef << " something retrieved!" << std::endl;
            eventQueue->ppqueue.pop();
            eventQueue->process->process(pevent);
          }
          else
          {
//            std::cout << om_timef << " returned not-an-element" << std::endl;
          }

        }
        return 0;
      }
    };
    EventQueueThread* eventQueueThread;

  public:
    EventQueue(const T& not_an_element,
        EventProcess* process, long timeout_ms = DEFAULT_QUEUE_ms)
      : process(process), timeout_ms(timeout_ms), ppqueue(not_an_element)
    {
      eventQueueThread = new EventQueueThread(this, not_an_element);
      eventQueueThread->start();
    }
    ~EventQueue()
    {
      terminate();
    }

    void terminate()
    {
      if (eventQueueThread != 0)
      {
        if (!eventQueueThread->terminated())
        {
          eventQueueThread->terminate();
          eventQueueThread->join();
        }
        delete eventQueueThread;

        eventQueueThread = 0;
      }
    }

    void add(T event)
    {
      ppqueue.push(event);
    }

  private:
    EventProcess* process;
    long timeout_ms;
    BlockingQueue<T> ppqueue;
  };

}

#endif /* _EVENTQUEUE_H_ */
