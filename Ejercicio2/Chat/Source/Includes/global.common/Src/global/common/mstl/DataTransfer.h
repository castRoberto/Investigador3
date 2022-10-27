//#pragma once // open when GCC 3.4

#ifndef _DATATRANSFER_H_
#define _DATATRANSFER_H_

/*
 * Jul 2, 2009 - ayachan
 */

#include "../global/Runnable.h"
#include "../global/Block.h"

#include "PushPullQueue.h"

#include "../sys/thread/EndlessThread.h"
#include "../sys/thread/ThreadManager.h"

#include "../util/MemBlock.h"
#include "../sys/AtomicOps.h"

namespace util
{

  /*
   * This set of classes allow to break the calling queue when processing
   * arriving data.
   * The quiz is that a data reception event can trigger a lot of processing
   * eventually coming back to the caller itself (as, for example, a heartbeat
   * signal can do). The purpose is to break that call-process queue in order to
   * free the caller to process other asynchronous events.
   */

  /**
   * Interface to listen for incomming data.
   */
  template <class T>
  struct DataTransferListener
  {
    virtual ~DataTransferListener() {}
    virtual void dataReceived(T source, const void* data, int datalen) = 0;
  };

  /**
   * This class is an implementation of the above in order to desagregate the
   * calling queue.
   * This class process in an endless thread that only read the buffer queue.
   */
  template <class T>
  class DataTransferBufferedListener : public DataTransferListener<T>,
                                       public Runnable
  {
  public:
    DataTransferBufferedListener(DataTransferListener<T>* targetListener)
      : targetListener(targetListener), enabled(false), queue(), terminated(true)
    {
      startDataTransfer();
    }

    ~DataTransferBufferedListener()
    {
      stopDataTransfer();
    }

    /**
     * @inherited from DataTransferListener<T>
     */
    void dataReceived(T source, const void* data, int datalen)
    {
      queue.push(new TransferItem(source, MemBlock::duplicate(data, datalen)));
    }

    void enable(bool value)
    {
      enabled = value;
    }

    /**
     * @inherited from Runnable
     */
    void* run(void* args = 0)
    {
      /*
       * Process can end either from this side as from the (calling) thread.
       */
      posix::Thread* callingThread = static_cast<posix::Thread*>(args);
      if (terminated || callingThread->terminated())
      {
        terminated = true;
        if (!callingThread->terminated())
          callingThread->terminate();
      }
      else
      {
        TransferItem* ti = queue.pull(500);
        if (ti != 0)
        {
          if (enabled && (targetListener != 0))
            targetListener->dataReceived(
                ti->source, ti->block->getMem(), ti->block->getLength());
          delete ti;
        }
      }
      return 0;
    }

  protected:
    void startDataTransfer()
    {
      if (posix::AtomicOps::testAndSet(terminated, false, "DataTransfer.start"))
        return;

      cycleThread = new posix::EndlessThread(this, ".DataTransfer");
      posix::getThreadManager()->manage(cycleThread, true);
      cycleThread->start();
      enable(true);
    }
    void stopDataTransfer()
    {
      if (posix::AtomicOps::testAndSet(terminated, true, "DataTransfer.stop"))
        return;

      enable(false);
      cycleThread->terminate();
    }

  private:
    DataTransferListener<T>* targetListener;
    bool enabled;

  private:
    /**
     * The item need to be buffered and then sent.
     */
    struct TransferItem
    {
      T source;
      Block* block;

      TransferItem(T source, Block* block) : source(source), block(block) {}
      ~TransferItem() { delete block; }
    };

    PushPullQueue<TransferItem> queue;

    posix::EndlessThread* cycleThread;
    bool terminated;
  };

}

#endif /* _DATATRANSFER_H_ */
