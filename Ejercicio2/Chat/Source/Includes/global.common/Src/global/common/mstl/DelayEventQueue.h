/*
 * DelayEventQueue.h
 *
 *  Created on: Jul 25, 2016
 *      Author: ayachan
 */

#ifndef DELAYEVENTQUEUE_H_
#define DELAYEVENTQUEUE_H_

#include "Delayed.h"
#include "DelayQueue.h"

#include "../sys/thread/Thread.h"

namespace util
{
  /**
   * An object that listen to blocking queues and report available events in a
   * call back method.<br />
   * Instances should provide the event process listener.
   */
  class DelayEventQueue
  {

    static const long DEFAULT_QUEUE_ms = 200;

  public:
    struct DelayEventProcess
    {
      virtual ~DelayEventProcess() {}
      virtual void process(const Delayed* event) = 0;
    };

  private:
    struct DelayEventQueueThread : public posix::Thread
    {
      DelayEventQueue* delayEventQueue;
      DelayEventQueueThread(DelayEventQueue* delayEventQueue)
        : delayEventQueue(delayEventQueue) {}
      /**
       * The cycle where the thread detects new events and call back the
       * registered listener.
       */
      void* run(void* args = 0)
      {
        while (!terminated())
        {
          const Delayed* pevent =
              delayEventQueue->dqueue.front(delayEventQueue->timeout_ms);
          if (pevent != 0)
          {
            /*
             * pop could remove "other" element inserted just after the front
             * logic...?
             *
             */
//            delayEventQueue->dqueue.pop();
            pevent = delayEventQueue->dqueue.remove(pevent);
            if (pevent != 0)
            {
//              util::timestamp ts;
              delayEventQueue->process->process(pevent);
//              std::cout << om_timef << " ~~~ " << ts.elapsed_ms() << " [ms] for " << pevent << std::endl;
            }
          }
        }
        return 0;
      }
    };
    DelayEventQueueThread* delayEventQueueThread;

  public:
    DelayEventQueue(
        DelayEventProcess* process, long timeout_ms = DEFAULT_QUEUE_ms)
      : process(process), timeout_ms(timeout_ms), dqueue()
    {
      delayEventQueueThread = new DelayEventQueueThread(this);
      delayEventQueueThread->start();
    }
    virtual ~DelayEventQueue()
    {
      terminate();
    }

    void terminate()
    {
      if (delayEventQueueThread != 0)
      {
        if (!delayEventQueueThread->terminated())
        {
          delayEventQueueThread->terminate();
          delayEventQueueThread->join();
        }
        delete delayEventQueueThread;

        delayEventQueueThread = 0;
      }
    }

    virtual void add(const Delayed* event)
    {
//      std::cout << om_timef << " adding event with delay " <<
//          event->getDelay() << "[ms]" << std::endl;
      dqueue.push(event);
    }
    const Delayed* remove(const Delayed* event)
    {
      return dqueue.remove(event);
//      dqueue.remove(event);
    }

  private:
    DelayEventProcess* process;
    long timeout_ms;
    DelayQueue dqueue;
  };

}

#endif /* DELAYEVENTQUEUE_H_ */
