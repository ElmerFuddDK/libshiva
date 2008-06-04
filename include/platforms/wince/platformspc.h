#include "../win32/platformspc.h"

#ifdef DEBUGMSG
# undef DEBUGMSG
#endif

#define __SHIVA_WIN32

#if defined(_WIN32_WCE_PSPC) || defined(WIN32_PLATFORM_PSPC)
# define __SHIVA_POCKETPC
#endif
#if (_WIN32_WCE < 400)
# define __SHIVA_WINCE3
#else
# define __SHIVA_WINCE4
#endif

#ifndef SHVVA_COPY
# define SHVVA_COPY(x,y) x = y
#endif

#ifndef GetWindowLongPtr
#define GetWindowLongPtr  GetWindowLong
#endif

#ifndef SetWindowLongPtr
#define SetWindowLongPtr  SetWindowLong
#endif

#ifndef GWLP_WNDPROC
#define GWLP_WNDPROC GWL_WNDPROC
#endif

#ifndef GWLP_USERDATA
#define GWLP_USERDATA GWL_USERDATA
#endif
