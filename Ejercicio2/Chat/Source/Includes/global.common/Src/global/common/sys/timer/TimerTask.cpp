
#include "TimerTask.h"
#include "Timer.h"

namespace posix
{

  void TimerTask::cancel()
  {
    assert(timer != 0);
    timer->cancel(this);
  }

} // namespace
