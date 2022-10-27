/*
 * Dec 30, 2008 - ayachan
 */

#include "timestamp.h"
#include "timespec.h"

#include "UtilString.h"

#include <stdio.h>

//#include <math.h>
//#if defined(__QNXNTO__)
//#include <stdlib.h>
//#endif

namespace util
{

  double timestamp::getTZDifference_s()
  {
    time_t currentTime = time(0);

    struct tm* tms = gmtime(&currentTime);
    time_t utcTime = mktime(tms);

    return difftime(currentTime, utcTime);
  }

  const long timestamp::THOUSAND = 1000L;
  const long timestamp::BILLION = 1000000000L;
  const long timestamp::MILLION = 1000000L;

  const double timestamp::D_THOUSAND = 1000.0;
  const double timestamp::D_BILLION = 1000000000.0;
  const double timestamp::D_MILLION = 1000000.0;

  void timestamp::abstime(struct timespec& ts)
  {
#ifdef __MINGW32__
    struct timeval tv = { 0 };
    gettimeofday(&tv, 0);
    ts.tv_sec = tv.tv_sec;
    ts.tv_nsec = tv.tv_usec * 1000;
#else
    if (clock_gettime(CLOCK_REALTIME, &ts) < 0)
      perror("[timestamp::abstime]: can't get time");
#endif
  }

  timestamp::timestamp()
  {
    abstime(ts);
  }

  timestamp::timestamp(const struct timespec& ts)
  {
    this->ts.tv_sec = ts.tv_sec;
    this->ts.tv_nsec = ts.tv_nsec;
  }

  /*
   *
   *
   */
  timestamp::timestamp(double seconds)
  {
    int factor = (seconds < 0) ? -1 : 1;
    double positive = (seconds < 0) ? -seconds : seconds;

    ts.tv_sec = (time_t)positive;
    ts.tv_nsec = (long)( (positive - ts.tv_sec) * D_BILLION);
    ts.tv_sec = factor * ts.tv_sec;
  }

  timestamp::timestamp(long ms)
  {
    ts.tv_sec = (time_t)(1.0 * ms / D_THOUSAND);
    ts.tv_nsec = (long)(1.0 * (ms - ts.tv_sec * D_THOUSAND) * D_MILLION);
  }

  timestamp::timestamp(const timestamp& rhs)
  {
    ts.tv_sec = rhs.ts.tv_sec;
    ts.tv_nsec = rhs.ts.tv_nsec;
  }

  timestamp timestamp::timestampIn(long ms)
  {
    timestamp ts;
    return ts + ms;
  }


  timestamp& timestamp::operator = (const timestamp& rhs)
  {
    if (this == &rhs) return *this;

    ts.tv_sec = rhs.ts.tv_sec;
    ts.tv_nsec = rhs.ts.tv_nsec;
    return *this;
  }

  timestamp& timestamp::operator = (double seconds)
  {
    ts.tv_sec = (long)seconds;
    ts.tv_nsec = (long)(1.0 * (seconds - ts.tv_sec) * D_BILLION);
    return *this;
  }

  timestamp::operator long() const
  {
    time_t positive = (ts.tv_sec < 0) ? -ts.tv_sec : ts.tv_sec;
    time_t absvalue =
      positive * THOUSAND + (long)(ts.tv_nsec / D_MILLION + 0.5);
    return (ts.tv_sec < 0) ? -absvalue : absvalue;
  }

  timestamp::operator long long() const
  {
    time_t positive = (ts.tv_sec < 0) ? -ts.tv_sec : ts.tv_sec;
    double dvalue =
        (1.0 * positive * THOUSAND + 1.0 * ts.tv_nsec / D_MILLION + 0.5);
//    std::cout << "... dvalue: " << dvalue << std::endl;
    return (ts.tv_sec < 0) ? -dvalue : dvalue;
//    long long absvalue = (long long)
//      (1.0 * positive * THOUSAND + ts.tv_nsec / D_MILLION + 0.5);
//    return (ts.tv_sec < 0) ? -absvalue : absvalue;
  }

  /*
   *
   *
   */
  timestamp::operator double() const
  {
    time_t positive = (ts.tv_sec < 0) ? -ts.tv_sec : ts.tv_sec;
    double absvalue = 1.0 * positive + 1.0 * ts.tv_nsec / D_BILLION;
    return (ts.tv_sec < 0) ? -absvalue : absvalue;
  }

