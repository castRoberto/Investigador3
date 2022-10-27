#ifndef _INFORMATIONMANAGER_H_
#define _INFORMATIONMANAGER_H_

/*
 * Mar 5, 2009 - ayachan
 */

#include <list>
#include <map>
#include <string>

//#include <iostream>

namespace util
{

  class InformationItem;
  class InformationItemObserver;

  /**
   * Manages common information.
   *
   * TODO add synchronism in access common lists
   */
  class InformationManager
  {
  public:
    /**
     * Retrieve a handle to the information item related to the given name.
     *
     * The item must already be registered in order to be able to link it. The
     * registering process is perform by the information item itself when
     * created.
     */
    static InformationItem* link(
        const std::string& name, InformationItemObserver* observer = 0);

    /**
     * Release the handle (to countdown).
     */
    static void unlink(const std::string& name);

    /**
     * Register this information item.
     */
    static void registerInformationItem(InformationItem* info);

  public:
    InformationManager() : table() {}

  private:
    /**
     * The list of currently allocated information item's.
     */
    std::map<std::string,InformationItem*> table;

  private:
    /**
     * The single instance of this manager.
     *
    static InformationManager* im;
     */
  public:
    static InformationManager* getInstance();
    static void releaseInstance();
  };

  /**
   * Stub container for any information.
   */
  class InformationItem
  {
  public:
    InformationItem(const std::string& name) : name(name), observers()
    {
      InformationManager::registerInformationItem(this);
    }
    virtual ~InformationItem()
    {
      InformationManager::unlink(name);
    }

    const std::string& getName() const { return name; }

    /**
     * Add an observer to this information item.
     */
    void addObserver(InformationItemObserver* observer);
    /**
     * Remove an observer.
     */
    void removeObserver(InformationItemObserver* observer);

  protected:
    virtual void notifyObserver(InformationItemObserver* observer);
    /**
     * Notify all observers of this information item.
     */
    void notifyAll();

  private:
    std::string name;
    std::list<InformationItemObserver*> observers;
  };

  /**
   * The information item on change callback.
   */
  class InformationItemObserver
  {
  public:
    virtual ~InformationItemObserver() {}
    virtual void informationChange(const InformationItem* info) = 0;
  };

  template<class tinfo>
  class TInformationItem : public InformationItem
  {
  public:
    TInformationItem(const std::string& name) : InformationItem(name), info() {}
    tinfo& operator*() { return info; }
//    const tinfo& operator*() const { return info; }

    void operator=(const tinfo& info)
    {
      set(info);
    }
    virtual void set(const tinfo& info)
    {
//      std::cout <<
//        "[InformationItem]: +++++++++++++++++++++++++++++++++++++" << std::endl;
//      std::cout << "[InformationItem]: assigning to " << getName() <<
//                   ", value " << info << std::endl;
      this->info = info;
      notifyAll();
    }
    const tinfo& get() const { return info; }
  protected:
    tinfo info;
  };

}

typedef util::TInformationItem<bool> BoolInfoItem;
typedef util::TInformationItem<long> LongInfoItem;
// [20091126] aya
typedef util::TInformationItem<std::string> StringInfoItem;

#endif /* _INFORMATIONMANAGER_H_ */
