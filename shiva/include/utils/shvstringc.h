#ifndef __SHIVA_UTILS_STRINGC_H
#define __SHIVA_UTILS_STRINGC_H

#include "shvhashvalue.h"
#include "shvrefobject.h"

#ifdef __SHIVA_EPOC
# include <e32std.h>
# include <e32des16.h>
#endif

#if defined(__SHIVA_WIN32)
# define __SHVSTRING_HEAPPROTECT
#endif

#if __SHVSTRINGDEFAULT == 8
# include "shvstring8c.h"
#elif __SHVSTRINGDEFAULT == 16
# include "shvstring16c.h"
#elif __SHVSTRINGDEFAULT == utf8
# include "shvstringutf8c.h"
#else
# error "Invalid string default define"
#endif

#ifndef __SHVSTRING_EXCLUDE_UNICODE
# ifndef __SHIVA_UTILS_STRING8C_H
#  include "shvstring8c.h"
# endif
# ifndef __SHIVA_UTILS_STRING16C_H
#   include "shvstring16c.h"
# endif
# ifndef __SHIVA_UTILS_STRINGUTF8C_H
#  include "shvstringutf8c.h"
# endif
#endif

#include "shvstringc.inl"

#endif
