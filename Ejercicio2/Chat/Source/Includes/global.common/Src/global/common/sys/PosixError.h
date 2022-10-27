#ifndef _POSIXERROR_H_
#define _POSIXERROR_H_

#include "../global/Exception.h"

namespace posix
{

  class PosixError : public util::Exception
  {
  public:
    /** Construct a new pthread error object. */
    explicit PosixError(const std::string &desc, int err = 0)
      : util::Exception(desc, err, "PosixError") {}
  };

}

#endif /*_POSIXERROR_H_*/
