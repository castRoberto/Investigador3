/*
 * Jun 20, 2009 - ayachan
 */

#include "omanips.h"
#include "timestamp.h"

#include "../sys/thread/Thread.h"
#include "UtilString.h"

#include <iomanip>

std::ostream& om_line(std::ostream& os)
{
  return os << "----------------------------------------"
            << "----------------------------------------\n";
}

std::ostream& om_time(std::ostream& os)
{
  double seconds = util::timestamp::getTimestamp();
  return os << std::fixed << std::setprecision(4) << seconds << " ";
}

std::ostream& om_timef(std::ostream& os)
{
  double seconds = util::timestamp::getTimestamp();

  int minutes = (int)(seconds / 60.0);
  seconds -= (minutes * 60);
  int hours = (int)(1.0 * minutes / 60.0);
  minutes -= (hours * 60);
  hours = hours % 24;

  int iseconds = (int)seconds;
  seconds -= iseconds;
  int decimals = (int)(seconds * 1000000.0 + 0.5);

  std::string s_hour = util::UtilString::format("%02d:%02d:%02d",
      hours, minutes, iseconds);
  std::string s_decs = util::UtilString::format("%06d", decimals);
  return os << s_hour << "." << s_decs;
//  return os << std::dec << std::setw(2) << std::setfill('0') << hours <<
//               ":" << std::setw(2) << std::setfill('0') << minutes <<
//               ":" << std::setw(2) << std::setfill('0') << iseconds <<
//               "." << std::setw(6) << std::setfill('0') << decimals;
}

std::ostream& om_thread(std::ostream& os)
{
  std::string logmsg = util::UtilString::format("[0x%03x] ",
      posix::Thread::getCurrentThread());
  return os << logmsg;
//  return os << std::hex <<
//               "[0x" << posix::Thread::getCurrentThread() << "] " << std::dec;
}
