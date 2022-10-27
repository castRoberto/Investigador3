
#include "CondVar.h"
#include "../PosixError.h"

#include "../../util/timespec.h"

#include <errno.h>
#include <string.h>

namespace posix
{

  CondVar::CondVar(Mutex& mutex)
   : mutex(mutex)
  {
    int result;
    if ( (result = pthread_cond_init(&condition, 0)) != 0)
      throw PosixError("Can't initialize condition var", result);
  }
  
  CondVar::~CondVar()
  {
    int result;
    if ( (result = pthread_cond_destroy(&condition)) != 0)
      std::cerr << "Can't destroy condition var. Error " << result << std::endl;
  }
  
  void CondVar::wait()
  {
    int result;
    if ( (result = pthread_cond_wait(&condition, &mutex.mutex)) != 0)
      throw PosixError("Can't wait for condition var", result);
  }
  
  void CondVar::signal()
  {
    int result;
    if ( (result = pthread_cond_signal(&condition)) != 0)
      throw PosixError("Can't signal condition var", result);
  }
  
  void CondVar::broadcast()
  {
    int result;
    if ( (result = pthread_cond_broadcast(&condition)) != 0)
      throw PosixError("Can't broadcast condition var", result);
  }
  
  bool CondVar::timedWait(long ms, const std::string& from)
  {
    bool success = false;
  
    struct timespec ts = abstime() + (ms * 1000);
    int result = pthread_cond_timedwait(&condition, &mutex.mutex, &ts);
    if (result == 0)
      success = true;
    else if (result == ETIMEDOUT)
      success = false;
    else
    {
      std::cerr << "[CondVar] Error " << result << ": " <<
                   strerror(result) << " from " << from << std::endl;
//      throw PosixError("Can't wait for condition var", result);
    }
  
    return success;
  }

}
