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
#include "../../../include/threadutils/shvmutexbase.h"

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

// Defines for handling how reference counting is made atomic
///\cond INTERNAL
#ifdef __APPLE__
# define __SHIVA_REFOBJ_APPLE
# define __SHIVA_REFOBJ_HASATOMICDECCHECK
#elif ANDROID
# define __SHIVA_REFOBJ_ANDROID
# define __SHIVA_REFOBJ_HASATOMICDECCHECK
#elif defined(__GNUC__)
# define __SHIVA_REFOBJ_GNUC
# define __SHIVA_REFOBJ_HASATOMICDECCHECK
#elif defined(__SHIVA_WIN32)
# define __SHIVA_REFOBJ_WIN32
# define __SHIVA_REFOBJ_HASATOMICDECCHECK
#else
static SHVMutexBase RefObj_GlobalRefLock;
# warning No atomic add - using global lock
#endif

#ifndef __SHIVA_REFOBJ_HASATOMICDECCHECK
static SHVMutexBase RefObj_GlobalDeleteLock;
#endif
///\endcond


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
	SHVASSERT(!References.DeleteInProgress);
	References.LockedIncrement();
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
	References.LockedDecrement();
} 

/*************************************
 * DestroyRef
 *************************************/
/// Releases a reference
void SHVRefObject::DestroyRef()
{
	if (References.LockedDecrementAndCheckDelete())
		delete this;
}

/*************************************
 * LockedIncrement
 *************************************/
/// Thread safe ++
void SHVRefObject::LockedIncrement(volatile int* ref)
{
#ifdef __SHIVA_REFOBJ_APPLE
	OSAtomicAdd32(1,ref);
#elif defined(__SHIVA_REFOBJ_ANDROID)
	__atomic_inc(ref);
#elif defined(__SHIVA_REFOBJ_GNUC)
	GNUC_NAMESPACE::__atomic_add(ref,1);
#elif defined(__SHIVA_REFOBJ_WIN32)
	InterlockedIncrement((LPLONG)ref);
#else
	RefObj_GlobalRefLock.Lock();
	(*ref)++;
	RefObj_GlobalRefLock.Unlock();
#endif
}

/*************************************
 * LockedDecrement
 *************************************/
/// Thread safe --
void SHVRefObject::LockedDecrement(volatile int* ref)
{


#ifdef __SHIVA_REFOBJ_APPLE
	OSAtomicAdd32(-1,ref);
#elif defined(__SHIVA_REFOBJ_ANDROID)
	__atomic_dec(ref);
#elif defined(__SHIVA_REFOBJ_GNUC)
	GNUC_NAMESPACE::__atomic_add(ref,-1);
#elif defined(__SHIVA_REFOBJ_WIN32)
	InterlockedDecrement((LPLONG)ref);
#else
	RefObj_GlobalRefLock.Lock();
	(*ref)--;
	RefObj_GlobalRefLock.Unlock();
#endif
}

///\cond INTERNAL
/*************************************
 * RefData::LockedDecrementAndCheckDelete
 *************************************/
bool SHVRefObject::RefData::LockedDecrementAndCheckDelete()
{
bool retVal;
#ifdef __SHIVA_REFOBJ_HASATOMICDECCHECK

# ifdef __SHIVA_REFOBJ_APPLE
	retVal = (OSAtomicAdd32(-1,&References) <= 0);
# elif defined(__SHIVA_REFOBJ_ANDROID)
	retVal = (__atomic_dec(&References) <= 1);
# elif defined(__SHIVA_REFOBJ_GNUC)
	retVal = (GNUC_NAMESPACE::__exchange_and_add(&References,-1) <= 1);
# elif defined(__SHIVA_REFOBJ_WIN32)
	retVal = (InterlockedDecrement((LPLONG)&References) <= 0);
# else
#  error Define FUBAR in SHVRefObject
# endif

	if (retVal)
		DeleteInProgress = true;
	
#else

	RefObj_GlobalDeleteLock.Lock();
	SHVRefObject::LockedDecrement(&References);
	if (References <= 0 && !DeleteInProgress)
		retVal = DeleteInProgress = true;
	else
		retVal = false;
	RefObj_GlobalDeleteLock.Unlock();

#endif
	
	return retVal;
}

/*************************************
 * RefData::RefInvalid
 *************************************/
bool SHVRefObject::RefData::RefInvalid()
{
bool retVal;

	if (References <= 0 && !DeleteInProgress)
	{
		retVal = DeleteInProgress = true;
	}
	else
	{
		retVal = false;
	}
	
	return retVal;
}
///\endcond
