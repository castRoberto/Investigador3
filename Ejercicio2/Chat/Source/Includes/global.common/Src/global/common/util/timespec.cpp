
#include "timespec.h"

#include <stdio.h>

/* 2008.10.31 aya
 * Solucion algo mula para poder obtener un timespec absoluto
 */
struct timespec abstime()
{
  struct timespec ts;
#ifdef __MINGW32__
  struct timeval tv = { 0 };
  gettimeofday(&tv, 0);
  ts.tv_sec = tv.tv_sec;
  ts.tv_nsec = tv.tv_usec * 1000;
#else
  if (clock_gettime(CLOCK_REALTIME, &ts) < 0)
    perror("[timespec::absTime]: can't get time");
#endif
  return ts;
}

struct timespec operator+ (struct timespec ts1, struct timespec ts2)
{
  const long BILLION = 1000000000;

  struct timespec tsr;
  tsr.tv_sec = ts1.tv_sec + ts2.tv_sec;
  tsr.tv_nsec = ts1.tv_nsec + ts2.tv_nsec;
  while (tsr.tv_nsec > BILLION)
  {
    tsr.tv_sec++;
    tsr.tv_nsec -= BILLION;
  }
  return tsr;
}

struct timespec operator+ (struct timespec ts, long useconds)
{
  const long MILLION = 1000000;
  const long BILLION = 1000000000;

  const double D_MILLION = 1000000.0;

  struct timespec tsr;
  int seconds = (int) (useconds / D_MILLION);
  useconds = useconds % MILLION;
  tsr.tv_sec = ts.tv_sec + seconds;
  tsr.tv_nsec = ts.tv_nsec + useconds * 1000;
  while (tsr.tv_nsec > BILLION)
  {
    tsr.tv_sec++;
    tsr.tv_nsec -= BILLION;
  }
  return tsr;
}

long operator- (struct timespec ts1, struct timespec ts2)
{
//  long ts1_us = timespec_to_us(ts1);
//  long ts2_us = timespec_to_us(ts2);
//  return ts1_us - ts2_us;

  double ts1_s = timespec_to_s(ts1);
  double ts2_s = timespec_to_s(ts2);
  return (long) ( (ts1_s - ts2_s) * 1000000.0);
}

bool operator== (struct timespec ts1, struct timespec ts2)
{
  return ( (ts1.tv_sec == ts2.tv_sec) && (ts1.tv_nsec == ts2.tv_nsec));
}

bool operator< (struct timespec ts1, struct timespec ts2)
{
  return ( (ts2 - ts1) > 0);
}

bool operator> (struct timespec ts1, struct timespec ts2)
{
  return ( (ts1 - ts2) > 0);
}

struct timespec us_to_timespec(long elapse_us)
{
  const long MILLION = 1000000;
  const double D_MILLION = 1000000.0;

  struct timespec ts;
  ts.tv_sec = (long) (elapse_us / D_MILLION);
  elapse_us = elapse_us % MILLION;
  ts.tv_nsec = elapse_us * 1000;
  return ts;
}

long timespec_to_us(struct timespec ts)
{
  const long MILLION = 1000000;
//  const double D_BILLION = 1000000000.0;
  long value_us = (long) (1.0 * (ts.tv_sec * MILLION + ts.tv_nsec/1000.0));
  std::cout << "timespec_to_us de " << ts << " es " << value_us << std::endl;

  return value_us;
}

double timespec_to_s(struct timespec ts)
{
  const double D_BILLION = 1000000000.0;
  double value_s = 1.0 * ts.tv_sec + ts.tv_nsec / D_BILLION;
//  std::cout << "timespec_to_s de " << ts << " es " << value_s << std::endl;

  return value_s;
}

std::ostream& operator<< (std::ostream& os, const struct timespec& ts)
{
  return os << '(' << ts.tv_sec << ',' << ts.tv_nsec << ')';
}
