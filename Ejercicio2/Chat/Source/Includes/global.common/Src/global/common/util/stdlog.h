/*
 * stdlog.h
 *
 *  Created on: Mar 28, 2017
 *      Author: ayachan
 */

#ifndef _STDLOG_H_
#define _STDLOG_H_

#include <time.h>

#include <iostream>
#include <sstream>

namespace log
{

  enum LogLevel
  {
    logFINEST,
    logFINER,
    logFINE,
    logCONFIG,
    logINFO,
    logWARNING,
    logSEVERE
  };

  struct LevelProvider
  {
    virtual ~LevelProvider() {}

    virtual LogLevel getLevel() const = 0;
    virtual LogLevel getBaseLevel() const = 0;
    virtual void clearLevel() = 0;

    virtual std::ostream& stream() const = 0;

    virtual std::string prefix() const = 0;
  };

  struct LogStreambuf : public std::stringbuf
  {
    LogStreambuf(LevelProvider* leveler)
        : std::stringbuf(), leveler(leveler)
    {}
    virtual ~LogStreambuf() {}

  protected:
    /**
     * Overload streambuf::sync in order to intercept output when flush.
     */
    virtual int sync();

    void reset();

  private:
    LevelProvider* leveler;
  };

  /**
   * Stream that filter output
   */
  struct LogStream: public std::ostream, public LevelProvider
  {
    LogStream(
        std::ostream& logStream = std::cout,
        LogLevel baseLevel = logINFO, LogLevel defaultLogLevel = logINFO,
        bool usePrefix = true, const std::string& unitName = "")
        : std::ostream(),
          logStream(logStream), strbuf(this),
          baseLevel(baseLevel), defaultLevel(defaultLogLevel),
          sentenceLevel(defaultLogLevel),
          usePrefix(usePrefix), unitName(unitName),
          lastfTime(0), lastPrefixTime()
    {
      previous = rdbuf(&strbuf);
    }
    ~LogStream()
    {
      rdbuf(previous);
    }

    std::ostream& level(std::ostream& os, const LogLevel& level)
    {
      setLevel(level);
      return os;
    }

    virtual LogLevel getLevel() const
    { return sentenceLevel; }
    virtual void clearLevel()
    { this->sentenceLevel = defaultLevel; }
    virtual LogLevel getBaseLevel() const
    { return baseLevel; }
    virtual std::ostream& stream() const
    { return logStream; }
    virtual std::string prefix() const;

    void setLevel(LogLevel level)
    { this->sentenceLevel = level; }
    void setDefaultLevel(LogLevel level)
    { this->defaultLevel = level; }
    void setBaseLevel(LogLevel level)
    { this->baseLevel = level; }
    void setUsePrefix(bool usePrefix)
    { this->usePrefix = usePrefix; }

  protected:
    std::ostream& logStream;

    LogStreambuf strbuf;
    std::streambuf* previous;

    LogLevel baseLevel;
    LogLevel defaultLevel;
    LogLevel sentenceLevel;

    bool usePrefix;
    std::string unitName;

    // (following two are modified when requesting the prefix)
    mutable clock_t lastfTime;
    mutable std::string lastPrefixTime;

  private:
    static const std::string NONE_PREFIX;
  };

  struct LogLeveling
  {
    LogLeveling(const LogLevel& level): level(level) {}
    LogLevel level;
  };

  LogStream& operator<<(LogStream& os, LogLeveling logLevel);

  /*
   * To use as manipulators.
   */
  extern struct LogLeveling finest;
  extern struct LogLeveling finer;
  extern struct LogLeveling fine;
  extern struct LogLeveling config;
  extern struct LogLeveling info;
  extern struct LogLeveling warning;
  extern struct LogLeveling severe;

  /**
   * The default log.
   * See initialization parameters.
   *
  extern LogStream clog;
   */

}

#endif /* _STDLOG_H_ */
