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

#include "../../../include/frameworkimpl/shvmainthreadeventqueue.h"
#include "../../../include/framework/shvmainthreadeventdispatcher.h"

#include "../../../include/framework/shvconsole.h"
#include "../../../include/utils/shvdir.h"


//-=========================================================================================================
// SHVMainThreadEventQueue class - Interface for the main event queue
//-=========================================================================================================

/*************************************
 * Constructor
 *************************************/
/// Constructor
SHVMainThreadEventQueue::SHVMainThreadEventQueue(SHVMainThreadEventDispatcher* dispatcher) : Modules(*this)
{
bool resolvedAppPathAndName = false;
	Dispatcher = dispatcher;
	Dispatcher->SetEventQueue(this);
	RunReturnVal = Modules.Initialize();

#ifdef __SHIVA_WIN32
	{
	SHVString moduleFileName, appPath, appName;
	long i;


		// Set up application path and name
		moduleFileName.SetBufferSize(_MAX_PATH);

		SHVVERIFY(::GetModuleFileName(NULL,(TCHAR*)moduleFileName.GetBuffer(),_MAX_PATH));
		appPath = SHVDir::ExtractPath(moduleFileName);
		appName = SHVDir::ExtractName(moduleFileName);

		i = appName.ReverseFind(_S("."));

		if (i > 0)
			appName[i] = 0;

		Modules.GetConfig().Set(SHVModuleList::DefaultCfgAppPath,appPath);
 		Modules.GetConfig().Set(SHVModuleList::DefaultCfgAppName,appName);
		
		resolvedAppPathAndName = true;
	}
#elif defined(__SHIVA_LINUX)
	{
	SHVString8 moduleFileName;
	long pathLen = 128;
	long i;
	
		// Set up application path and name
		moduleFileName.SetBufferSize(pathLen);

		i = readlink(SHVString8C::Format("/proc/%d/exe",getpid()).GetSafeBuffer(), moduleFileName.GetBuffer(), pathLen);
		
		while (i >= pathLen)
		{
			pathLen += 128;
			moduleFileName.SetBufferSize(pathLen);
			i = readlink(SHVString8C::Format("/proc/%d/exe",getpid()).GetSafeBuffer(), moduleFileName.GetBuffer(), pathLen);
		}
		
		if (i < 0 || i >= pathLen)
		{
			SHVConsole::ErrPrintf(_S("Error resolving app path and name\n"));
		}
		else
		{
		SHVString appPath, appName;
			
			moduleFileName.GetBuffer()[i] = '\0';
			
			appPath = SHVDir::ExtractPath(moduleFileName).ToStrT();
			appName = SHVDir::ExtractName(moduleFileName).ToStrT();

			Modules.GetConfig().Set(SHVModuleList::DefaultCfgAppPath,appPath);
			Modules.GetConfig().Set(SHVModuleList::DefaultCfgAppName,appName);

			resolvedAppPathAndName = true;
		}
	}
#endif

	if (!resolvedAppPathAndName)
	{
		// Setting app path and app name to some default nonsense
		Modules.GetConfig().Set(SHVModuleList::DefaultCfgAppPath,SHVStringC(_S(".")) + SHVDir::Delimiter());
		Modules.GetConfig().Set(SHVModuleList::DefaultCfgAppName,SHVStringC(_S("")));
	}

	Dispatcher->SetupDefaults(Modules);
	Running = SHVBool::False;
}

/*************************************
 * SHVMainThreadEventQueue
 *************************************/
SHVMainThreadEventQueue::~SHVMainThreadEventQueue()
{
	delete Dispatcher;
}

/*************************************
 * Run
 *************************************/
/// Runs the application
/**
 * This method will run the application by first calling InitializeEventLoop to initialize
 * platformspecific data before starting. After this the event queue should be considered
 * active.\n
 * It starts the module list, wich activates the event system. Upon success the event loop
 * Is started by calling the pure virtual RunEventLoop.\n
 * When the event queue exits then the application is finished, and SHVModule::Destroy have
 * been called on all modules. It then cleans up and returns the RunReturnVal that is either
 * true or an error message from any of the above.
 */
SHVBool SHVMainThreadEventQueue::Run()
{
	RunReturnVal = SHVBool::True;
	ThreadID = SHVThreadBase::GetCurrentThreadID();
	
	// initialize data for the event loop
	RunReturnVal = Dispatcher->InitializeEventLoop();

	SHVASSERT(GetModuleList().GetConfig().Contains(SHVModuleList::DefaultCfgAppPath));
	SHVASSERT(GetModuleList().GetConfig().Contains(SHVModuleList::DefaultCfgAppName));
	
	if (RunReturnVal)
		RunReturnVal = GetModuleList().Start();
	
	if (RunReturnVal)
	{
		Running = SHVBool::True;
	
		// Now for the main event! ... loop
		Dispatcher->RunEventLoop();

		RunReturnVal = GetModuleList().GetReturnError();

		if (RunReturnVal && !GetModuleList().GetStartupErrors().IsEmpty())
			RunReturnVal = SHVBool::False;
	}

	GetModuleList().DestroyModules();

	return RunReturnVal;
}


/*************************************
 * EnqueueEvent
 *************************************/
void SHVMainThreadEventQueue::EnqueueEvent(SHVEvent* event, SHVEventSubscriberBase* subscriber)
{
	EventList.EnqueueEvent(Modules,event,subscriber);
}

/*************************************
 * SignalDispatcher
 *************************************/
void SHVMainThreadEventQueue::SignalDispatcher()
{
	Dispatcher->SignalDispatcher();
}

/*************************************
 * GetThreadID
 *************************************/
SHVThreadBase::ThreadID SHVMainThreadEventQueue::GetThreadID()
{
	return ThreadID;
}

/*************************************
 * LockEvent
 *************************************/
SHVBool SHVMainThreadEventQueue::LockEvent()
{
	return EventList.LockEvent();
}

/*************************************
 * UnlockEvent
 *************************************/
void SHVMainThreadEventQueue::UnlockEvent()
{
	EventList.UnlockEvent();
}

/*************************************
 * OnEvent
 *************************************/
void SHVMainThreadEventQueue::OnEvent(SHVEvent* event)
{
	if (SHVEvent::Equals(event,SHVMainThreadEventQueue::EventInternalStop))
	{
		Running = SHVBool::False;
		Dispatcher->StopEventLoop(event->GetSubID() ? SHVBool::True : SHVBool::False);
	}
}
