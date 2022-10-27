#ifndef _LISTENER_H_
#define _LISTENER_H_

/**
 * Defines an interface with a callback method.
 */
class Listener
{
public:
  virtual ~Listener() {}

	/**
   * Callback method.
   * Don't forget to static_cast the data to the appropiate type. 
	 */
	virtual void handle(void* source, const void* data) = 0;	
};

/**
 * Over-definition with some data type specification through templates.
 */
template<class T>
class TListener
{
public:
  virtual ~TListener() {}

  virtual void handle(void* source, T data) = 0;  
};

#endif /*_LISTENER_H_*/
