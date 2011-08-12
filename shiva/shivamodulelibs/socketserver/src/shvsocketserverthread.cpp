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

#include "../include/shvsocketserverimpl.h"
#if defined(__SHIVA_WINCE)
// nothing special - use a mutex for signalling
#elif defined(__SHIVA_WIN32)
# include <io.h>
# define pipe(x) _pipe(x,50,0);
# include <fcntl.h>
#else
# include <unistd.h>
# include <arpa/inet.h>
# include <errno.h>
# include <fcntl.h>
#endif


///\cond INTERNAL
//=========================================================================================================
// SHVSocketServerThread class - The socket handling thread
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVSocketServerThread::SHVSocketServerThread(SHVSocketServerImpl* server) : SocketServer(server)
{
#ifndef __SHIVASOCKETS_NOSELECTMODE
	// Initialize pipe signaller for the select statement
	pipe(PipeSignal);
#endif

	// disable blocking mode on the pipes - close on failure
#if !defined(__SHIVA_WIN32) && !defined(__SHIVASOCKETS_NOSELECTMODE)
	if (fcntl(PipeSignal[0], F_SETFL, O_NONBLOCK) < 0)
	{
		close(PipeSignal[0]);
		PipeSignal[0] = 0;
	}
	if (fcntl(PipeSignal[1], F_SETFL, O_NONBLOCK) < 0)
	{
		close(PipeSignal[1]);
		PipeSignal[1] = 0;
	}
#endif
	
	QueueSignalled = false;
}

/*************************************
 * Destructor
 *************************************/
SHVSocketServerThread::~SHVSocketServerThread()
{
	WaitForTermination();
#ifndef __SHIVASOCKETS_NOSELECTMODE
	if (PipeSignal[0])
		close(PipeSignal[0]);
	if (PipeSignal[1])
		close(PipeSignal[1]);
#endif
}


/*************************************
 * StartThread
 *************************************/
SHVBool SHVSocketServerThread::StartThread(SHVModuleList& modules)
{
#ifndef __SHIVASOCKETS_NOSELECTMODE
	// don't start the thread if the pipe isn't created
	if (!PipeSignal[0] || !PipeSignal[1])
		return SHVBool::False;
#endif
		
	if (!SocketThread.IsRunning())
	{
		Modules = &modules;
		KillSignal = false;
		SocketThread.Start(this,&SHVSocketServerThread::ThreadFunc);
	}
	
	return SocketThread.IsRunning();
}

/*************************************
 * StopThread
 *************************************/
void SHVSocketServerThread::StopThread()
{
	if (SocketThread.IsRunning())
	{
		KillSignal = true;
		SignalDispatcher();
	}
}

/*************************************
 * WaitForTermination
 *************************************/
void SHVSocketServerThread::WaitForTermination()
{
	StopThread();

	while (SocketThread.IsRunning())
	{
		KillSignal = true;
		SignalDispatcher();
		SHVThreadBase::Sleep(40);
	}
}

// EventQueue functions
/*************************************
 * EnqueueEvent
 *************************************/
void SHVSocketServerThread::EnqueueEvent(SHVEvent* event, SHVEventSubscriberBase* subscriber)
{
	EventList.EnqueueEvent(*Modules,event,subscriber);
}

/*************************************
 * SignalDispatcher
 *************************************/
void SHVSocketServerThread::SignalDispatcher()
{
	SocketServer->SocketServerLock.Lock();
#ifdef __SHIVASOCKETS_NOSELECTMODE
	if (SocketThread.IsRunning() && SocketServer->ThreadSignal.IsLocked())
		SocketServer->ThreadSignal.Unlock();
#else
	if (SocketThread.IsRunning() && !QueueSignalled)
	{
		QueueSignalled = true;
		write(PipeSignal[1],"1",1);
	}
#endif
	SocketServer->SocketServerLock.Unlock();
}

/*************************************
 * GetThreadID
 *************************************/
SHVThreadBase::ThreadID SHVSocketServerThread::GetThreadID()
{
	return SocketThread.GetThreadID();
}

