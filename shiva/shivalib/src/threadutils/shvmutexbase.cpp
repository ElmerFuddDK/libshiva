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

#include "../../../include/threadutils/shvmutexbase.h"

#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
# include <e32hal.h>
# include <e32base.h>
#else
# include <stdlib.h>
# include <sched.h>
# include <sys/time.h>
#endif


//=========================================================================================================
//  SHVMutexBase class - Non-thread aware locking class
//=========================================================================================================
/// \class SHVMutexBase shvmutexbase.h "shiva/include/threadutils/shvmutexbase.h"

/*************************************
 * Constructor
 *************************************/
SHVMutexBase::SHVMutexBase()
{
#ifdef __SHIVA_WIN32
	Locker = CreateMutex(NULL,FALSE,NULL);
	Mutex = 0;
	NewOwner = INVALID_HANDLE_VALUE;
#elif defined(__SHIVA_EPOC)
	Locker.CreateLocal();
	Mutex = 0;
	NewOwner = NULL;
#else
	pthread_mutex_init(&Locker,NULL);
	Mutex = 0;
	NewOwner = NULL;
#endif
}

/*************************************
 * Destructor
 *************************************/
SHVMutexBase::~SHVMutexBase()
{
	if (IsLocked()) Unlock();
#ifdef __SHIVA_WIN32
SHVListIterator<HANDLE,HANDLE> i(Queue);
	while (i.MoveNext()) // this should never ever happen
		CloseHandle(i.Get());
	CloseHandle(Locker);
#elif defined(__SHIVA_EPOC)
SHVListIterator<SHVQueuedThread,SHVQueuedThread&> i(Queue);
	while (i.MoveNext()) // this should never ever happen
	{
		if (i.Get().Timer)
			delete i.Get().Timer;
		if (i.Get().Mutex)
			delete i.Get().Mutex;
	}
	Locker.Close();
#else
	pthread_mutex_destroy(&Locker);
#endif
}

/*************************************
 * Lock
 *************************************/
/// Lock the mutex, with an optional timeout value
/**
 \param timeout Timeout in milliseconds (optional)
 \return Success
 *
 * This function will lock the mutex. If the mutex is locked already it will
 * wait for it to be unlocked, even if the calling thread has the lock.
 * If the timeout parameter is given, the thread will wait for the given delay
 * before giving up and returning false, unless the mutex is unlocked by
 * another thread within the duration.
 */
