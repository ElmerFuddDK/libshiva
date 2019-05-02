#ifndef __SHIVA_UTILS_STRINGC_H
#define __SHIVA_UTILS_STRINGC_H

#include "shvhashvalue.h"
#include "shvrefobject.h"

#ifdef __SHIVA_EPOC
# include <e32std.h>
# include <e32des16.h>
#endif

#include "shvstring8c.h"

#ifndef __SHVSTRING_EXCLUDE_UNICODE
# include "shvstring16c.h"
# include "shvstringutf8c.h"
#endif

#include "shvstringc.inl"

#endif
