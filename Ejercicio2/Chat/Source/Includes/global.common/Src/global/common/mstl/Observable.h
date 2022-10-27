/*
 * Observable.h
 *
 *  Created on: Oct 11, 2017
 *      Author: ayachan
 */

#include "../queue/BlockingQueueEx.h"
#include "../queue/EventQueueEx.h"

#include <algorithm>
#include <list>

#ifndef OBSERVABLE_H_
#define OBSERVABLE_H_

namespace mstl
{

  /*
   * Event hierarchy base and structures.
   */

  struct ObservableEventType
  {
    enum Value { ADD_OBSERVER, REMOVE_OBSERVER, NOTIFY };
  };
  struct ObservableEvent
  {
    ObservableEventType::Value type;
    ObservableEvent(ObservableEventType::Value type) : type(type) {}
  };

  template <class T>
  class Observable : public util::EventQueueEx<ObservableEvent>::EventProcess
  {
  public:
    /**
     * The observer type to subscribe to this observable.
     */
    struct Observer
    {
      ~Observer() {}
      virtual void notification(const Observable* observable, const T& data) = 0;
    };

  public:
    Observable() : bq(), queue(&bq, this), observers(), terminated(false) {}
    ~Observable()
    {
      terminate();
    };

    void terminate()
    {
      if (terminated)
        return;
      terminated = true;
      queue.terminate();
    }

    void process(const ObservableEvent* event)
    {
      switch (event->type)
      {
        case ObservableEventType::ADD_OBSERVER:
        {
          ObserverEvent* obs = (ObserverEvent*) event;
          observers.push_back(obs->observer);
        }
          break;
        case ObservableEventType::REMOVE_OBSERVER:
        {
          ObserverEvent* obs = (ObserverEvent*) event;

          typename std::list<Observer*>::iterator iter =
              find(observers.begin(), observers.end(), obs->observer);
          if (iter != observers.end())
            observers.erase(iter);
        }
          break;
        case ObservableEventType::NOTIFY:
        {
          NotifyEvent* nevt = (NotifyEvent*) event;
          for(
              typename std::list<Observer*>::const_iterator cit = observers.begin();
              cit != observers.end(); ++cit)
          {
            Observer* observer = *cit;
            observer->notification(this, nevt->data);
          }
        }
          break;
      }
      delete event;
    }

  private:
    struct ObserverEvent : public ObservableEvent
    {
      Observer* observer;
      ObserverEvent(ObservableEventType::Value op, Observer* observer)
        : ObservableEvent(op), observer(observer) {}
    };
    struct NotifyEvent : public ObservableEvent
    {
      T data;
      NotifyEvent(const T& data)
        : ObservableEvent(ObservableEventType::NOTIFY), data(data) {}
    };

    util::BlockingQueueEx<ObservableEvent> bq;
    util::EventQueueEx<ObservableEvent> queue;

    std::list<Observer*> observers;
    bool terminated;

  public:

    void addObserver(Observer* observer)
    { bq.push(new ObserverEvent(ObservableEventType::ADD_OBSERVER, observer)); }
    void removeObserver(Observer* observer)
    { bq.push(new ObserverEvent(ObservableEventType::REMOVE_OBSERVER, observer)); }

    void notify(const T& data)
    { bq.push(new NotifyEvent(data)); }
  };
}

#endif /* OBSERVABLE_H_ */
