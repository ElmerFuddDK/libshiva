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
#include "../../../include/threadutils/shvthreadbase.h"
#include "../../../include/utils/shvdir.h"
#include "../../../include/utils/shvbuffer.h"

#include "../../../include/frameworkimpl/shvmainthreadeventdispatcherconsole.h"
#include "../../../include/framework/shveventstdin.h"

#ifdef __SHIVA_WIN32
# include <io.h>
# define EventInternalSdin -1
#else
# include <unistd.h>
# include <fcntl.h>
# include <errno.h>
#endif
#include <string.h>


//=========================================================================================================
// SHVMainThreadEventDispatcherConsole class - Dispatcher for console user IO
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVMainThreadEventDispatcherConsole::SHVMainThreadEventDispatcherConsole()
{
#ifdef __SHIVA_WIN32
	Initializing = false;
#else
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
#endif
}

/*************************************
 * Destructor
 *************************************/
SHVMainThreadEventDispatcherConsole::~SHVMainThreadEventDispatcherConsole()
{
#ifdef __SHIVA_WIN32
	if (Initializing)
	{
		::CloseHandle(GetStdHandle(STD_INPUT_HANDLE));
		for (int i=10;StdinThread.IsRunning();i+=10)
			SHVThreadBase::Sleep(i);
	}
	SHVASSERT(!StdinThread.IsRunning());
#else
	if (PipeSignal[0])
		close(PipeSignal[0]);
	if (PipeSignal[1])
		close(PipeSignal[1]);
#endif
}

/*************************************
 * SetupDefaults
 *************************************/
void SHVMainThreadEventDispatcherConsole::SetupDefaults(SHVModuleList& modules)
{
#ifdef __SHIVA_WIN32
	selfSubs = new SHVEventSubscriber(this,&modules);
	ModuleList = &modules;
#endif
	modules.GetConfig().Set(SHVModuleList::DefaultCfgAppPath,SHVStringC(_T(".")) + SHVDir::Delimiter());
	modules.GetConfig().Set(SHVModuleList::DefaultCfgAppName,SHVStringC(_T("")));
}

/*************************************
 * SignalDispatcher
 *************************************/
void SHVMainThreadEventDispatcherConsole::SignalDispatcher()
{
#ifdef __SHIVA_WIN32
	Signal.Unlock();
#else
	Mutex.Lock();
	if (Running() && !QueueSignalled)
	{
		QueueSignalled = true;
		write(PipeSignal[1],"1",1);
	}
	Mutex.Unlock();
#endif
}

/*************************************
 * InitializeEventLoop
 *************************************/
SHVBool SHVMainThreadEventDispatcherConsole::InitializeEventLoop()
{
#ifdef __SHIVA_WIN32
	Initializing = true;
	Signal.Lock();
	StdinThread.Start(this,&SHVMainThreadEventDispatcherConsole::StdinFunc);
	return StdinThread.IsRunning();
#else
	StdinPos = 0;
	return (PipeSignal[0] && PipeSignal[1] ? SHVBool::True : SHVBool::False);
#endif
}

/*************************************
 * RunEventLoop
 *************************************/
