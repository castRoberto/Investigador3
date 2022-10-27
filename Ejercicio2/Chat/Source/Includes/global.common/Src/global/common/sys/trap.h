#ifndef _TRAP_H_
#define _TRAP_H_

/*
 * Feb 1, 2009 - ayachan
 */

#include <signal.h>

/**
 * Create a trap to a given signal so a condition raise when happens.
 * Just a single signal and a single instance would be created.
 */
class Trap
{
public:
  class TrapException {};

public:
  Trap(int signal = SIGQUIT); // ctrl-|
  ~Trap();

public:
  bool raised() const { return signalled; }

private:
  int signal;
  bool signalled;
  static void catch_signal(int signo);
};

Trap& getTrap(int signal = SIGQUIT);

#endif /* _TRAP_H_ */
