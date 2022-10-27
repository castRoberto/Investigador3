#ifndef _ARGUMENTPARSER_H_
#define _ARGUMENTPARSER_H_

/*
 * Jan 7, 2009 - ayachan
 */

#include <stdlib.h>
#include <string>

//#if defined(__QNXNTO__)
//#else
//#endif

/**
 * Just an utility to retrieve arguments from the command line.
 */
class ArgumentParser
{
private:
  int argc;
  char** argv;

  int next;

  int _start;
  int _mark;

public:
  ArgumentParser(int argc, char* argv[], int start = 0)
    : argc(argc), argv(argv), next(start), _start(start), _mark(start) {}

  /**
   * @return The next argument as string or the default value.
   */
  const char* nextString(const char* defaultValue = 0)
  {
    return (next < argc) ? argv[next++] : defaultValue;
  }

  /**
   * @return The next argument as int or the default value.
   */
  int nextInt(int defaultValue = 0)
  {
    return (next < argc) ? atoi(argv[next++]) : defaultValue;
  }

  /**
   * Skip the next argument.
   */
  void skip() { next++; }

  /**
   * @return TRUE if exist more arguments to read.
   */
  bool available() { return next < argc; }

  void reset() { next = _start; }

  void mark() { _mark = next; }
  void restore() { next = _mark; }

  /**
   * @return The 'argc' from the last argument read.
   */
  int getArgc() { return argc - next; }

  /**
   * @return The 'argv' from the last argument read.
   */
  char** getArgv() { return argv + next; }
};

#endif /* _ARGUMENTPARSER_H_ */