  bool timestamp::operator == (const timestamp& rhs) const
  {
    return (ts.tv_sec == rhs.ts.tv_sec) && (ts.tv_nsec == rhs.ts.tv_nsec);
  }

  bool timestamp::operator <  (const timestamp& rhs) const
  {
    return (ts.tv_sec < rhs.ts.tv_sec) ||
           ( (ts.tv_sec == rhs.ts.tv_sec) && (ts.tv_nsec < rhs.ts.tv_nsec));
  }

  /*
   *
  timestamp timestamp::operator + (double seconds)
  {
    long isecs = (long)seconds;
    double fsecs = seconds - isecs;
    ts.tv_sec += isecs;
    ts.tv_nsec += (long)(fsecs * D_BILLION + 0.5);
    while (ts.tv_nsec > BILLION)
    {
      ts.tv_sec++;
      ts.tv_nsec -= BILLION;
    }
    return *this;
  }
   *
   */

  timestamp timestamp::operator + (long ms)
  {
    ts = ts + ms * THOUSAND;
    return *this;
  }
  long timestamp::operator - (const timestamp& rhs)
  {
    double value_s = (double)*this;
    double xvalue_s = (double)rhs;
    timestamp xts(value_s - xvalue_s);

    return (long)xts;
  }

  timestamp timestamp::operator + (const timestamp& rhs)
  {
    ts.tv_sec += rhs.ts.tv_sec;
    ts.tv_nsec += rhs.ts.tv_nsec;
    while (ts.tv_nsec > BILLION)
    {
      ts.tv_sec++;
      ts.tv_nsec -= BILLION;
    }
    return *this;
  }

  /*
   *
  double timestamp::operator - (const timestamp& rhs)
  {
    double dlhs = double(*this);
    double drhs = double(rhs);

    return dlhs - drhs;
  }
   *
   */

  const std::string timestamp::getTimef() const
  {
    return getTimef(*this);
  }
  const std::string timestamp::getTimef(const util::timestamp& ts)
  {
    static const std::string badTime = "--:--:--.------";

//    double seconds = util::timestamp::getTimestamp();
    double seconds = ts;

    if (seconds <= 0)
      return badTime;

    int minutes = (int)(seconds / 60.0);
    seconds -= (minutes * 60);
    int hours = (int)(1.0 * minutes / 60.0);
    minutes -= (hours * 60);
    hours = hours % 24;
  
    int iseconds = (int)seconds;
    seconds -= iseconds;
    int decimals = (int)(seconds * 1000000.0 + 0.5);

    return UtilString::format("%2.2d:%2.2d:%2.2d.%6.6d",
               hours, minutes, iseconds, decimals);
  }

  void timestamp::serialize(util::ByteBuffer* bb) const
  {
    bb->putLong(ts.tv_sec);
    bb->putLong(ts.tv_nsec);
  }

  void timestamp::deserialize(util::ByteBuffer* bb)
  {
    ts.tv_sec = bb->getLong();
    ts.tv_nsec = bb->getLong();
  }

  double timestamp::elapsed_ms() const
  {
    struct timespec tsnow;
    abstime(tsnow);

    double now_s = 1.0 * tsnow.tv_sec + tsnow.tv_nsec / D_BILLION;
    double then_s = 1.0 * ts.tv_sec + ts.tv_nsec / D_BILLION;
    return (now_s - then_s) * D_THOUSAND;
//
//    timestamp now;
//    return doublediff(now, *this) * 1000.0;
  }

  /**
   * Return the difference in seconds as a double.
   */
  double doublediff(const timestamp& lhs, const timestamp& rhs)
  {
//    std::cout << " double dif: " << lhs << " vs " << rhs << std::endl;
    double dlhs = double(lhs);
    double drhs = double(rhs);

    return dlhs - drhs;
  }

} // namespace

std::ostream& operator<< (std::ostream& os, const util::timestamp& ts)
{
//    return os << '(' << (long)ts << ')';
  return os << '(' << ts.getSeconds() << "," << ts.getNanoSeconds() << ')' <<
               ", " << (double)ts;
}
