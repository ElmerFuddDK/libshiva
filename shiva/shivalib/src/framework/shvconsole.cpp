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
#ifdef __SHIVA_WIN32
# include "../../../include/utils/shvstring.h"
#endif
#ifdef __SHIVA_WINCE
# ifndef __SHIVAWINCE_EXCLUDE_CONSOLE_SUPPORT
#  include "../../../include/frameworkimpl/shvmainthreadeventdispatcherconsole.h"
# endif
#endif


//=========================================================================================================
// SHVConsole class - console output static class
//=========================================================================================================
/// \class SHVConsole shvconsole.h "shiva/include/framework/shvconsole.h"

/*************************************
 * Printf
 *************************************/
/// Prints to stdout if it is a console application
void SHVConsole::Printf(const SHVTChar* str, ...)
{
SHVVA_LIST args;
	SHVVA_START(args,str);
#if __SHVSTRINGDEFAULT == 16
	SHVConsole::PrintfList16(str, args);
#elif __SHVSTRINGDEFAULT == utf8
	SHVConsole::PrintfListUTF8(str, args);
#else
	SHVConsole::PrintfList8(str, args);
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
#if __SHVSTRINGDEFAULT == 16
	SHVConsole::PrintfList16(str, argList);
#elif __SHVSTRINGDEFAULT == utf8
	SHVConsole::PrintfListUTF8(str, argList);
#else
	SHVConsole::PrintfList8(str, argList);
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
#if __SHVSTRINGDEFAULT == 16
	SHVConsole::ErrPrintfList16(str, args);
#elif __SHVSTRINGDEFAULT == utf8
	SHVConsole::ErrPrintfListUTF8(str, args);
#else
	SHVConsole::ErrPrintfList8(str, args);
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
#if __SHVSTRINGDEFAULT == 16
	SHVConsole::ErrPrintfList16(str, argList);
#elif __SHVSTRINGDEFAULT == utf8
	SHVConsole::ErrPrintfListUTF8(str, argList);
#else
	SHVConsole::ErrPrintfList8(str, argList);
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
#if defined(__SHIVA_WIN32) || defined(__SHIVA_EPOC)
	wprintf(L"%s", SHVString8C::FormatList(str, args).ToStr16().GetSafeBuffer());
#else
	if (SHVConsole::NativeEncodingIsUTF8())
		printf("%s", SHVString8C::FormatList(str, args).ToStrUTF8().GetSafeBuffer());
	else
		vprintf(str,args);
#endif
	SHVVA_END(args);
}

/*************************************
 * PrintfList8
 *************************************/
/// Prints to stdout if it is a console application
void SHVConsole::PrintfList8(const SHVChar* str, SHVVA_LIST args)
{
SHVVA_LIST argList;
	SHVVA_COPY( argList, args );
#if defined(__SHIVA_WIN32) || defined(__SHIVA_EPOC)
	wprintf(L"%s", SHVString8C::FormatList(str, argList).ToStr16().GetSafeBuffer());
#else
	if (SHVConsole::NativeEncodingIsUTF8())
		printf("%s", SHVString8C::FormatList(str, argList).ToStrUTF8().GetSafeBuffer());
	else
		vprintf(str,argList);
#endif
	SHVVA_END( argList );
}

/*************************************
 * ErrPrintf8
 *************************************/
/// Prints to stderr if it is a console application
void SHVConsole::ErrPrintf8(const SHVChar* str, ...)
{
SHVVA_LIST args;
	SHVVA_START(args,str);
#if defined(__SHIVA_WIN32) || defined(__SHIVA_EPOC)
	fwprintf(stderr, L"%s", SHVString8C::FormatList(str, args).ToStr16().GetSafeBuffer());
#else
	if (SHVConsole::NativeEncodingIsUTF8())
		fprintf(stderr, "%s", SHVString8C::FormatList(str, args).ToStrUTF8().GetSafeBuffer());
	else
		vfprintf(stderr,str,args);
#endif
	SHVVA_END(args);
}

