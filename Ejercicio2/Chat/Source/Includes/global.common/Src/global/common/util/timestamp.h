#ifndef _TIMESTAMP_H_
#define _TIMESTAMP_H_

/*
 * Dec 30, 2008 - ayachan
 */

#include "Serializable.h"

#include <time.h>
#ifdef __MINGW32__
#include <pthread.h>
#include "time_mingw_patch.h"
#endif

#include <string>
#include <iostream>

namespace util
{

  /**
   * This class represents a time, intended to be used to mark time instances
   * and operate between them.
   * All methods are declared virtual in order to have different implementations
   * that would depend on the OS.
   */
  class timestamp : public Serializable
  {
  public:
    /**
     * Create an instance that represents the current time.
     */
    timestamp();
    /**
     * Build an instance with the given time.
     */
    timestamp(const struct timespec& ts);
    timestamp(double seconds);
    timestamp(long ms);
    /**
     * Copy constructor.
     */
    timestamp(const timestamp& rhs);
    /**
     * Assignment
     */
    timestamp& operator = (const timestamp& rhs);
    timestamp& operator = (double seconds);

    /*
     * Explicit convertions.
     */

    /**
     * Convert to milliseconds, eventually epoch's if time is absolute.
     */
    operator long() const;
    operator long long() const;
    /**
     * Convert to seconds as a double.
     */
    operator double() const;
    const struct timespec get_ts() { return ts; }

    /*
     * Boolean operations.
     */

    bool operator == (const timestamp& rhs) const;
    bool operator <  (const timestamp& rhs) const;

    bool operator != (const timestamp& rhs) const { return !(*this == rhs); }
    bool operator >  (const timestamp& rhs) const { return rhs < *this; }
    bool operator >= (const timestamp& rhs) const { return !(*this < rhs); }
    bool operator <= (const timestamp& rhs) const { return !(*this > rhs); }

    /*
     * Binary operations.
     */

    timestamp operator + (long ms);
    long operator - (const timestamp& rhs);

    timestamp operator + (const timestamp& rhs);
//    timestamp operator + (double seconds);
//    double operator - (const timestamp& rhs);

  public:
    time_t getSeconds() const { return ts.tv_sec; }
    long getNanoSeconds() const { return ts.tv_nsec; }

    /**
     * Get elapsed from this time to now, in [ms]
     */
    double elapsed_ms() const;

  protected:
    static const long THOUSAND;
    static const long BILLION;
    static const long MILLION;

    static const double D_THOUSAND;
    static const double D_BILLION;
    static const double D_MILLION;

  public:
    static void abstime(timestamp& ts) { abstime(ts.ts); }
    static void abstime(struct timespec& ts);
    static timestamp getTimestamp() { timestamp t; return t; }

    /**
     * @return A timestamp set ms milliseconds behind.
     */
    static timestamp timestampIn(long ms);

    /**
     * @return The current time zone - UTC difference, in seconds.
     * 
     * To convert from platform actual time to UTC, you must SUBSTRACT this
     * value. Remember that in Chile this value is negative (-4 or -3) while in
     * Spain is positive (+1 or +2).
     * This value is retrieved using "time", "mgtime", "mktime" and "difftime",
     * all standard methods declared in <time.h>.
     * Already tested in UBUNTU and NEUTRINO.
     */
    static double getTZDifference_s();

  public:
    int serializedLen() const { return 8; } // two longs
    void serialize(util::ByteBuffer* bb) const;
    void deserialize(util::ByteBuffer* bb);

  public:
    const std::string getTimef() const;
    static const std::string getTimef(const util::timestamp& ts);

  private:
    struct timespec ts;
  };

  double doublediff(const timestamp& lhs, const timestamp& rhs);

} // namespace

std::ostream& operator<< (std::ostream& os, const util::timestamp& ts);

#endif /* _TIMESTAMP_H_ */
