/*
 * Mar 5, 2009 - ayachan
 */

#include "InformationManager.h"

#include <iostream>

namespace util
{
  /*
   * about InformationItem
   */

  void InformationItem::addObserver(
      InformationItemObserver* observer)
  {
    observers.push_back(observer);
  }

  void InformationItem::removeObserver(
      InformationItemObserver* observer)
  {
    observers.remove(observer);
  }

  void InformationItem::notifyObserver(
      InformationItemObserver* observer)
  {
    observer->informationChange(this);
  }

  void InformationItem::notifyAll()
  {
    int count = 0;

    //std::cout << "[InformationItem] Notificando "
    //          << name << ", " << observers.size() << " observers" << std::endl;

    std::list<InformationItemObserver*>::iterator it = observers.begin();
    while (it != observers.end())
    {
      notifyObserver(*it);
      it++;

      count++;
    }

    //std::cout <<
    //    "[InformationItem]: +++++++++++++++++++++++++++++++++++++" << std::endl;
    //std::cout << "[InformationItem]: " << name << ", " << count << " observers notified." << std::endl;
  }

  /*
   * about static InformationManager methods
   *
  InformationManager* InformationManager::im = 0;
   */

  InformationItem* InformationManager::link(
      const std::string& name, InformationItemObserver* observer)
  {
    InformationManager* im = InformationManager::getInstance();

    InformationItem* item = 0;
    std::map<std::string,InformationItem*>::iterator it = im->table.find(name);
    if (it != im->table.end())
    {
      item = it->second;
      if (observer != 0)
      {
        item->addObserver(observer);
        //std::cout << "[InformationItem]: subscribing to " << item->getName() << std::endl; 
      }
      else
      {
        //std::cout << "[InformationItem]: subscripcion SIN callback a " << name << std::endl;
      }
    }
    else
    {
      //std::cout << "[InformationItem]: ERROR! NO ENCUENTRA " << name << std::endl;
    }
    return item;

    // TODO counting
  }

  void InformationManager::unlink(const std::string& name)
  {
    // TODO countdown
  }

  void InformationManager::registerInformationItem(InformationItem* info)
  {
    InformationManager* im = InformationManager::getInstance();

    im->table.insert(std::make_pair(info->getName(), info));
  }

  InformationManager* InformationManager::getInstance()
  {
    static InformationManager im;
    return &im;

//    if (im == 0)
//      im = new InformationManager();
//    return im;
  }
  void InformationManager::releaseInstance()
  {
//    if (im != 0)
//      delete im;
  }

}