/*************************************
 * ErrPrintfList8
 *************************************/
/// Prints to stderr if it is a console application
void SHVConsole::ErrPrintfList8(const SHVChar* str, SHVVA_LIST args)
{
SHVVA_LIST argList;
	SHVVA_COPY( argList, args );
#if defined(__SHIVA_WIN32) || defined(__SHIVA_EPOC)
	fwprintf(stderr, L"%s", SHVString8C::FormatList(str, argList).ToStr16().GetSafeBuffer());
#else
	if (SHVConsole::NativeEncodingIsUTF8())
		fprintf(stderr, "%s", SHVString8C::FormatList(str, argList).ToStrUTF8().GetSafeBuffer());
	else
		vfprintf(stderr,str,argList);
#endif
	SHVVA_END( argList );
}

/*************************************
 * PrintfUTF8
 *************************************/
void SHVConsole::PrintfUTF8(const SHVChar* str, ...)
{
SHVVA_LIST args;
	SHVVA_START(args,str);
#if defined(__SHIVA_WIN32) || defined(__SHIVA_EPOC)
	wprintf(L"%s", SHVStringUTF8C::FormatList(str, args).ToStr16().GetSafeBuffer());
#else
	if (!SHVConsole::NativeEncodingIsUTF8())
		printf("%s", SHVStringUTF8C::FormatList(str, args).ToStr8().GetSafeBuffer());
	else
		vprintf(str,args);
#endif
	SHVVA_END(args);
}

/*************************************
 * PrintfListUTF8
 *************************************/
void SHVConsole::PrintfListUTF8(const SHVChar* str, SHVVA_LIST args)
{
SHVVA_LIST argList;
	SHVVA_COPY( argList, args );
#if defined(__SHIVA_WIN32) || defined(__SHIVA_EPOC)
	wprintf(L"%s", SHVStringUTF8C::FormatList(str, argList).ToStr16().GetSafeBuffer());
#else
	if (!SHVConsole::NativeEncodingIsUTF8())
		printf("%s", SHVStringUTF8C::FormatList(str, argList).ToStr8().GetSafeBuffer());
	else
		vprintf(str,argList);
#endif
	SHVVA_END( argList );
}

/*************************************
 * ErrPrintfUTF8
 *************************************/
void SHVConsole::ErrPrintfUTF8(const SHVChar* str, ...)
{
SHVVA_LIST args;
	SHVVA_START(args,str);
#if defined(__SHIVA_WIN32) || defined(__SHIVA_EPOC)
	fwprintf(stderr, L"%s", SHVStringUTF8C::FormatList(str, args).ToStr16().GetSafeBuffer());
#else
	if (!SHVConsole::NativeEncodingIsUTF8())
		fprintf(stderr, "%s", SHVStringUTF8C::FormatList(str, args).ToStr8().GetSafeBuffer());
	else
		vfprintf(stderr,str,args);
#endif
	SHVVA_END(args);
}

/*************************************
 * ErrPrintfListUTF8
 *************************************/
void SHVConsole::ErrPrintfListUTF8(const SHVChar* str, SHVVA_LIST args)
{
SHVVA_LIST argList;
	SHVVA_COPY( argList, args );
#if defined(__SHIVA_WIN32) || defined(__SHIVA_EPOC)
	fwprintf(stderr, L"%s", SHVStringUTF8C::FormatList(str, argList).ToStr16().GetSafeBuffer());
#else
	if (!SHVConsole::NativeEncodingIsUTF8())
		fprintf(stderr, "%s", SHVStringUTF8C::FormatList(str, argList).ToStr8().GetSafeBuffer());
	else
		vfprintf(stderr,str,argList);
#endif
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
#ifdef __SHIVA_POSIX
	printf("%s", SHVString16C::FormatList(str, args).ToStrT().GetSafeBuffer());
#else
	vwprintf((const WCHAR*)str,args);
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
#ifdef __SHIVA_POSIX
	printf("%s", SHVString16C::FormatList(str, argList).ToStrT().GetSafeBuffer());
#else
	vwprintf((const WCHAR*)str,argList);
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
#ifdef __SHIVA_POSIX
	fprintf(stderr, "%s", SHVString16C::FormatList(str, args).ToStrT().GetSafeBuffer());
#else
	vfwprintf(stderr,(const WCHAR*)str,args);
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
#ifdef __SHIVA_POSIX
	fprintf(stderr, "%s", SHVString16C::FormatList(str, argList).ToStrT().GetSafeBuffer());
#else
	vfwprintf(stderr,(const WCHAR*)str,args);
#endif
	SHVVA_END( argList );
}

