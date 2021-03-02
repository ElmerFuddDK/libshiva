/*
 *   Copyright (C) 2008 by Lars Eriksen
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of the
 *   License, or (at your option) any later version with the following
 *   exeptions:
 *
 *   1) Static linking to the library does not constitute derivative work
 *      and does not require the author to provide source code for the
 *      application.
 *      Compiling applications with the source code directly linked in is
 *      Considered static linking as well.
 *
 *   2) You do not have to provide a copy of the license with programs
 *      that are linked against this code.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../../../include/utils/shvassert.h"
#include "../../../include/utils/shvstring.h"
#include "../../../include/framework/shvconsole.h"

#if defined(__SHIVA_WIN32) && !defined(__SHIVA_WINCE) && !defined(__MINGW32__)
# include <crtdbg.h>
# if defined(_MSC_VER) && defined(DEBUG) && defined(SHIVASTATICMODULELIB)
#  pragma comment(lib, "user32.lib")
# endif
#elif !defined(__SHIVA_WINCE) && !defined(__MINGW32__)
# include <signal.h>
#endif
#ifdef ANDROID
#include <android/log.h>
#endif

#undef HASGLIBCBACKTRACE
#if defined(__GLIBC__)
# if __GLIBC_PREREQ(2, 10)
#  define HASGLIBCBACKTRACE
#  include "../../../include/threadutils/shvthreadbase.h"
#  include "../../../include/utils/shvtime.h"
#  include <stdlib.h>
#  include <execinfo.h>
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <fcntl.h>
#  include <unistd.h>
# endif
#endif

/// \class SHVAssert shvassert.h "shiva/include/utils/shvassert.h"

bool SHVAPI SHVAssert::ReportError(const char* fileName, int lineNo)
{
#if defined(__SHIVA_WINCE)
	///\todo Implement assertions for wince
	return true;
#elif defined(__SHIVA_WIN32) && !defined(__MINGW32__)
# ifdef DEBUG
char* brkEnv = ::getenv("SHVBREAKONASSERT");
	if (brkEnv)
	{
		SHVConsole::ErrPrintf8("ASSERTION FAILED IN %s, LINE %d\n", fileName, lineNo);
		fflush(stderr);
		return (::strcmp(brkEnv, "0") ? true : false);
	}
	else
	{
		// we remove WM_QUIT because if it is in the queue then the message box
		// won't display
		MSG msg;
		BOOL bQuit = PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
		BOOL bResult = _CrtDbgReport(_CRT_ASSERT, fileName, lineNo, NULL, NULL);

		if (bQuit)
			PostQuitMessage((int)msg.wParam);
		return (bResult ? true : false);
	}
# endif
#else
	SHVConsole::ErrPrintf8("ASSERTION FAILED IN %s, LINE %d\n",fileName,lineNo);
# ifdef __MINGW32__
	fflush(stderr);
# else
	if (::getenv("SHVBREAKONASSERT"))
	{
		raise(SIGINT);
	}
# endif
	return false;
#endif
	return true;
}

void SHVAPI SHVAssert::Trace(const SHVTChar* s, ...)
{
SHVString str;
SHVVA_LIST args;

	SHVVA_START(args,s);
	str.FormatList(s,args);
	SHVVA_END(args);

#if defined(__SHIVA_WIN32) && !defined(__MINGW32__)
# if __SHVSTRINGDEFAULT == 8
	::OutputDebugStringA(str.GetSafeBuffer());
# else
	::OutputDebugStringW(str.AsStr16C().GetSafeBufferWin32());
# endif
#elif defined(__SHIVA_EPOC)
# ifdef __MINGW32__
	if (str.Find(_S("\n")) >= 0)
		fflush(stderr);
# endif
#elif defined (ANDROID)
	__android_log_print(ANDROID_LOG_DEBUG,"Shiva", "%s", str.GetSafeBuffer());
#else
	SHVConsole::ErrPrintf(_S("%s"),str.GetSafeBuffer());
#endif
}

void SHVAssert::DumpStackTrace(const char* dir, const char* basename, const void* ptr, bool append, const char* xtrainfo)
{
#if defined(HASGLIBCBACKTRACE)
char fname[512];
void *array[10];
size_t size;
int fd;

	if (::snprintf(fname,sizeof(fname),"%s/%s_%zd_%zd.txt",dir,basename,(ssize_t)ptr,SHVThreadBase::GetCurrentThreadID()) >= (int)sizeof(fname))
		return;

	fd = (append ? ::open(fname,O_WRONLY|O_CREAT|O_APPEND,00666) : ::creat(fname,00666));
	if (fd)
	{
	SHVString tStr(SHVTime::CreateDateStringNow());
		::write(fd,tStr.GetSafeBuffer(),tStr.GetLength());
		::write(fd,"\n",1);
		if (xtrainfo)
			::write(fd,xtrainfo,SHVString8C::StrLenInBytes(xtrainfo));
		size = ::backtrace(array, 10);
		if (size > 1)
		::backtrace_symbols_fd(array+1, size-1, fd);
		::close(fd);
	}
#else
	SHVUNUSED_PARAM(dir);
	SHVUNUSED_PARAM(basename);
	SHVUNUSED_PARAM(ptr);
	SHVUNUSED_PARAM(append);
	SHVUNUSED_PARAM(xtrainfo);
#endif
}

void SHVAssert::ClearStackTrace(const char* dir, const char* basename, const void* ptr)
{
#if defined(HASGLIBCBACKTRACE)
char fname[512];

	if (::snprintf(fname,sizeof(fname),"%s/%s_%zd_%zd.txt",dir,basename,(ssize_t)ptr,SHVThreadBase::GetCurrentThreadID()) >= (int)sizeof(fname))
		return;
	
	::remove(fname);

#else
	SHVUNUSED_PARAM(dir);
	SHVUNUSED_PARAM(basename);
	SHVUNUSED_PARAM(ptr);
#endif
}
