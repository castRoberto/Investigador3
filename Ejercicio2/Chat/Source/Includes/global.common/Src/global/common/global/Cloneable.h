#ifndef _CLONEABLE_H_
#define _CLONEABLE_H_

class Cloneable
{
public:
  virtual ~Cloneable() {}

  virtual Cloneable* clone() const = 0;
};

#endif /*_CLONEABLE_H_*/
