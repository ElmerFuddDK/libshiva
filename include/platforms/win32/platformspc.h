#if !defined(__SHIVA_MFC) // include this if you have an MFC project
# ifdef DEBUG
#  include <crtdbg.h>
#  define BREAKPOINT (_CrtDbgBreak(), 0)
# endif
# define WIN32_LEAN_AND_MEAN
# include <windows.h>
# include <stdio.h>
# include <stdlib.h>
# include <string.h>
# include <winnls.h>
  typedef wchar_t WCHAR;
#else
# include <afxtempl.h>
# include <afxsock.h>
# include <shlwapi.h>
#endif

#ifndef SHVVA_COPY
# define SHVVA_COPY(x,y) x = y
#endif

#ifdef LIBSHIVA_EXPORTS
# define SHVAPI __declspec(dllexport)
#else
# define SHVAPI __declspec(dllimport)
#endif
