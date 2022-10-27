/*
 * RuledEventDelayQueueEx.h
 *
 *  Created on: Nov 17, 2016
 *      Author: ayachan
 */

#ifndef RULEDDELAYQUEUEEX_H_
#define RULEDDELAYQUEUEEX_H_

#include "IBlockingQueue.h"
#include "BlockingQueueEx.h"
#include "EventQueueEx.h"

#include <vector>
#include <list>

#include "../util/omanips.h"
#include "../util/timestamp.h"
#include "../mstl/Delayed.h"

#include <iostream>
#include <limits.h>

namespace util
{

  /**
   * The possible actions related to rule processing.
   */
  enum RuleAction
  {
    // do nothing (keep existing, ignore incoming, do not compose)
    RULE_NOTHING,
    // keep existing and add incoming (no new composed)
    RULE_KEEP_BOTH,
    // discard existing for incoming (no new composed)
    RULE_KEEP_INCOMING,
    // discard existing and incoming for (eventual) new composed
    RULE_DISCARD_BOTH
  };

  /**
   * Blocking sorted queue where items are processed by rules when inserted.
   * The rules can determinate if an incoming element has or not relation with
   * other existing elements in the queue and perform an action.
   */
  template <class T, class Op>
  class RuledDelayQueueEx
      : public IBlockingQueue<T>,
        public EventQueueEx<T>::EventProcess
  {
  public:
    struct MatchRule
    {
      /**
       * Test about an element.
       *
       * @param element
       *          The element to be tested.
       * @return TRUE if the element match.
       */
      virtual bool imatch(const T* element) const = 0;
    };

    /**
     * Define a rule to apply to each element to be inserted in the queue.
     */
    struct Rule : public MatchRule
    {
      virtual ~Rule() {}

      /**
       * Test about the element to insert.
       *
       * @param incoming
       *          The element to be inserted.
       * @return TRUE if the incoming element match the intended element type.
       */
      virtual bool imatch(const T* incoming) const = 0;
      /**
       * Test about an existing element in the queue.
       * It can be assumed that the incoming element previously match the
       * 'imatch' test.
       *
       * @param existing
       *          The existing element in the queue.
       * @param incoming
       *          The element to be inserted.
       * @return TRUE if the existing element match the intended relation with
       *         the incoming element.
       */
      virtual bool ematch(const T* existing, const T* incoming) const = 0;
      /**
       * Resolve about the final solution that turns about what to do with:
       *   - the existing record,
       *   - the incoming record, and
       *   - an eventual new "composed" record (that replace previous both)
       *
       * so the action could be:
       *
       * 1. do nothing (keep existing, ignore incoming, do not compose).
       * 2. keep existing and add incoming,
       * 3. discard existing for incoming,
       * 4. discard existing and incoming for (new) composed.
       *
       * That method should safely assume that the incoming and the existing
       * elements previously match the 'imatch' and the 'ematch' tests.
       *
       * IMPORTANT:
       * This method must *not* touch (free) the objects discarded, if any.
       *
       * @param existing
       *          The existing element in the queue.
       * @param incoming
       *          The element to be inserted.
       * @return The action and the eventual element to be inserted in the queue
       *         if happen.
       */
      virtual std::pair<RuleAction, const T*> proc(const T* existing, const T* incoming) const = 0;
    };

  public:
    RuledDelayQueueEx(const std::string& title = "RuledDelayQueueEx")
      : IBlockingQueue<T>(), rm(), queue(), iqueue(), eiqueue(&iqueue, this, title), rules() {}
    ~RuledDelayQueueEx()
    {
      // need to stop event retrieving
      eiqueue.terminate();
    }

  public:
    void addRule(const Rule* rule)
    {
      posix::Mutex::MutexLock lock(rm, ".RuledDelayQueueEx::addRule");
      rules.push_back(rule);
    }

    /*
     * (from EventQueueEx<T>::EventProcess)
     */
    void process(const T* event)
    {
      posix::Mutex::MutexLock lock(rm, ".RuledDelayQueueEx::process");

//      std::cout << om_timef << " ~~~~~ processing " << event << std::endl;

      /*
       * default is to get incoming doing nothing on the queue (it happens at
       * start when the queue is empty)
       */
      std::pair<RuleAction, const T*> result =
          std::make_pair(RULE_KEEP_BOTH, (const T*)0);
      // (in some situations we will need to know existing record location...)
      typename std::list<const T*>::iterator eit = queue.end();

      // only if something in the queue
      if (queue.size() > 0)
      {
        for (
            typename std::vector<const Rule*>::const_iterator rit = rules.begin();
            rit != rules.end(); ++rit)
        {
          const Rule* rule = *rit;

          // forget if not match the incoming
          if (!rule->imatch(event))
            continue;

          // check on current event queue
          for (eit = queue.begin(); eit != queue.end(); ++eit)
          {
            const T* existing = *eit;
            // check if match the existing event check
            if (rule->ematch(existing, event))
            {
//            std::cout << om_timef << " ~~~~~ matched " << *eit << std::endl;
              result = rule->proc(existing, event);
              break; // eit points to the existing
            }
          }
        }
      }

      bool changed = false;
      switch (result.first)
      {
        case RULE_NOTHING:
        {
          // do nothing (keep existing, ignore incoming, do not compose)
          std::cout << " RULE_NOTHING ignoring " << event << std::endl;
          delete event;
        }
          break;
        case RULE_KEEP_BOTH:
        {
          // keep existing and add incoming (no new composed)
          typename std::list<const T*>::iterator next =
              upper_bound(queue.begin(), queue.end(), event, Op::lt);
          queue.insert(next, event);
          changed = true;
        }
          break;
        case RULE_KEEP_INCOMING:
        {
          // discard existing for incoming (no new composed)
          std::cout << " RULE_KEEP_INCOMING incoming " << event << std::endl;

          if (eit != queue.end())
          {
            const T* existing = *eit;
            queue.erase(eit);
            delete existing;
          }

          typename std::list<const T*>::iterator next =
              upper_bound(queue.begin(), queue.end(), event, Op::lt);
          queue.insert(next, event);
          changed = true;
        }
          break;
        case RULE_DISCARD_BOTH:
        {
          // discard existing and incoming for (eventual) new composed
          std::cout << " RULE_DISCARD_BOTH incoming " << event << std::endl;

          if (eit != queue.end()) // just in case
          {
            const T* existing = *eit;
            queue.erase(eit);
            delete existing;
          }
          delete event;

          if (result.second != 0)
          {
            typename std::list<const T*>::iterator next =
                upper_bound(queue.begin(), queue.end(), result.second, Op::lt);
            queue.insert(next, result.second);
          }
          changed = true;
        }
          break;
      }

      if (changed)
      {
//        std::cout << om_timef << " queue after input processing: " << queue.size() << std::endl;
//        for (
//            typename std::list<const T*>::const_iterator cit = queue.begin();
//            cit != queue.end(); ++cit)
//        {
//          const util::Delayed* delayed = (const util::Delayed*)*cit;
//          std::cout << " ... " << delayed << std::endl;
//        }

        rm.signal();
      }
    }

    /**
     * Overloaded to apply rules.
     */
    void push(const T* value)
    {
//      posix::Mutex::MutexLock lock(rm, ".RuledDelayQueueEx::push");
//      std::cout << om_timef << " ~~~~~ pushing " << value << std::endl;
      iqueue.push(value);
//      std::cout << om_timef << " ruled queue after push: " << iqueue.size() << std::endl;
    }

    const T* front(long ms = 0)
    {
      posix::Mutex::MutexLock lock(rm, ".RuledDelayQueueEx::front");
      util::timestamp tsnow;

      long topDelay = queue.empty() ? LONG_MAX : Op::delay(queue.front());
//      std::cout << om_timef << " --> top delay is " << topDelay << " [ms]" << std::endl;
      while
        ( (tsnow.elapsed_ms() < ms) &&
          (queue.empty() || (Op::delay(queue.front()) > 0)))
      {
        /*
         * Someone could insert an element in the meantime!
         */
        topDelay = queue.empty() ? LONG_MAX : Op::delay(queue.front());
//        std::cout << om_timef << " --> loop top delay is " << topDelay << " [ms]" << std::endl;
        if (topDelay <= 0)
          break;

        long remaining = ms - (long) tsnow.elapsed_ms();
        if (remaining <= 0)
          break;

        long waitTime_ms = (topDelay < remaining) ? topDelay : remaining;
//        std::cout << om_timef << "     waiting " << waitTime_ms << " [ms]" << std::endl;
        rm.timedWait(waitTime_ms, "RuledDelayQueueEx");
      }

      const T* result = 0;
      if (queue.empty() || (Op::delay(queue.front()) > 0))
        result = 0;
      else
      {
        result = queue.front();
        queue.remove(result);
//        std::cout << om_timef << " returning something" << std::endl;
      }
      return result;
    }

    /**
     * Remove all elements in the list that match the given rule.
     */
    int remove(const MatchRule* matchRule)
    {
      posix::Mutex::MutexLock lock(rm, ".RuledDelayQueueEx::remove (rule)");
      int result = 0;

      for (
          typename std::list<const T*>::iterator iter = queue.begin();
          iter != queue.end(); ++iter)
      {
        if (matchRule->imatch(*iter))
        {
          std::cout << " ~~~~~ removed " << *iter << std::endl;
          iter = queue.erase(iter);
          result += 1;
        }
      }
      return result;
    }

    bool remove(const T* element)
    {
      posix::Mutex::MutexLock lock(rm, ".RuledDelayQueueEx::remove");
      bool result = false;

      std::list<const Delayed*>::iterator elem =
          find(queue.begin(), queue.end(), element);
      if (elem != queue.end())
      {
        queue.erase(elem);
        result = true;
      }

      return result;
    }

    void pop()
    {
      if (queue.empty() || (Op::delay(queue.front()) > 0))
      {
        // not yet
      }
      else
      {
        posix::Mutex::MutexLock lock(rm, ".RuledDelayQueueEx::pop");
//        std::cout << om_timef << " --> pop top with delay " <<
//            Op::delay(queue.front()) << std::endl;
        queue.pop_front();
      }
    }

    int size() const
    {
      return queue.size();
    }

    void clear()
    {
      queue.clear();
    }

  private:
    posix::Monitor rm;

    std::list<const T*> queue;

    BlockingQueueEx<T> iqueue;
    EventQueueEx<T> eiqueue;

    std::vector<const Rule*> rules;
  };
}

#endif /* RULEDDELAYQUEUEEX_H_ */
