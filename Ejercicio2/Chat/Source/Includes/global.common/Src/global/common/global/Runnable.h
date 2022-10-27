#ifndef _RUNNABLE_H_
#define _RUNNABLE_H_

class Runnable
{
public:
  virtual ~Runnable() {}

  virtual void* run(void* args = 0) = 0;
};

#endif /*_RUNNABLE_H_*/
