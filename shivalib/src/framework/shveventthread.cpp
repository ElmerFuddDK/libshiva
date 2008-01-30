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

#include "../../../include/framework/shveventthread.h"



//=========================================================================================================
// SHVEventThread class - Implementation of a simple event thread
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVEventThread::SHVEventThread()
{
}

/*************************************
 * Destructor
 *************************************/
SHVEventThread::~SHVEventThread()
{
	WaitForTermination();
}

/*************************************
 * StartThread
 *************************************/
/// Starts the event thread
/**
 \param modules Reference to the module list
 *
 * IMPORTANT - Always call this function before usage. Eg. before or during
 * register of a module.
 */
void SHVEventThread::StartThread(SHVModuleList& modules)
{
	if (!EventThread.IsRunning())
	{
		Modules = &modules;
		TimeoutInterval = SHVMutexBase::Infinite;
		KillSignal = false;
		ThreadSignal.Lock(); // acquire signal before starting the thread - yay
		EventThread.Start(this,&SHVEventThread::ThreadFunc);
	}
}

/*************************************
 * StopThread
 *************************************/
/// Stops the thread
/**
 * Call this function to signal termination of the thread. This is
 * generally a good idea to do on predestroy if you have a worker
 * thread.
 */
void SHVEventThread::StopThread()
{
	if (EventThread.IsRunning())
	{
		KillSignal = true;
		SignalDispatcher();
	}
}

/*************************************
 * WaitForTermination
 *************************************/
/// Waits for the thread to terminate
/**
 * Will ensure the killsignal is in place and wait for the thread to
 * terminate.
 */
void SHVEventThread::WaitForTermination()
{
	StopThread();

	while (EventThread.IsRunning())
	{
		KillSignal = true;
		SignalDispatcher();
		SHVThreadBase::Sleep(40);
	}
}


/*************************************
 * EnqueueEVent
 *************************************/
void SHVEventThread::EnqueueEvent(SHVEvent* event, SHVEventSubscriberBase* subscriber)
{
	EventList.EnqueueEvent(*Modules,event,subscriber);
}

/*************************************
 * SignalDispatcher
 *************************************/
void SHVEventThread::SignalDispatcher()
{
	ThreadSignal.Unlock();
}

/*************************************
 * GetThreadID
 *************************************/
SHVThreadBase::ThreadID SHVEventThread::GetThreadID()
{
	return EventThread.GetThreadID();
}

/*************************************
 * LockEvent
 *************************************/
SHVBool SHVEventThread::LockEvent()
{
	return EventList.LockEvent();
}

/*************************************
 * UnlockEvent
 *************************************/
void SHVEventThread::UnlockEvent()
{
	EventList.UnlockEvent();
}

/*************************************
 * SetSignalTimeout
 *************************************/
/// Sets the timeout value for the signal wait
/**
 \param interval Interval in milliseconds
 *
 * Call this function if you want to get the Pre/Post-EventDispatch
 * functions called regularly.\n
 * It should be noted that this function will not signal the thread
 * in order to get the timeout set to the given value. You will need
 * to do that manually with SignalDispatcher .
 */
void SHVEventThread::SetSignalTimeout(int interval)
{
	TimeoutInterval = interval;
}


/*************************************
 * ThreadStarting
 *************************************/
/// Is called when starting - for inheritance
void SHVEventThread::ThreadStarting()
{}

/*************************************
 * ThreadStopping
 *************************************/
/// Is called when stopping - for inheritance
void SHVEventThread::ThreadStopping()
{}

/*************************************
 * PreEventDispatch
 *************************************/
/// Is called before dispatching - for inheritance
void SHVEventThread::PreEventDispatch()
{}

/*************************************
 * PostEventDispatch
 *************************************/
/// Is called after dispatching - for inheritance
void SHVEventThread::PostEventDispatch()
{}


///\cond INTERNAL
/*************************************
 * ThreadFunc
 *************************************/
void SHVEventThread::ThreadFunc()
{
	ThreadStarting();

	while (!KillSignal)
	{
		ThreadSignal.Lock(TimeoutInterval); // wait for signal

		PreEventDispatch();
		EventList.DispatchEvents(*Modules);
		PostEventDispatch();
	}

	ThreadStopping();

	EventList.Clear();
}
///\endcond
