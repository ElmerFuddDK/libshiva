#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include <ctype.h>

#ifdef __SHIVA_ANDROID_STL
#include <cctype>
#include <ios>
#endif

#ifdef _S
# undef _S
#endif

# ifdef DEBUG
#  include <signal.h>
#  define SHVBREAKPOINT (::raise(SIGTRAP))
# endif

#ifndef va_list
# include <stdarg.h>
#endif
