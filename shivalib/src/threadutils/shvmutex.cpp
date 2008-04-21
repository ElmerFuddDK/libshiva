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

#include "../../../include/threadutils/shvmutex.h"


// ========================================================================================================
//  SHVMutex class
// ========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVMutex::SHVMutex()
{
	RefCount = 0;
	Owner = SHVMutexBase::InvalidThreadID;
}

 /*************************************
 * Lock
 *************************************/
/// Lock the mutex, with an optional timeout value
/**
 \param timeout Timeout in milliseconds (optional)
 \return Success
 *
 * This function will lock the mutex. If the mutex is locked by
 * the same thread, its reference count is incremented.
 *
 * If another thread attempts to get the lock it will wait for
 * it to be made available.
 */
bool SHVMutex::Lock(int timeout)
{
bool retVal;
	if (HasLock())
		retVal = true;
	else
		retVal = SHVMutexBase::Lock(timeout);

	if (retVal)
	{
		RefCount++;
		Owner = SHVThreadBase::GetCurrentThreadID();
	}

	return retVal;
}

/*************************************
 * Unlock
 *************************************/
/// Unlock the mutex, if locked and final reference
/**
 * This function will decrement the reference count, and unlock
 * the thread if no pending locks are present.
 * It will only do so if called from the owning thread tho.
 */
void SHVMutex::Unlock()
{
	SHVASSERT(HasLock());

	if (HasLock())
	{
		if (--RefCount == 0)
		{
			Owner = SHVMutexBase::InvalidThreadID;
			SHVMutexBase::Unlock();
		}
	}
}

/*************************************
 * UnlockAll
 *************************************/
/// Unlocks all references to the mutex
/**
 \return Number of references
 *
 * Will return the number of references to the lock by the calling
 * thread. It will return 0 if no locks were active, or it was
 * locked by another thread.
 */
int SHVMutex::UnlockAll()
{
int retVal;
	if (HasLock())
	{
		retVal = RefCount;
		RefCount = 0;
		Owner = SHVMutexBase::InvalidThreadID;
		SHVMutexBase::Unlock();
	}
	else
		retVal = 0;

	return retVal;
}

/*************************************
 * LockMultiple
 *************************************/
/// Locks the mutex multiple times
/**
 \param refCount The number of locks to acquire
 \param timeout  Timeout in milliseconds (optional)
 \return Success
 *
 * This function will lock the mutex. If the mutex is locked by
 * the same thread, incrementing the reference count refCount times.
 *
 * This function and UnlockAll are meant as a way to temporarily
 * completely release the lock to a mutex, and restoring the
 * original mutex state afterwards.
 */
bool SHVMutex::LockMultiple(int refCount, int timeout)
{
bool retVal;

	if (refCount <= 0)
		return true;
	else if (HasLock())
		retVal = true;
	else
		retVal = SHVMutexBase::Lock(timeout);

	if (retVal)
	{
		RefCount += refCount;
		Owner = SHVThreadBase::GetCurrentThreadID();
	}

	return retVal;
}

/*************************************
 * IsLocked
 *************************************/
/// Tests if the mutex is locked
/**
 \return Returns true if the mutex is locked
 */
bool SHVMutex::IsLocked()
{
	return SHVMutexBase::IsLocked();
}

/*************************************
 * GetOwner
 *************************************/
/// Get the thread id of the locking owner
/**
 \return Returns the thread id of the owner
 *
 * This function will return SHVMutexBase::InvalidThreadID if the
 * mutex is not locked.
 */
SHVThreadBase::ThreadID SHVMutex::GetOwner()
{
	return Owner;
}

/*************************************
 * HasLock
 *************************************/
/// Check if the calling thread has the lock
/**
 \return True if the mutex is locked by the calling thread
 */
bool SHVMutex::HasLock()
{
	return (SHVMutexBase::IsLocked() && Owner == SHVThreadBase::GetCurrentThreadID());
}

/*************************************
 * GetLockedCount
 *************************************/
/// Returns the amount of times the mutex has been locked
int SHVMutex::GetLockCount()
{
	return RefCount;
}
