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
#elif !defined(__SHIVA_WINCE) && !defined(__MINGW32__)
# include <signal.h>
#endif
#ifdef ANDROID
#include <android/log.h>
#endif

/// \class SHVAssert shvassert.h "shiva/include/utils/shvassert.h"

bool SHVAPI SHVAssert::ReportError(const char* fileName, int lineNo)
{
#if defined(__SHIVA_WINCE)
	///\todo Implement assertions for wince
	return true;
#elif defined(__SHIVA_WIN32) && !defined(__MINGW32__)
# ifdef DEBUG
	// we remove WM_QUIT because if it is in the queue then the message box
	// won't display
	MSG msg;
	BOOL bQuit = PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
	BOOL bResult = _CrtDbgReport(_CRT_ASSERT, fileName, lineNo, NULL, NULL);

	if (bQuit)
		PostQuitMessage((int)msg.wParam);
	return (bResult ? true : false);
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
	::OutputDebugString((const TCHAR*)str.GetSafeBuffer());
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
