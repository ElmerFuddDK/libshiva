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

#include "../../../include/framework/shveventthreadpool.h"



//=========================================================================================================
// SHVEventThreadPool class
//=========================================================================================================
/// \class SHVEventThreadPool shveventthreadpool.h "shiva/include/framework/shveventthreadpool.h"

/*************************************
 * Constructor
 *************************************/
SHVEventThreadPool::SHVEventThreadPool(SHVEventSubscriberBase* stateSubscriber) : EventList(this), StateSubscriber(stateSubscriber)
{
}

/*************************************
 * Destructor
 *************************************/
SHVEventThreadPool::~SHVEventThreadPool()
{
	WaitForTermination();
}

/*************************************
 * SetThreadStartingSubscriber
 *************************************/
/// Set the thread starting subscriber
/**
 \param subs Subscriber to be emitted on when a new thread is started
 *
 * If you use a direct subscriber then you can use this method to initialize
 * thread specific data once a new thread is started. There is no equivalent
 * thread stop subscriber as threads are currently not being harvested again
 * after being started.
 */
void SHVEventThreadPool::SetThreadStartingSubscriber(SHVEventSubscriberBase* subs)
{
SHVBool locked = LockEvent();
	ThreadStartingSubscriber = subs;
	if (locked)
		UnlockEvent();
}

/*************************************
 * Start
 *************************************/
/// Starts the event thread pool
/**
 \param modules Reference to the module list
 \param maxCount Maximum thread pool count
 \param threadPriority Priority of all the threads
 *
 * IMPORTANT - Always call this function before usage. Eg. before or during
 * register of a module.
 */
void SHVEventThreadPool::Start(SHVModuleList& modules, int maxCount, short threadPriority)
{
	if (!EventThread.IsRunning())
	{
		Modules = &modules;
		TimeoutInterval = SHVMutexBase::Infinite;
		KillSignal = false;
		ThreadPool.SetThreadStartingFunction(this,&SHVEventThreadPool::PoolThreadStarting);
		ThreadPool.Start(0,maxCount,threadPriority);
		ThreadSignal.Lock(); // acquire signal before starting the thread - yay
		EventThread.Start(this,&SHVEventThreadPool::ThreadFunc,threadPriority);
	}
}

/*************************************
 * StopThread
 *************************************/
/// Stops the thread
/**
 * Call this function to signal termination of the thread pool. This
 * is generally a good idea to do on preunregister.
 */
void SHVEventThreadPool::Stop()
{
	if (EventThread.IsRunning())
	{
		KillSignal = true;
		SignalDispatcher();
	}
	StateSubscriber = NULL;
	ThreadStartingSubscriber = NULL;
}

/*************************************
 * WaitForTermination
 *************************************/
/// Waits for the thread pool to terminate
/**
 * Will ensure the killsignal is in place and wait for the thread to
 * terminate.
 */
void SHVEventThreadPool::WaitForTermination()
{
	Stop();

	while (EventThread.IsRunning())
	{
		KillSignal = true;
		SignalDispatcher();
		SHVThreadBase::Sleep(40);
	}
	ThreadPool.Stop();
}


/*************************************
 * EnqueueEVent
 *************************************/
void SHVEventThreadPool::EnqueueEvent(SHVEvent* event, SHVEventSubscriberBase* subscriber)
{
	EventList.EnqueueEvent(*Modules,event,subscriber);
}

/*************************************
 * SignalDispatcher
 *************************************/
void SHVEventThreadPool::SignalDispatcher()
{
	ThreadSignal.Unlock();
}

/*************************************
 * GetThreadID
 *************************************/
SHVThreadBase::ThreadID SHVEventThreadPool::GetThreadID()
{
	return SHVThreadBase::InvalidThreadID;
}

/*************************************
 * LockEvent
 *************************************/
SHVBool SHVEventThreadPool::LockEvent()
{
	return EventList.LockEvent();
}

/*************************************
 * UnlockEvent
 *************************************/
void SHVEventThreadPool::UnlockEvent()
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
void SHVEventThreadPool::SetSignalTimeout(int interval)
{
	TimeoutInterval = interval;
}


/*************************************
 * ThreadStarting
 *************************************/
/// Is called when starting - for inheritance
void SHVEventThreadPool::ThreadStarting()
{
	if (!StateSubscriber.IsNull())
	{
		StateSubscriber->EmitNow(*Modules,new SHVEvent(NULL,EventStateStarting));
	}
}

/*************************************
 * ThreadStopping
 *************************************/
/// Is called when stopping - for inheritance
void SHVEventThreadPool::ThreadStopping()
{
	if (!StateSubscriber.IsNull())
	{
		StateSubscriber->EmitNow(*Modules,new SHVEvent(NULL,EventStateStopping));
	}
}

/*************************************
 * PreEventDispatch
 *************************************/
/// Is called before dispatching - for inheritance
void SHVEventThreadPool::PreEventDispatch()
{}

/*************************************
 * PostEventDispatch
 *************************************/
/// Is called after dispatching - for inheritance
void SHVEventThreadPool::PostEventDispatch()
{}


///\cond INTERNAL
/*************************************
 * ThreadFunc
 *************************************/
void SHVEventThreadPool::ThreadFunc()
{
	ThreadStarting();

	while (!KillSignal)
	{
		ThreadSignal.Lock(TimeoutInterval); // wait for signal
		if (EventActiveInQueue(*GetModuleList()))
		{
			PreEventDispatch();
			EventList.DispatchEvents(*Modules);
			PostEventDispatch();
			EventDeactivatedInQueue(*GetModuleList());
		}
		else
		{
			EventList.Clear(*Modules);
		}
	}

	ThreadStopping();

	EventList.Clear(*Modules);
}

/*************************************
 * PoolThreadStarting
 *************************************/
void SHVEventThreadPool::PoolThreadStarting()
{
SHVEventSubscriberBaseRef subs;

	if (LockEvent())
	{
		subs = ThreadStartingSubscriber;
		UnlockEvent();
	}

	if (!subs.IsNull())
		subs->EmitNow(*Modules,new SHVEvent(NULL,EventPoolThreadStarting));
}

/*************************************
 * PoolEmitEvent
 *************************************/
void SHVEventThreadPool::PoolEmitEvent(SHVEventQueueListThreadPool::EventEntry* entry)
{
	entry->Subscriber->Perform(entry->Event);
	EventList.EventDeactivatedInQueue(*Modules);
	delete entry;
}

/*************************************
 * PerformEvent
 *************************************/
void SHVEventQueueListThreadPool::PerformEvent(SHVModuleList& modules, EventEntryPtr& entry)
{
	if (Owner->ThreadPool.Execute(Owner,&SHVEventThreadPool::PoolEmitEvent,entry.ReleaseReference()) == SHVThreadBase::InvalidThreadID)
	{
		SHVASSERT(false);
		delete entry;
		EventDeactivatedInQueue(modules);
	}
}
///\endcond
