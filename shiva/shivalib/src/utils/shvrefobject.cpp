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

#include "../../../include/utils/shvrefobject.h"

#if __APPLE__
# include <libkern/OSAtomic.h>
#else
#ifdef ANDROID
#include <sys/atomics.h>
#elif defined(__GNUC__)
# if (__GNUC__ > 4) || ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 2))
#   include <ext/atomicity.h>
#   if defined(_GLIBCXX_BEGIN_NAMESPACE) || defined(_GLIBCXX_BEGIN_NAMESPACE_VERSION)
#    define GNUC_NAMESPACE __gnu_cxx
#   else
#    define GNUC_NAMESPACE
#   endif
# else
#   include <bits/atomicity.h>
#   define GNUC_NAMESPACE __gnu_cxx
# endif
#endif

#endif



//=========================================================================================================
// SHVRefObject - base reference counting class
//=========================================================================================================
/// \class SHVRefObject shvrefobject.h "shiva/include/utils/shvrefobject.h"

/*************************************
 * CreateRef
 *************************************/
/// Creates a reference, and returns the pointer
SHVRefObject* SHVRefObject::CreateRef()
{
	SHVASSERT(!DeleteInProgress);
	LockedIncrement(&References);
	return this;
}

/*************************************
 * ReleaseRef
 *************************************/
/// Releases a reference, without validating if the object needs to be deleted
/**
 * See the documentation for SHVRefObjectContainer<T>::ReleaseReference
 */
void SHVRefObject::ReleaseRef()
{
	LockedDecrement(&References);
} 

/*************************************
 * DestroyRef
 *************************************/
/// Releases a reference
void SHVRefObject::DestroyRef()
{
	LockedDecrement(&References);
	if (References <= 0 && !DeleteInProgress)
	{
		DeleteInProgress = true;
		delete this;
	}
}

///\cond INTERNAL
/*#if defined(__SHIVA_WIN32) && !defined(__SHIVA_WINCE) && !defined(__GNUC__)
void __fastcall RefObject_Inc(volatile int*)
{
	__asm
	{
		lock inc dword ptr [ecx]
	}
}
void __fastcall RefObject_Dec(volatile int*)
{
	__asm
	{
		lock dec dword ptr [ecx]
	}
}
#endif*/
///\endcond

/*************************************
 * LockedIncrement
 *************************************/
/// Thread safe ++
void SHVRefObject::LockedIncrement(volatile int* ref)
{
#ifdef __APPLE__
	OSAtomicIncrement32(ref);
#elif ANDROID
	__atomic_inc(ref);
#elif defined(__GNUC__)
	GNUC_NAMESPACE::__atomic_add(ref,1);
#elif defined(__SHIVA_WINCE)
	InterlockedIncrement((LPLONG)ref); // does not work in 64 bit
#elif defined(__SHIVA_WIN32)
	InterlockedIncrement((LPLONG)ref);
//	RefObject_Inc(ref);
#else
	(*ref)++;
#endif
}

/*************************************
 * LockedDecrement
 *************************************/
/// Thread safe --
void SHVRefObject::LockedDecrement(volatile int* ref)
{
#ifdef __APPLE__
	OSAtomicDecrement32(ref);
#elif ANDROID
	__atomic_dec(ref);
#elif __GNUC__
	GNUC_NAMESPACE::__atomic_add(ref,-1);
#elif defined(__SHIVA_WINCE)
	InterlockedDecrement((LPLONG)ref); // does not work in 64 bit
#elif defined(__SHIVA_WIN32)
	InterlockedDecrement((LPLONG)ref);
//	RefObject_Dec(ref);
#else
	(*ref)--;
#endif
}
