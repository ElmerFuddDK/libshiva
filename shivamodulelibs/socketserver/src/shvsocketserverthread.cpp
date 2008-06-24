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
#include <unistd.h>
#include <fcntl.h>

#include <arpa/inet.h>

///\cond INTERNAL
//=========================================================================================================
// SHVSocketServerThread class - The socket handling thread
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVSocketServerThread::SHVSocketServerThread(SHVSocketServerImpl* server) : SocketServer(server)
{
	// Initialize pipe signaller for the select statement
	pipe(PipeSignal);

	// disable blocking mode on the pipes - close on failure
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
	
	QueueSignalled = false;
}

/*************************************
 * Destructor
 *************************************/
SHVSocketServerThread::~SHVSocketServerThread()
{
	WaitForTermination();
	if (PipeSignal[0])
		close(PipeSignal[0]);
	if (PipeSignal[1])
		close(PipeSignal[1]);
}


/*************************************
 * StartThread
 *************************************/
SHVBool SHVSocketServerThread::StartThread(SHVModuleList& modules)
{
	// don't start the thread if the 
	if (!PipeSignal[0] || !PipeSignal[1])
		return SHVBool::False;
		
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
	if (SocketThread.IsRunning() && !QueueSignalled)
	{
		QueueSignalled = true;
		write(PipeSignal[1],"1",1);
	}
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
fd_set rfds;
int nextFD, retVal;
char dummyBuffer[50];
SHVListIterator<SHVSocketImplRef,SHVSocketImpl*> SocketListItr(SocketServer->SocketList);
SHVList<SHVSocketImplRef,SHVSocketImpl*> pendingList;
SHVListIterator<SHVSocketImplRef,SHVSocketImpl*> pendingListItr(pendingList);

	SignalDispatcher();

	while (!KillSignal)
	{
		// fill file decriptor list for select
		FD_ZERO(&rfds);
		FD_SET(PipeSignal[0], &rfds);
		nextFD = PipeSignal[0]+1;
		
		// fill in the socket file descriptors
		SocketServer->SocketServerLock.Lock();
		for (SocketListItr.Pos() = NULL; SocketListItr.MoveNext();)
		{
			if (SocketListItr.Get()->GetState() != SHVSocket::StateError &&
				SocketListItr.Get()->Socket != SHVSocketImpl::InvalidSocket)
			{
				FD_SET(SocketListItr.Get()->Socket,&rfds);
				
				if (SocketListItr.Get()->Socket >= nextFD)
					nextFD = SocketListItr.Get()->Socket+1;
			}
		}
		SocketServer->SocketServerLock.Unlock();
		
		
		// wait for incoming events
		retVal = select(nextFD, &rfds, NULL, NULL, NULL);
		
		if (!retVal || KillSignal) // nothing happened, or terminating
		{
			continue;
		}
		else if (retVal == -1) // ERROR!
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
		
		
		// Dispatch normal events
		EventList.DispatchEvents(*Modules);
		
		// Run through sockets and cache the sockets that are pending
		SocketServer->SocketServerLock.Lock();
		for (SocketListItr.Pos() = NULL; SocketListItr.MoveNext();)
		{
			// check for activity on socket
			if (FD_ISSET(SocketListItr.Get()->Socket,&rfds))
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
}
///\endcond
