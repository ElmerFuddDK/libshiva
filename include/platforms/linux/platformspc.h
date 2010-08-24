#include <unistd.h>
#include <malloc.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>

# ifdef DEBUG
#  include <signal.h>
#  define SHVBREAKPOINT (::raise(SIGTRAP))
# endif

#ifndef va_list
# include <stdarg.h>
#endif
