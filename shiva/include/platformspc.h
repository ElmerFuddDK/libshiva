// this is the main header file for SHIVA
// it is needed in all source files

#ifndef __SHIVA_PLATFORMSPC
#define __SHIVA_PLATFORMSPC

#undef __SHIVA_WIN32
#undef __SHIVA_WINCE
#undef __SHIVA_POSIX
#undef __SHIVA_POSIX_LINUX
#undef __SHIVA_POSIX_FREEBSD
#undef __SHIVA_POSIX_OSX
#undef __SHIVA_POSIX_IPHONEOS
#undef __SHIVA_POSIX_IPHONEOS_SIMULATOR
#undef __SHIVA_EPOC

#undef SHVVA_LIST

#ifndef NULL
# define NULL 0
#endif

#if defined(_DEBUG) && !defined(DEBUG)
# define DEBUG
#endif

#if defined(SHIVA_QT_PREINCLUDE) && defined(__cplusplus)
# include <QtCore/qlist.h>
#endif


// include main platform headers

#ifdef _WIN32_WCE
# define __SHIVA_WIN32
# define __SHIVA_WINCE
# include "shvconfig.h"
# include "platforms/wince/platformspc.h"
#elif defined(_WIN32) && !defined(__EPOC32__) && !defined(__SYMBIAN32__)
# define __SHIVA_WIN32
# include "shvconfig.h"
# include "platforms/win32/platformspc.h"
#elif defined(__EPOC32__) || defined(__SYMBIAN32__)
# define __SHIVA_EPOC
# include "shvconfig.h"
# include "platforms/symbian/platformspc.h"
#elif defined(__FreeBSD__)
# define __SHIVA_POSIX
# define __SHIVA_POSIX_FREEBSD
# include "shvconfig.h"
# include "platforms/freebsd/platformspc.h"
#elif defined(__APPLE__)
# define __SHIVA_POSIX
# define __SHIVA_POSIX_OSX
# include "shvconfig.h"
# include "platforms/osx/platformspc.h"
#elif defined(linux) || defined(__linux)
# define __SHIVA_LINUX // for historic reasons
# define __SHIVA_POSIX
# define __SHIVA_POSIX_LINUX
# include "shvconfig.h"
# include "platforms/linux/platformspc.h"
#else
# define __SHIVA_POSIX
# include "shvconfig.h"
# include "platforms/posix/platformspc.h"
#endif

// Check for string default
#if !defined(__SHVSTRINGDEFAULT)
# define __SHVSTRINGDEFAULT 8
#endif

#ifndef SHVVA_LIST
# define SHVVA_LIST  va_list
#endif
#ifndef SHVVA_START
# define SHVVA_START(x,y) va_start(x,y)
#endif
#ifndef SHVVA_COPY
# define SHVVA_COPY(x,y)  va_copy(x,y)
#endif
#ifndef SHVVA_END
# define SHVVA_END(x)     va_end(x)
#endif
#ifndef SHVVA_ARG
# define SHVVA_ARG(x,y)     va_arg(x,y)
#endif

#ifndef SIZE_T_MAX
# define SIZE_T_MAX ((size_t)0-(size_t)1)
#endif

#ifndef SHVAPI
# define SHVAPI
#endif

#define SHV_DISABLE_COPY_AND_ASSIGNMENT(className) private: \
  inline className(const className& ) {} \
  inline className& operator=(const className& ) {return *this;}

#ifndef SHVUNUSED_PARAM
# define SHVUNUSED_PARAM(x) (void) x
#endif

#include "shvtypes.h"
#include "utils/shvassert.h"

#endif
