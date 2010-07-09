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

#include "../../../include/framework/shvconsole.h"
#ifdef __SHIVA_WINCE
# include "../../../include/frameworkimpl/shvmainthreadeventdispatcherconsole.h"
#endif


//=========================================================================================================
// SHVConsole class - console output static class
//=========================================================================================================

/*************************************
 * Printf
 *************************************/
/// Prints to stdout if it is a console application
void SHVConsole::Printf(const SHVTChar* str, ...)
{
SHVVA_LIST args;
	SHVVA_START(args,str);
#if defined(__SHIVA_WINCE)
	vwprintf(str,args);
#elif defined(UNICODE)
	vwprintf((const TCHAR*)str,args);
#else
	vprintf(str,args);
#endif
	SHVVA_END(args);
}

/*************************************
 * PrintfList
 *************************************/
/// Prints to stdout if it is a console application
void SHVConsole::PrintfList(const SHVTChar* str, SHVVA_LIST args)
{
SHVVA_LIST argList;
	SHVVA_COPY( argList, args );
#if defined(__SHIVA_WINCE)
	vwprintf(str,argList);
#elif defined(UNICODE)
	vwprintf((const TCHAR*)str,argList);
#else
	vprintf(str,argList);
#endif
	SHVVA_END( argList );
}

/*************************************
 * ErrPrintf
 *************************************/
/// Prints to stderr if it is a console application
void SHVConsole::ErrPrintf(const SHVTChar* str, ...)
{
SHVVA_LIST args;
	SHVVA_START(args,str);
#if defined(__SHIVA_WINCE)
	vfwprintf(stderr,str,args);
#elif defined(UNICODE)
	vfwprintf(stderr,(const TCHAR*)str,args);
#else
	vfprintf(stderr,str,args);
#endif
	SHVVA_END(args);
}

/*************************************
 * ErrPrintfList
 *************************************/
/// Prints to stderr if it is a console application
void SHVConsole::ErrPrintfList(const SHVTChar* str, SHVVA_LIST args)
{
SHVVA_LIST argList;
	SHVVA_COPY( argList, args );
#if defined(__SHIVA_WINCE)
	vfwprintf(stderr,str,argList);
#elif defined(UNICODE)
	vfwprintf(stderr,(const TCHAR*)str,argList);
#else
	vfprintf(stderr,str,argList);
#endif
	SHVVA_END( argList );
}

/*************************************
 * Printf8
 *************************************/
/// Prints to stdout if it is a console application
void SHVConsole::Printf8(const SHVChar* str, ...)
{
SHVVA_LIST args;
	SHVVA_START(args,str);
	vprintf(str,args);
	SHVVA_END(args);
}

/*************************************
 * PrintfList
 *************************************/
/// Prints to stdout if it is a console application
void SHVConsole::PrintfList8(const SHVChar* str, SHVVA_LIST args)
{
SHVVA_LIST argList;
	SHVVA_COPY( argList, args );
	vprintf(str,argList);
	SHVVA_END( argList );
}

/*************************************
 * ErrPrintf
 *************************************/
/// Prints to stderr if it is a console application
void SHVConsole::ErrPrintf8(const SHVChar* str, ...)
{
SHVVA_LIST args;
	SHVVA_START(args,str);
	vfprintf(stderr,str,args);
	SHVVA_END(args);
}

/*************************************
 * ErrPrintfList
 *************************************/
/// Prints to stderr if it is a console application
void SHVConsole::ErrPrintfList8(const SHVChar* str, SHVVA_LIST args)
{
SHVVA_LIST argList;
	SHVVA_COPY( argList, args );
	vfprintf(stderr,str,argList);
	SHVVA_END( argList );
}

/*************************************
 * Printf
 *************************************/
/// Prints to stdout if it is a console application
void SHVConsole::Printf16(const SHVWChar* str, ...)
{
SHVVA_LIST args;
	SHVVA_START(args,str);
#ifdef __SHIVA_LINUX
	printf("%s", SHVString16C::FormatList(str, args).ToStrT().GetSafeBuffer());
#elif defined(__SHIVA_WINCE)
	vwprintf(str,args);
#else
	vwprintf((const TCHAR*)str,args);
#endif
	SHVVA_END(args);
}

/*************************************
 * PrintfList
 *************************************/
/// Prints to stdout if it is a console application
void SHVConsole::PrintfList16(const SHVWChar* str, SHVVA_LIST args)
{
SHVVA_LIST argList;
	SHVVA_COPY( argList, args );
#ifdef __SHIVA_LINUX
	printf("%s", SHVString16C::FormatList(str, argList).ToStrT().GetSafeBuffer());
#elif defined(__SHIVA_WINCE)
	vwprintf(str,argList);
#else
	vwprintf((const TCHAR*)str,argList);
#endif
	SHVVA_END( argList );
}

/*************************************
 * ErrPrintf
 *************************************/
/// Prints to stderr if it is a console application
void SHVConsole::ErrPrintf16(const SHVWChar* str, ...)
{
SHVVA_LIST args;
	SHVVA_START(args,str);
#ifdef __SHIVA_LINUX
	fprintf(stderr, "%s", SHVString16C::FormatList(str, args).ToStrT().GetSafeBuffer());
#elif defined(__SHIVA_WINCE)
	vfwprintf(stderr,str,args);
#else
	vfwprintf(stderr,(const TCHAR*)str,args);
#endif
	SHVVA_END(args);
}

/*************************************
 * ErrPrintfList
 *************************************/
/// Prints to stderr if it is a console application
void SHVConsole::ErrPrintfList16(const SHVWChar* str, SHVVA_LIST args)
{
SHVVA_LIST argList;
	SHVVA_COPY( argList, args );
#ifdef __SHIVA_LINUX
	fprintf(stderr, "%s", SHVString16C::FormatList(str, argList).ToStrT().GetSafeBuffer());
#elif defined(__SHIVA_WINCE)
	vfwprintf(stderr,str,args);
#else
	vfwprintf(stderr,(const TCHAR*)str,args);
#endif
	SHVVA_END( argList );
}

#ifdef __SHIVA_WINCE
///\cond INTERNAL
/*************************************
 * *printf overrides for CE
 *************************************/
void SHVConsole::vwprintf(const SHVWChar* str, SHVVA_LIST args)
{
SHVVA_LIST argList;
	SHVVA_COPY( argList, args );
	SHVMainThreadEventDispatcherConsole::Print(SHVString16C::FormatList(str,argList));
	SHVVA_END( argList );
}
void SHVConsole::vprintf(const SHVChar* str, SHVVA_LIST args)
{
SHVVA_LIST argList;
	SHVVA_COPY( argList, args );
	SHVMainThreadEventDispatcherConsole::Print(SHVString8C::FormatList(str,argList).ToStr16());
	SHVVA_END( argList );
}

void SHVConsole::vfwprintf(FILE* f, const SHVWChar* str, SHVVA_LIST args)
{
SHVVA_LIST argList;
	SHVVA_COPY( argList, args );
	SHVMainThreadEventDispatcherConsole::Print(SHVString16C::FormatList(str,argList));
	SHVVA_END( argList );
}

void SHVConsole::vfprintf(FILE* f, const SHVChar* str, SHVVA_LIST args)
{
SHVVA_LIST argList;
	SHVVA_COPY( argList, args );
	SHVMainThreadEventDispatcherConsole::Print(SHVString8C::FormatList(str,argList).ToStr16());
	SHVVA_END( argList );
}

///\endcond
#endif
