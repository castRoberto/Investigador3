/*
 * stdlog.cpp
 *
 *  Created on: Mar 28, 2017
 *      Author: ayachan
 */

#include "stdlog.h"

#include <stdio.h>

namespace log
{

  struct LogLeveling finest(logFINEST);
  struct LogLeveling finer(logFINER);
  struct LogLeveling fine(logFINE);
  struct LogLeveling config(logCONFIG);
  struct LogLeveling info(logINFO);
  struct LogLeveling warning(logWARNING);
  struct LogLeveling severe(logSEVERE);

  inline int LogStreambuf::sync()
  {
    int result = std::stringbuf::sync();
    if (leveler->getLevel() >= leveler->getBaseLevel())
    {
      leveler->stream() << leveler->prefix() <<
//            "[" << leveler->getLevel() << "[]" << leveler->getBaseLevel() << "] " <<
          str();
      leveler->stream().flush();
    }
    else
    {
      // (nope)
//        std::cout << "nope: sentence = " << leveler->getLevel() <<
//            ", base = " << leveler->getBaseLevel() << std::endl;
    }
    reset();
    return result;
  }

  inline void LogStreambuf::reset()
  {
    str("");
    leveler->clearLevel();
  }

  /**
   * Instance a log on std::cout with base level INFO and default level FINE
   * (that means that all output without leveling, will be intercepted).
   *
  LogStream clog(std::cout, logINFO, logINFO);
   */

  LogStream& operator<<(LogStream& os, LogLeveling logLevel)
  {
    os.setLevel(logLevel.level);
    return os;
  }

  const std::string LogLevel_str[] =
  {
      "FINEST", "FINER", "FINE", "CONFIG", "INFO", "WARNING", "SEVERE", ""
  };

  const std::string LogStream::NONE_PREFIX = "";

  /**
   * Produce a header that include the log level and the current time.
   */
  std::string LogStream::prefix() const
  {
    if (!usePrefix)
      return NONE_PREFIX;

    if (lastPrefixTime.empty() || ( (clock() - lastfTime) >= CLOCKS_PER_SEC))
    {
      lastfTime = clock();

      // produce the time
      time_t rawtime;
      struct tm * timeinfo;
      char timeBuffer[80];

      time (&rawtime);
      timeinfo = localtime (&rawtime);

      strftime (timeBuffer, 80, "%H:%M:%S", timeinfo);

      lastPrefixTime = timeBuffer;
    }

    // produce the prefix
    char prefixBuffer[80];
    if (unitName.empty())
      sprintf (prefixBuffer, "%s [%s] ",
          lastPrefixTime.c_str(), LogLevel_str[getLevel()].c_str());
    else
      sprintf (prefixBuffer, "%s [%s] [%s]",
          lastPrefixTime.c_str(), LogLevel_str[getLevel()].c_str(),
          unitName.c_str());

    std::string result(prefixBuffer);
    return result;
  }
}