bool SHVMutexBase::Lock(int timeout)
{
#ifdef __SHIVA_WIN32
bool retVal;
	SHVVERIFY(WaitForSingleObject(Locker, INFINITE) == WAIT_OBJECT_0);
	if (Mutex == 1) // is the mutex locked ?
	{
	HANDLE queue;
	SHVListPos pos;

		// initialize wait object
		queue = CreateSemaphore(NULL,0,1,NULL);

		// add it to the fifo queue
		Queue.AddTail(queue);
		pos = Queue.GetTailPosition();

		ReleaseMutex(Locker);

		// wait for a signal
		WaitForSingleObject(queue, ( timeout == Infinite ? INFINITE : timeout ) );

		SHVVERIFY(WaitForSingleObject(Locker, INFINITE) == WAIT_OBJECT_0);

		// did we get the lock ?
		retVal = (NewOwner == queue);
		if (retVal)
			NewOwner = INVALID_HANDLE_VALUE;

		// cleanup
		Queue.RemoveAt(pos);
		CloseHandle(queue);
	}
	else
	{
		retVal = true;
		Mutex = 1;
	}
	ReleaseMutex(Locker);
	return retVal;
#elif defined(__SHIVA_EPOC)
bool retVal = false;
	Locker.Wait();
	if (Mutex == 1) // is the mutex locked ?
	{
	SHVQueuedThread queue;
	TRequestStatus status;
	SHVListPos pos;

		// initialize wait object
		queue.Owner = RThread().Id();
		if (timeout == Infinite)
		{
			queue.Timer = NULL;
			queue.Mutex = new RMutex();
			queue.Mutex->CreateLocal();
			queue.Mutex->Wait();
		}
		else
		{
			queue.Mutex = NULL;
			queue.Timer = new RTimer();
			queue.Timer->CreateLocal();
			queue.Timer->After(status, timeout*1000);
		}
		Queue.AddTail(queue);
		pos = Queue.GetTailPosition();


		// Release the FIFO queue lock
		Locker.Signal();

		// wait for signal
		if (queue.Mutex)
			queue.Mutex->Wait();
		else
			User::WaitForRequest(status);

		Locker.Wait();

		// did we get the lock ?
		retVal = (NewOwner == (queue.Mutex ? (void*)queue.Mutex : (void*)queue.Timer) );
		if (retVal)
			NewOwner = NULL;

		// cleanup
		Queue.RemoveAt(pos);
		if (queue.Timer)
		{
			queue.Timer->Cancel();
			queue.Timer->Close();
			delete queue.Timer;
		}
		if (queue.Mutex)
		{
			queue.Mutex->Signal();
			queue.Mutex->Close();
			delete queue.Mutex;
		}

	}
	else
	{
		retVal = true;
		Mutex = 1;
	}
	Locker.Signal();
	return retVal;
#else
bool retVal = false;
	pthread_mutex_lock(&Locker); // acquire rights to try lock
	if (Mutex == 1) // if locked
	{
	pthread_cond_t queue;
	timespec spc;
	SHVListPos pos;

		// initialize wait object
		pthread_cond_init(&queue,NULL);
		if (timeout != Infinite)
		{

#if _POSIX_TIMERS > 0
			if (clock_gettime(CLOCK_REALTIME, &spc))
			{
				fprintf(stderr,"MUTEX TIMER ERROR\n");
				abort();
			}
#else
#warning "Review the implementation"
			struct timeval tv;
			gettimeofday(&tv, NULL);
			spc.tv_sec = tv.tv_sec;
			spc.tv_nsec = tv.tv_usec*1000;
#endif
			spc.tv_sec  += timeout/1000;
			spc.tv_nsec += (timeout%1000)*1000000UL;
			if ( spc.tv_nsec >= 1000000000L )
			{
				spc.tv_sec  += 1;
				spc.tv_nsec -= 1000000000UL;
			}

		}

		// add it to the fifo queue
		Queue.AddTail(&queue);
		pos = Queue.GetTailPosition();

		// wait for a signal
		if (timeout == Infinite)
			pthread_cond_wait(&queue,&Locker);
		else
			pthread_cond_timedwait(&queue,&Locker,&spc);

		// did we get the lock ?
		retVal = (NewOwner == &queue);
		if (retVal)
			NewOwner = NULL;

		// cleanup
		Queue.RemoveAt(pos);
		pthread_cond_destroy(&queue);
	}
	else
	{
		retVal = true;
		Mutex = 1;
	}
	pthread_mutex_unlock(&Locker);

	return retVal;
#endif
}

/*************************************
 * Unlock
 *************************************/
/// Unlock the mutex, if locked
/**
 * This function unlocks the mutex, making it available again. If other
 * threads are waiting for the lock, the first thread in the FIFO queue
 * will be given the lock.
 */
void SHVMutexBase::Unlock()
{
#ifdef __SHIVA_WIN32
	SHVVERIFY(WaitForSingleObject(Locker, INFINITE) == WAIT_OBJECT_0);
	if (Queue.GetCount()) // somebody is waiting
	{
	SHVListPos pos;
	LONG prevCount;
		NewOwner = Queue.GetFirst(pos);
		ReleaseSemaphore(NewOwner,1,&prevCount);
	}
	else
	{
		Mutex = 0;
	}
	ReleaseMutex(Locker);
#elif defined(__SHIVA_EPOC)
	Locker.Wait();
	if (Queue.GetCount()) // somebody is waiting
	{
	SHVListPos pos;
	SHVQueuedThread queue = Queue.GetFirst(pos);

		if (queue.Timer)
		{
		RTimer timer( *queue.Timer );
		RThread thread;

			thread.Open(queue.Owner);

			timer.Duplicate(thread);
			timer.Cancel();

			timer.Close();
			thread.Close();
			NewOwner = queue.Timer;
		}
		if (queue.Mutex)
		{
			queue.Mutex->Signal();
			NewOwner = queue.Mutex;
		}
	}
	else
	{
		Mutex = 0;
	}
	Locker.Signal();
#else
	pthread_mutex_lock(&Locker);
	if (Queue.GetCount()) // somebody is waiting
	{
	SHVListPos pos;
		NewOwner = Queue.GetFirst(pos);
        pthread_cond_signal(NewOwner);
	}
	else
	{
		Mutex = 0;
	}
	pthread_mutex_unlock(&Locker);
#endif
}

/*************************************
 * IsLocked
 *************************************/
/// Tests if the mutex is locked
/**
 \return Returns true if the mutex is locked
 */
bool SHVMutexBase::IsLocked()
{
#ifdef __SHIVA_WIN32
	return Mutex == 1;
#elif defined(__SHIVA_EPOC)
	return Mutex == 1;
#else
	return Mutex == 1;
#endif
}
