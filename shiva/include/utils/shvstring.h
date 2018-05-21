#ifndef __SHIVA_UTILS_STRING_H
#define __SHIVA_UTILS_STRING_H

#include "shvstringc.h"

#if __SHVSTRINGDEFAULT == 8
# include "shvstring8.h"
#elif __SHVSTRINGDEFAULT == 16
# include "shvstring16.h"
#elif __SHVSTRINGDEFAULT == utf8
# include "shvstringutf8.h"
#else
# error "Invalid string default define"
#endif

#ifndef __SHVSTRING_EXCLUDE_UNICODE
# ifndef __SHIVA_UTILS_STRING8_H
#  include "shvstring8.h"
# endif
# ifndef __SHIVA_UTILS_STRING16_H
#   include "shvstring16.h"
# endif
# ifndef __SHIVA_UTILS_STRINGUTF8_H
#  include "shvstringutf8.h"
# endif
#endif

#endif