/*************************************
 * LockEvent
 *************************************/
SHVBool SHVSocketServerThread::LockEvent()
{
	return EventList.LockEvent();
}

/*************************************
 * UnlockEvent
 *************************************/
void SHVSocketServerThread::UnlockEvent()
{
	EventList.UnlockEvent();
}


/*************************************
 * ThreadFunc
 *************************************/
void SHVSocketServerThread::ThreadFunc()
{
#ifndef __SHIVASOCKETS_NOSELECTMODE
fd_set rfds;
fd_set wfds;
int nextFD, retVal;
char dummyBuffer[50];
#endif
SHVListIterator<SHVSocketImplRef,SHVSocketImpl*> SocketListItr(SocketServer->SocketList);
SHVList<SHVSocketImplRef,SHVSocketImpl*> pendingList;
SHVListIterator<SHVSocketImplRef,SHVSocketImpl*> pendingListItr(pendingList);

	SignalDispatcher();

	while (!KillSignal)
	{
#ifdef __SHIVASOCKETS_NOSELECTMODE
		SocketServer->ThreadSignal.Lock(); // will block until we get a signal
#else
		// fill file decriptor list for select
		FD_ZERO(&rfds);
		FD_ZERO(&wfds);
		FD_SET(PipeSignal[0], &rfds);
		nextFD = PipeSignal[0]+1;
		
		// fill in the socket file descriptors
		SocketServer->SocketServerLock.Lock();
		for (SocketListItr.Pos() = NULL; SocketListItr.MoveNext();)
		{
			if (SocketListItr.Get()->GetState() != SHVSocket::StateError &&
				SocketListItr.Get()->Socket != SHVSocketImpl::InvalidSocket)
			{
				if (SocketListItr.Get()->GetState() == SHVSocket::StateConnecting)
					FD_SET(SocketListItr.Get()->Socket,&wfds);
				else
					FD_SET(SocketListItr.Get()->Socket,&rfds);
				
				if ((int)SocketListItr.Get()->Socket >= nextFD)
					nextFD = int(SocketListItr.Get()->Socket)+1;
			}
		}
		SocketServer->SocketServerLock.Unlock();
		
		// wait for incoming events
		retVal = select(nextFD, &rfds, &wfds, NULL, NULL);
		
		if (!retVal || KillSignal) // nothing happened, or terminating
		{
			continue;
		}
		else if (
				retVal == -1
# ifdef __SHIVA_POSIX
				&& errno != EINTR
#endif
				) // ERROR!
		{
			// insert error handling here
			KillSignal = true;
			continue;
		}
		
		// clear the signal pipe if present
		if (FD_ISSET(PipeSignal[0],&rfds))
		{
			// If this read is blocking we will be in trouble ... oh well
			SocketServer->SocketServerLock.Lock();
			QueueSignalled = false;
			read(PipeSignal[0], dummyBuffer, 50);
			SocketServer->SocketServerLock.Unlock();
		}
#endif	
		
		
		// Dispatch normal events
		EventList.DispatchEvents(*Modules);
		
		// Run through sockets and cache the sockets that are pending
		SocketServer->SocketServerLock.Lock();
		for (SocketListItr.Pos() = NULL; SocketListItr.MoveNext();)
		{
			// check for activity on socket
#ifdef __SHIVASOCKETS_NOSELECTMODE
			if (SocketListItr.Get()->IsPending)
#else
			if (FD_ISSET(SocketListItr.Get()->Socket,&rfds) || FD_ISSET(SocketListItr.Get()->Socket,&wfds))
#endif
			{
				pendingList.AddTail(SocketListItr.Get());
			}
		}
		SocketServer->SocketServerLock.Unlock();
		// now that the list is unlocked we can perform the events
		while (pendingListItr.MoveNext())
		{
			pendingListItr.Get()->PerformEvent();
		}
		pendingList.RemoveAll();
		
	}
#ifdef __SHIVASOCKETS_NOSELECTMODE
	SocketServer->ThreadSignal.Unlock(); // release the signal
#endif
}
///\endcond
