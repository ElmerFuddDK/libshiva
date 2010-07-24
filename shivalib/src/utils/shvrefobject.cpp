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

#ifdef __GNUC__
# include <ext/atomicity.h>
# ifdef _GLIBCXX_BEGIN_NAMESPACE
#  define GNUC_NAMESPACE __gnu_cxx
# else
#  define GNUC_NAMESPACE
# endif
#endif


//=========================================================================================================
// SHVRefObject - base reference counting class
//=========================================================================================================

/*************************************
 * CreateRef
 *************************************/
/// Creates a reference, and returns the pointer
SHVRefObject* SHVRefObject::CreateRef()
{
	SHVASSERT(!DeleteInProgress);
	LockedIncrement(References);
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
	LockedDecrement(References);
} 

/*************************************
 * DestroyRef
 *************************************/
/// Releases a reference
void SHVRefObject::DestroyRef()
{
	LockedDecrement(References);
	if (References <= 0 && !DeleteInProgress)
	{
		DeleteInProgress = true;
		delete this;
	}
}

/*************************************
 * LockedIncrement
 *************************************/
/// Thread safe ++
void SHVRefObject::LockedIncrement(int& ref)
{
#ifdef __GNUC__
	GNUC_NAMESPACE::__atomic_add(&ref,1);
#else
	ref++;
#endif
}

/*************************************
 * LockedDecrement
 *************************************/
/// Thread safe --
void SHVRefObject::LockedDecrement(int& ref)
{
#ifdef __GNUC__
	GNUC_NAMESPACE::__atomic_add(&ref,-1);
#else
	ref--;
#endif
}
