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

#include "../../../include/threadutils/shvsemaphore.h"


// ========================================================================================================
//  SHVSemaphore class
// ========================================================================================================
/// \class SHVSemaphore shvsemaphore.h "shiva/include/threadutils/shvsemaphore.h"

/*************************************
 * Constructor
 *************************************/
SHVSemaphore::SHVSemaphore(unsigned resourceCount) : ResourceCount(resourceCount)
{
}

/*************************************
 * Destructor
 *************************************/
SHVSemaphore::~SHVSemaphore()
{
	SHVASSERT(WaitQueue.GetCount() == 0);
}

/*************************************
 * Lock
 *************************************/
/// Locks the semaphore object
/**
 * It is not nessecary to lock a semaphore before waiting, but
 * this function will lock it, so you can check if there are
 * enough resources before waiting:
\code
bool TryDoStuff(SHVSemaphore sem, int resources)
{
	sem.Lock();
	if (sem.GetResourceCount() >= resources)
	{
		sem.Wait(resources);
		sem.Unlock();
		
		DoStuff();
		
		sem.Signal(resources);
		return true;
	}
	else
	{
		sem.Unlock();
	}
	return false;
}
\endcode
 */
void SHVSemaphore::Lock()
{
	SignalLock.Lock();
}

/*************************************
 * Unlock
 *************************************/
/// Unlocks the semaphore object
/**
 \see Lock
 */
void SHVSemaphore::Unlock()
{
	SignalLock.Unlock();
}

/*************************************
 * Wait
 *************************************/
/// Attempts to consume resources
/**
 \param resources The amount of resources to take (default 1)
 *
 * Will block if resources are not available, until they are
 * signalled.
 */
void SHVSemaphore::Wait(unsigned resources)
{
	SHVASSERT(resources > 0);
	SignalLock.Lock();
	if (ResourceCount < resources)
	{
	WaitLock waitLock(resources);
	int lockCount;
		
		WaitQueue.AddTail(&waitLock);
		lockCount = SignalLock.UnlockAll() - 1; // unlock all, but don't lock again afterwards
		
		waitLock.Lock.Lock(); // Will wait until we have the resources
		waitLock.Lock.Unlock();

		if (lockCount > 0)
			SignalLock.LockMultiple(lockCount);
	}
	else
	{
		ResourceCount -= resources;
		SignalLock.Unlock();
	}
}

/*************************************
 * Signal
 *************************************/
/// Signals the semaphore, releasing any waiting threads
void SHVSemaphore::Signal(unsigned resources)
{
	SHVASSERT(resources > 0);
	SignalLock.Lock();
	ResourceCount += resources;
	while (WaitQueue.GetCount() && WaitQueue.GetFirst()->ResourceCount <= ResourceCount)
	{
		ResourceCount -= WaitQueue.GetFirst()->ResourceCount;
		WaitQueue.PopHead()->Lock.Unlock();
	}
	SignalLock.Unlock();
}

/*************************************
 * GetResourceCount
 *************************************/
/// Returns the available resources
int SHVSemaphore::GetResourceCount()
{
	return ResourceCount;
}
