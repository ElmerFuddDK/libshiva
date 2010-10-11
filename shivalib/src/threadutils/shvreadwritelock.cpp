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

#include "../../../include/threadutils/shvreadwritelock.h"



//=========================================================================================================
//  SHVReadWriteLock class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVReadWriteLock::SHVReadWriteLock()
{
	ExclusiveOwner = SHVMutexBase::InvalidThreadID;
	RefCount = 0;
}

/*************************************
 * LockShared
 *************************************/
void SHVReadWriteLock::LockShared()
{
	Lock.Lock();
	if (RefCount && ExclusiveOwner != SHVMutexBase::InvalidThreadID
				 && ExclusiveOwner != SHVThreadBase::GetCurrentThreadID())
	{
	SHVMutexBase waitLock;
		waitLock.Lock();
		WaitQueue.AddTail(Wait(waitLock,SHVMutexBase::InvalidThreadID));
		Lock.Unlock();

		waitLock.Lock();

		Lock.Lock();
		waitLock.Unlock();
		SHVASSERT(ExclusiveOwner == SHVMutexBase::InvalidThreadID && RefCount);
	}
	else
	{
		RefCount++;
	}
	Lock.Unlock();
}

/*************************************
 * LockExclusive
 *************************************/
void SHVReadWriteLock::LockExclusive()
{
	Lock.Lock();
	if (RefCount && ExclusiveOwner != SHVThreadBase::GetCurrentThreadID())
	{
	SHVMutexBase waitLock;
		waitLock.Lock();
		WaitQueue.AddTail(Wait(waitLock,SHVThreadBase::GetCurrentThreadID()));
		Lock.Unlock();

		waitLock.Lock();

		Lock.Lock();
		waitLock.Unlock();
		SHVASSERT(ExclusiveOwner == SHVThreadBase::GetCurrentThreadID() && RefCount);
	}
	else
	{
		RefCount++;
		ExclusiveOwner = SHVThreadBase::GetCurrentThreadID();
	}
	Lock.Unlock();
}

/*************************************
 * TryLockExclusive
 *************************************/
SHVBool SHVReadWriteLock::TryLockExclusive()
{
SHVBool retVal;
	Lock.Lock();
	if (RefCount && ExclusiveOwner != SHVThreadBase::GetCurrentThreadID())
	{
		retVal = SHVBool::False;
	}
	else
	{
		RefCount++;
		ExclusiveOwner = SHVThreadBase::GetCurrentThreadID();
		retVal = SHVBool::True;
	}
	Lock.Unlock();
	return retVal;
}

/*************************************
 * Unlock
 *************************************/
void SHVReadWriteLock::Unlock()
{
	Lock.Lock();
	SHVASSERT(RefCount > 0);
	if (--RefCount == 0)
	{
		if (WaitQueue.GetCount())
		{
			do 
			{
			Wait wait(WaitQueue.PopHead());

				RefCount++;
				ExclusiveOwner = wait.NewOwner;
				wait.WaitObject.Unlock();
			} while (ExclusiveOwner == SHVMutexBase::InvalidThreadID && WaitQueue.GetCount() && WaitQueue.GetFirst().NewOwner == SHVMutexBase::InvalidThreadID);
		}
		else
		{
			ExclusiveOwner = SHVMutexBase::InvalidThreadID;
		}
	}
	Lock.Unlock();
}

/*************************************
 * IsExclusiveLocked
 *************************************/
bool SHVReadWriteLock::IsExclusiveLocked()
{
	return (ExclusiveOwner != SHVMutexBase::InvalidThreadID);
}

/*************************************
 * HasExclusiveLock
 *************************************/
bool SHVReadWriteLock::HasExclusiveLock()
{
	return (ExclusiveOwner == SHVThreadBase::GetCurrentThreadID());
}
