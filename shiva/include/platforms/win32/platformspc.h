#if !defined(__SHIVA_MFC) // include this if you have an MFC project
# define WIN32_LEAN_AND_MEAN
# if defined(DEBUG) && !defined(__SHIVA_WINCE)
#  if !defined(__MINGW32__) && !defined(WINELIB)
#    define _CRTDBG_MAP_ALLOC
#    define SHVBREAKPOINT (_CrtDbgBreak(), 0)
#  else
#    define SHVBREAKPOINT false
#  endif
# endif
# include <windows.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <winnls.h>
  typedef wchar_t WCHAR;
# if defined(DEBUG) && !defined(__SHIVA_WINCE)
#  if !defined(__MINGW32__) && !defined(WINELIB)
#    include <crtdbg.h>
#  endif
# endif
#else
# if defined(DEBUG) && !defined(__SHIVA_WINCE)
#  include <crtdbg.h>
#  define SHVBREAKPOINT (_CrtDbgBreak(), 0)
# endif
# include <afxtempl.h>
# include <afxsock.h>
# include <shlwapi.h>
#endif

#if !defined(__SHVSTRINGDEFAULT) && defined(UNICODE)
# define __SHVSTRINGDEFAULT 16
#endif

#ifndef SHVVA_COPY
# define SHVVA_COPY(x,y) x = y
#endif

#ifdef LIBSHIVA_EXPORTS
# define SHVAPI __declspec(dllexport)
# pragma warning( disable: 4251 )
#elif defined(NOLIBSHIVA)
# define SHVAPI
#else
# define SHVAPI __declspec(dllimport)
# pragma warning( disable: 4251 )
#endif

#ifdef GetObject
# undef GetObject
# ifdef UNICODE
#  define GDIGetObject   GetObjectW
# else
#  define GDIGetObject   GetObjectA
# endif
#endif