void SHVMainThreadEventDispatcherConsole::RunEventLoop()
{
#ifndef __SHIVA_WIN32
fd_set rfds;
int nextFD, retVal;
char dummyBuffer[50];
SHVBufferRef readBuf;
size_t bufReadPos = 0;
#endif

	DispatchEvents();

#ifdef __SHIVA_WIN32
	Initializing = false;
#endif

	while ( Running() )
	{
#ifdef __SHIVA_WIN32
		Signal.Lock();
		DispatchEvents();
#else
		// fill file decriptor list for select
		FD_ZERO(&rfds);
		FD_SET(PipeSignal[0], &rfds);
		nextFD = PipeSignal[0]+1;
		FD_SET(STDIN_FILENO,&rfds);
		
		if (STDIN_FILENO > nextFD)
			nextFD = STDIN_FILENO+1;
	
		retVal = select(nextFD, &rfds, NULL, NULL, NULL);
		
		if (!retVal || !Running()) // nothing happened, or terminating
		{
			continue;
		}
		else if (retVal == -1) // ERROR!
		{
			if (errno == EINTR) // we got a signal
				continue;
			printf("Error in main event loop\n");
			_exit(-1);
		}
		
		// clear the signal pipe if present
		if (FD_ISSET(PipeSignal[0],&rfds))
		{
			// If this read is blocking we will be in trouble ... oh well
			Mutex.Lock();
			QueueSignalled = false;
			read(PipeSignal[0], dummyBuffer, 50);
			Mutex.Unlock();
			
			DispatchEvents();
		}
		
		// Check for stdin
		if (FD_ISSET(STDIN_FILENO,&rfds))
		{
			if (bufReadPos == StdinBufSize || readBuf.IsNull())
			{
				readBuf = new SHVBufferChunk<StdinBufSize>();
				::memset(readBuf->GetBuffer(),0,StdinBufSize);
				bufReadPos = 0;
				
				StdinStream.AddBuffer(readBuf);
			}

			retVal = read(STDIN_FILENO, readBuf->GetBuffer()+bufReadPos, StdinBufSize-bufReadPos);
			
			if (retVal <=0)
			{
				printf("Error in main event loop whilst processing stdin\n");
				_exit(-1);
			}
			else
			{
				bufReadPos += retVal;
				HandleStdin();
			}
		}
#endif
	}

#ifdef __SHIVA_WIN32
	::CloseHandle(GetStdHandle(STD_INPUT_HANDLE));
	for (int i=10;StdinThread.IsRunning();i+=10)
		SHVThreadBase::Sleep(i);
#endif
}

/*************************************
 * StopEventLoop
 *************************************/
void SHVMainThreadEventDispatcherConsole::StopEventLoop(SHVBool errors)
{
	SignalDispatcher();
	if (!errors)
	{
	SHVString errStr = Queue->GetModuleList().GetStartupErrors();
		fprintf(stderr, "\n\nRegistering failed:\n\"%s\"\n\n", errStr.GetSafeBuffer());
	}
}

/*************************************
 * HandleStdin
 *************************************/
void SHVMainThreadEventDispatcherConsole::HandleStdin()
{
size_t newline;
SHVString8 str, cmd, data;

	while ( (newline = StdinStream.SeekByte('\n', StdinPos)) < StdinStream.GetSize())
	{
		str.SetBufferSize(newline - StdinPos);
		StdinStream.ReadString8(str,newline - StdinPos,StdinPos);
		StdinPos = newline + 1;
		
#ifdef __SHIVA_WIN32
		this->selfSubs->EmitNow(*ModuleList,new SHVEvent(this,EventInternalSdin,SHVInt(),new SHVEventStdin(NULL,str.ReleaseBuffer())));
#else
		Queue->GetModuleList().EmitEvent(new SHVEventStdin(NULL,str.ReleaseBuffer()));
#endif
		
		StdinStream.Truncate(StdinPos);
	}
}

/*************************************
 * OnEvent
 *************************************/
void SHVMainThreadEventDispatcherConsole::OnEvent(SHVEvent* event)
{
#ifdef __SHIVA_WIN32
	if (event->GetCaller() == this && SHVEvent::Equals(event,EventInternalSdin))
	{
		Queue->GetModuleList().EmitEvent((SHVEvent*)event->GetObject());
	}
#else
	SHVUNUSED_PARAM(event);
#endif
}

#ifdef __SHIVA_WIN32
///\cond INTERNAL
/*************************************
 * OnEvent
 *************************************/
void SHVMainThreadEventDispatcherConsole::StdinFunc()
{
SHVBufferRef readBuf;
size_t bufReadPos = 0;
int retVal;

	StdinPos = 0;
	while (Running() || Initializing)
	{
		if (bufReadPos == StdinBufSize || readBuf.IsNull())
		{
			readBuf = new SHVBufferChunk<StdinBufSize+1>();
			::memset(readBuf->GetBuffer(),0,StdinBufSize);
			bufReadPos = 0;
			
			StdinStream.AddBuffer(readBuf);
		}

		retVal = _read(0, readBuf->GetBuffer()+bufReadPos, (unsigned int)StdinBufSize-bufReadPos);
		
		if (retVal <=0)
		{
			if (!Running())
				continue;
			printf("Error in main event loop whilst processing stdin\n");
			break;
		}
		else
		{
			bufReadPos += retVal;
			readBuf->GetBuffer()[bufReadPos] = '\0'; // clear any unwanted Xtra newline
			HandleStdin();
		}
	}
}
///\endcond
#endif