/*************************************
 * NativeEncodingIsUTF8
 *************************************/
/// Returns true if native encoding is UTF8
bool SHVConsole::NativeEncodingIsUTF8()
{
#ifdef __SHIVA_POSIX
static int NativeEncodingDetected;

	if (NativeEncodingDetected == 0)
	{
	SHVString8C charset(getenv("LC_CTYPE"));
		if (charset.IsEmpty())
		{
			charset = getenv("LANG");
		}
		if (charset.Right(5).CompareNoCase("UTF-8") == 0 
		    || charset.Right(4).CompareNoCase("utf8") == 0)
		{
			NativeEncodingDetected = 2;
		}
		else
		{
			NativeEncodingDetected = 1;
		}
	}
	
	return NativeEncodingDetected == 2;
#else
	return false;
#endif
}

#ifdef __SHIVA_WINCE
///\cond INTERNAL
/*************************************
 * *printf overrides for CE
 *************************************/
void SHVConsole::vwprintf(const WCHAR* str, SHVVA_LIST args)
{
SHVVA_LIST argList;
	SHVVA_COPY( argList, args );
# ifndef __SHIVAWINCE_EXCLUDE_CONSOLE_SUPPORT
	SHVMainThreadEventDispatcherConsole::Print(SHVString16C::FormatList((const SHVWChar*)str,argList));
# endif
	SHVVA_END( argList );
}
void SHVConsole::vprintf(const SHVChar* str, SHVVA_LIST args)
{
SHVVA_LIST argList;
	SHVVA_COPY( argList, args );
# ifndef __SHIVAWINCE_EXCLUDE_CONSOLE_SUPPORT
	SHVMainThreadEventDispatcherConsole::Print(SHVString8C::FormatList(str,argList).ToStr16());
# endif
	SHVVA_END( argList );
}

void SHVConsole::vfwprintf(FILE* f, const WCHAR* str, SHVVA_LIST args)
{
SHVVA_LIST argList;
	SHVVA_COPY( argList, args );
# ifndef __SHIVAWINCE_EXCLUDE_CONSOLE_SUPPORT
	SHVMainThreadEventDispatcherConsole::Print(SHVString16C::FormatList((const SHVWChar*)str,argList));
# endif
	SHVVA_END( argList );
}

void SHVConsole::vfprintf(FILE* f, const SHVChar* str, SHVVA_LIST args)
{
SHVVA_LIST argList;
	SHVVA_COPY( argList, args );
# ifndef __SHIVAWINCE_EXCLUDE_CONSOLE_SUPPORT
	SHVMainThreadEventDispatcherConsole::Print(SHVString8C::FormatList(str,argList).ToStr16());
# endif
	SHVVA_END( argList );
}

void SHVConsole::wprintf(const WCHAR* str, ...)
{
SHVVA_LIST args;
	SHVVA_START(args,str);
	vwprintf(str,args);
	SHVVA_END(args);
}

void SHVConsole::fwprintf(FILE* f, const WCHAR* str, ...)
{
SHVVA_LIST args;
	SHVVA_START(args,str);
	vfwprintf(f,str,args);
	SHVVA_END(args);
}

///\endcond
#endif
