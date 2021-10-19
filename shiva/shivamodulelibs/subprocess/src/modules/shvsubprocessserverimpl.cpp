/*
 *   Copyright (C) 2008-2010 by Lars Eriksen
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
#include "../../../../include/platformspc.h"

#include "shvsubprocessserverimpl.h"
#include "shvsubprocessimpl.h"


//=========================================================================================================
// SHVSubProcessServerImpl class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVSubProcessServerImpl::SHVSubProcessServerImpl(SHVModuleList& modules) : SHVSubProcessServer(modules)
{
#ifdef __SHIVA_WIN32
	if (Kernel32.Load(_S("kernel32")))
		Kernel32.Resolve((void**)&CancelIoEx, _S("CancelIoEx"));
	else
		CancelIoEx = NULL;
	Running = true;
	hThreadSignaller = CreateEvent(NULL, FALSE, FALSE, NULL);
	ActiveProcessLock.Lock();
	ActiveProcessMonitorThread.Start(this, &SHVSubProcessServerImpl::ActiveProcessMonitor);
#endif
}

/*************************************
 * Destructor
 *************************************/
SHVSubProcessServerImpl::~SHVSubProcessServerImpl()
{
#ifdef __SHIVA_WIN32
	SHVASSERT(ActiveProcesses.GetCount() == 0);

	Running = false;
	SetEvent(hThreadSignaller);
	while (ActiveProcessMonitorThread.IsRunning())
		SHVThreadBase::Sleep(10);
	CloseHandle(hThreadSignaller);
#endif
}

/*************************************
 * Register
 *************************************/
SHVBool SHVSubProcessServerImpl::Register()
{
	return SHVSubProcessServer::Register();
}

/*************************************
 * PostRegister
 *************************************/
void SHVSubProcessServerImpl::PostRegister()
{
	SHVSubProcessServer::PostRegister();
}

/*************************************
 * Unregister
 *************************************/
void SHVSubProcessServerImpl::Unregister()
{
	SHVSubProcessServer::Unregister();
}

/*************************************
 * New
 *************************************/
SHVSubProcess* SHVSubProcessServerImpl::New()
{
	return new SHVSubProcessImpl(*this);
}

#ifdef __SHIVA_WIN32
void SHVSubProcessServerImpl::ActiveProcessMonitor()
{
HANDLE* handles;
int handleCount, handleSize;

	handleCount = 1;
	handleSize = 10;

	handles = (HANDLE*)malloc(sizeof(HANDLE)*handleSize);
	handles[0] = hThreadSignaller;

	ActiveProcessLock.Unlock();

	while (Running)
	{
	DWORD dResult = WaitForMultipleObjects(handleCount, handles, FALSE, INFINITE);
	SHVListPos pos;

		if (dResult == 0 + WAIT_OBJECT_0) // The signal
		{
		int i;
			ActiveProcessLock.Lock();
			// First close all the closing process threads
			for (pos = ActiveProcesses.GetHeadPosition(); pos;)
			{
				if (ActiveProcesses.GetAt(pos).Closing && ActiveProcesses.GetAt(pos).Owner)
				{
					if (!ActiveProcesses.GetAt(pos).Owner->IsRunning())
					{
						ActiveProcesses.GetAt(pos).Owner->WaitForTermination();
						ActiveProcesses.GetAt(pos).Owner = NULL;
					}
				}
				if (!ActiveProcesses.GetAt(pos).Owner)
				{
					for (i = 1; i < handleCount; i++)
					{
						if (handles[i] == ActiveProcesses.GetAt(pos).hListenProcess)
						{
							::CloseHandle(handles[i]);
							if (i < handleCount - 1)
							{
								handles[i] = handles[handleCount - 1];
							}
							handleCount--;
						}
					}
					ActiveProcesses.RemoveAt(pos);
				}
				else
				{
					ActiveProcesses.MoveNext(pos);
				}
			}
			// Finally add any new handles
			for (pos = NULL; ActiveProcesses.MoveNext(pos);)
			{
				if (ActiveProcesses.GetAt(pos).hListenProcess == INVALID_HANDLE_VALUE)
				{
					if (handleCount == handleSize)
					{
					HANDLE* oldHandles = handles;
					int oldHandleSize = handleSize;
						handleSize += 10;
						handles = (HANDLE*)malloc(sizeof(HANDLE)*handleSize);
						memcpy(handles, oldHandles, sizeof(HANDLE)*oldHandleSize);
						free(oldHandles);
					}
					::DuplicateHandle(GetCurrentProcess(), ActiveProcesses.GetAt(pos).hProcess, GetCurrentProcess(), &handles[handleCount], 0, FALSE, DUPLICATE_SAME_ACCESS);
					ActiveProcesses.GetAt(pos).hListenProcess = handles[handleCount];
					handleCount++;
				}
			}
			ActiveProcessLock.Unlock();
		}
		else
		{
		int idx = dResult - WAIT_OBJECT_0;
		bool found = false;
			if (idx > 0 && idx < handleCount)
			{
				ActiveProcessLock.Lock();
				for (pos = NULL; !found && ActiveProcesses.MoveNext(pos);)
				{
					if (ActiveProcesses.GetAt(pos).hListenProcess == handles[idx])
					{
						::CloseHandle(handles[idx]);
						found = true;

						if (idx < handleCount - 1)
						{
							handles[idx] = handles[handleCount-1];
						}
						handleCount--;
						ActiveProcesses.GetAt(pos).hListenProcess = INVALID_HANDLE_VALUE;
						ActiveProcesses.GetAt(pos).Closing = true;

						if (ActiveProcesses.GetAt(pos).Owner)
						{
							if (!ActiveProcesses.GetAt(pos).Owner->IsRunning())
							{
								ActiveProcesses.GetAt(pos).Owner->WaitForTermination();
								ActiveProcesses.GetAt(pos).Owner = NULL;
							}
						}
					}
				}
				ActiveProcessLock.Unlock();
			}
		}
	}

	///\todo free all handles

	free(handles);
}

void SHVSubProcessServerImpl::AddActiveProcess(HANDLE hProcess, SHVSubProcessImpl* owner)
{
	if (CancelIoEx)
	{
		SHVASSERT(ActiveProcessMonitorThread.IsRunning());

		ActiveProcessLock.Lock();
		ActiveProcesses.AddTail(ActiveProcess(hProcess, owner));
		ActiveProcessLock.Unlock();

		SetEvent(hThreadSignaller);
	}
}
void SHVSubProcessServerImpl::RemoveActiveProcess(HANDLE /*hProcess*/, SHVSubProcessImpl* owner)
{
	if (CancelIoEx && SHVThreadBase::GetCurrentThreadID() != ActiveProcessMonitorThread.GetThreadID())
	{
		bool found = false;

		SHVASSERT(ActiveProcessMonitorThread.IsRunning());

		ActiveProcessLock.Lock();
		for (SHVListPos pos = NULL; !found && ActiveProcesses.MoveNext(pos);)
		{
			if (ActiveProcesses.GetAt(pos).Owner == owner)
			{
				ActiveProcesses.GetAt(pos).Owner = NULL;
				found = true;
			}
		}
		ActiveProcessLock.Unlock();

		SetEvent(hThreadSignaller);
	}
}

#endif
