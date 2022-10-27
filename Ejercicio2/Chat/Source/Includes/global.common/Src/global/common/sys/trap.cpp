/*
 * Feb 1, 2009 - ayachan
 */

#include "trap.h"

#include <sys/types.h>
#include <unistd.h>

#include <iostream>

Trap& getTrap(int signal)
{
  static Trap trap(signal);

  return trap;
}

Trap::Trap(int signal)
  : signal(signal), signalled(false)
{
  struct sigaction act;
  act.sa_handler = catch_signal;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  if (sigaction(signal, &act, NULL) < 0)
    std::cerr << "Failed to catch signal (" << signal << ")" << std::endl;
  else
    std::cout << "TRAPPED (by default, end with ctrl-|)" << std::endl;
}

Trap::~Trap()
{
  struct sigaction act;
  act.sa_handler = SIG_DFL;
  sigemptyset(&act.sa_mask);
  act.sa_flags = 0;
  if (sigaction(signal, &act, NULL) < 0)
    std::cerr << "Failed to uncatch signal (" << signal << ")" << std::endl;
}

void Trap::catch_signal(int signo)
{
  if (signo == ::getTrap().signal)
    ::getTrap().signalled = true;
}
