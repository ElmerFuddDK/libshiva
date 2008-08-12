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

#include "../../../include/frameworkimpl/shvmodulelistimpl.h"
#include "../../../include/frameworkimpl/shvmainthreadeventqueue.h"
#include "../../../include/framework/shvmodule.h"
#include "../../../include/utils/shvstring.h"

#include "../../../include/frameworkimpl/shvtimerimpl.h"

#define STUB

//=========================================================================================================
// SHVModuleListImpl class - implementation of module list
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVModuleListImpl::SHVModuleListImpl(SHVMainThreadEventQueue& defaultQueue)
	: DefaultEventQueue(defaultQueue)
{
	State = StateInitializing;

	UserConfigs.SetBucketCount(5);

	EventsActive = 0;
	TagSeed = 0;
}


/*************************************
 * Initialize
 *************************************/
SHVBool SHVModuleListImpl::Initialize()
{
SHVBool retVal(false);

	if (State == StateInitializing)
	{
		State = StateInitialized;
	}

	DestroyModules(); // cleans up

	return retVal;
}

/*************************************
 * Start
 *************************************/
SHVBool SHVModuleListImpl::Start()
{
SHVEventSubscriberBaseRef direct = new SHVEventSubscriber(this);
SHVEventSubscriberBaseRef queued = new SHVEventSubscriber(this,this);

	State = StateStarting;

	InternalSubscriber = new SHVEventSubscriberFunc<SHVModuleListImpl>(this,&SHVModuleListImpl::OnInternalEvent,this);

	EventSubscribe(__EVENT_GLOBAL_DELAYSHUTDOWN,direct);
	EventSubscribe(__EVENT_GLOBAL_DELAYSHUTDOWNFINISHED,queued);

	if (!(Timer = (SHVTimer*)ResolveModule("Timer")))
		AddModule(Timer = new SHVTimerImpl(*this));

	DefaultEventQueue.EnqueueEvent(new SHVEvent(this,EventInternalRegister),InternalSubscriber);

	DefaultEventQueue.SignalDispatcher();

	return true;
}

/*************************************
 * DestroyModules
 *************************************/
void SHVModuleListImpl::DestroyModules()
{
	if (State == StateInitialized)
	{
		Config.Clear();
		UserConfigs.Clear();
		Modules.RemoveAll();
		StartupErrors.RemoveAll();
		ShutdownDelays.RemoveAll();
	}
}

/*************************************
 * AddModule
 *************************************/
SHVBool SHVModuleListImpl::AddModule(SHVModuleBase* module)
{
SHVBool retVal(false);

	if (State == StateInitialized || State == StateStarting)
	{
		retVal = ( ResolveModule(module->GetName().GetBufferConst()) ? false : true );

		if (retVal)
			Modules.AddTail(module);
	}

	if (!retVal)
		delete module;

	return retVal;
}

/*************************************
 * ResolveModule
 *************************************/
SHVModuleBase* SHVModuleListImpl::ResolveModule(const SHVChar* name)
{
SHVInternalListIterator itr(Modules);
SHVModuleBase* retVal = NULL;

	while (!retVal && itr.MoveNext())
	{
		if (itr.Get()->GetName() == name)
			retVal = itr.Get();
	}

	return retVal;
}

/*************************************
 * GetIterator
 *************************************/
SHVModuleListIteratorPtr SHVModuleListImpl::GetIterator(const SHVChar* group)
{
	return new SHVModuleListIteratorImpl(Modules,group);
}

/*************************************
 * GetConfig
 *************************************/
SHVConfig& SHVModuleListImpl::GetConfig(int cfgID)
{
SHVConfig* retVal;

	if (cfgID == SHVModuleList::CfgDefault)
		retVal = &Config;
	else
	{
		Lock.Lock();
		if (!UserConfigs.Find(cfgID))
			UserConfigs[cfgID] = new SHVConfigImpl();
		retVal = (SHVConfig*)UserConfigs[cfgID];
		Lock.Unlock();
	}
	
	return *retVal;
}

/*************************************
 * CreateTag
 *************************************/
SHVInt SHVModuleListImpl::CreateTag()
{
	return TagSeed++;
}

/*************************************
 * EventSubscribe
 *************************************/
void SHVModuleListImpl::EventSubscribe(SHVEvent* event, SHVEventSubscriberBase* subscriber)
{
	Events.EventSubscribe(event,subscriber);
}

/*************************************
 * EmitEvent
 *************************************/
SHVBool SHVModuleListImpl::EmitEvent(SHVEvent* event)
{
	return Events.EmitEvent(*this,event);
}

/*************************************
 * AddStartupError
 *************************************/
SHVBool SHVModuleListImpl::AddStartupError(const SHVStringC& error)
{
	if (State == StateInitialized || State == StateRegistering)
	{
		StartupErrors.AddTail(error + SHVStringC(_T("\n")));
	}

	return SHVBool::False;
}

/*************************************
 * GetStartupErrors
 *************************************/
SHVStringBuffer SHVModuleListImpl::GetStartupErrors()
{
SHVString retVal;
SHVListIterator<SHVString,const SHVStringC> itr(StartupErrors);

	while (itr.MoveNext())
	{
		retVal += itr.Get();
	}

	return retVal.ReleaseBuffer();
}

/*************************************
 * IsRegistered
 *************************************/
SHVBool SHVModuleListImpl::IsRegistered()
{
	return (State >= StateRunning && State <= StateRunningPreUnreg);
}

/*************************************
 * IsDestroying
 *************************************/
SHVBool SHVModuleListImpl::IsUnregistering()
{
	return (State == StateUnregistering);
}

/*************************************
 * EnqueueEvent
 *************************************/
void SHVModuleListImpl::EnqueueEvent(SHVEvent* event, SHVEventSubscriberBase* subscriber)
{
	DefaultEventQueue.EnqueueEvent(event,subscriber);
}

/*************************************
 * SignalDispatcher
 *************************************/
void SHVModuleListImpl::SignalDispatcher()
{
	DefaultEventQueue.SignalDispatcher();
}

/*************************************
 * LockEvent
 *************************************/
SHVBool SHVModuleListImpl::LockEvent()
{
	return DefaultEventQueue.LockEvent();
}

/*************************************
 * UnlockEvent
 *************************************/
void SHVModuleListImpl::UnlockEvent()
{
	DefaultEventQueue.UnlockEvent();
}

/*************************************
 * CloseApp
 *************************************/
void SHVModuleListImpl::CloseApp()
{
	Lock.Lock();
	if (State == StateRunning)
	{
		State = StateRunningClosing;
		Lock.Unlock();
		
		EmitEvent(new SHVEvent(this,EventClosing));
		
		DelayShutdownFinished(NULL,NULL);
	}
	else
	{
		Lock.Unlock();
	}
}


/*************************************
 * GetThreadID
 *************************************/
SHVThreadBase::ThreadID SHVModuleListImpl::GetThreadID()
{
	return DefaultEventQueue.GetThreadID();
}

/*************************************
 * EventActiveInQueue
 *************************************/
SHVBool SHVModuleListImpl::EventActiveInQueue()
{
SHVBool retVal(true);

	Lock.Lock();

	if (IsRegistered() || (State > StateInitialized && State < StateUnregistering && SHVThreadBase::GetCurrentThreadID() == DefaultEventQueue.GetThreadID()))
		EventsActive++;
	else
		retVal = false;

	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * EventDeactivatedInQueue
 *************************************/
void SHVModuleListImpl::EventDeactivatedInQueue()
{
bool locked = Lock.Lock();
	EventsActive--;

	if (EventsActive <= 0)
	{
		if (State == StateUnregistering)
		{
			if (SHVThreadBase::GetCurrentThreadID() != DefaultEventQueue.GetThreadID())
			{
				// temporarily open for events so we can get the closing event to the main thread
				// this is ok since we can see there are no active events in the system, and we have the lock
				State = StateRunningClosing;
				DefaultEventQueue.EnqueueEvent(new SHVEvent(this,EventInternalUnregister),InternalSubscriber);
				State = StateUnregistering;
				locked = false;
				Lock.Unlock();

				DefaultEventQueue.SignalDispatcher();
			}
			else
			{
				locked = false;
				Lock.Unlock();

				UnregisterModules();
			}
		}
		else if (State == StateUnregistered)
		{

			if (SHVThreadBase::GetCurrentThreadID() != DefaultEventQueue.GetThreadID())
			{
				// temporarily open for events so we can get the closing event to the main thread
				// this is ok since we can see there are no active events in the system, and we have the lock
				State = StateRunningClosing;
				DefaultEventQueue.EnqueueEvent(new SHVEvent(this,EventInternalClose),InternalSubscriber);
				State = StateUnregistered;
				locked = false;
				Lock.Unlock();

				DefaultEventQueue.SignalDispatcher();
			}
			else
			{
			SHVEventRef event = new SHVEvent( &DefaultEventQueue, SHVMainThreadEventQueue::EventInternalStop, StartupErrors.GetCount() ? 0 : 1 );
			int locks;
				State = StateInitialized;
				Events.Clear();
				
				locks = Lock.UnlockAll();
				DefaultEventQueue.OnEvent(event);
				Lock.LockMultiple(locks);
				
				if (State == StateInitialized)
					Events.Clear();

			}
		}
	}

	if (locked)
	{
		Lock.Unlock();
	}
}

/*************************************
 * OnEvent
 *************************************/
void SHVModuleListImpl::OnEvent(SHVEvent* event)
{
	if (SHVEventString::Equals(event,__EVENT_GLOBAL_DELAYSHUTDOWN))
	{
		DelayShutdown(event);
	}
	else if (SHVEventString::Equals(event,__EVENT_GLOBAL_DELAYSHUTDOWNFINISHED))
	{
		DelayShutdownFinished(event->GetCaller(),NULL);
	}

	else if (event->GetCaller() == Timer)
	{
		if (SHVEvent::Equals(event,SHVTimer::EventTimeout))
		{
			DelayShutdownFinished(NULL,(SHVTimerInstance*)event->GetObject());
		}
	}
}

/*************************************
 * RegisterModules
 *************************************/
SHVBool SHVModuleListImpl::RegisterModules()
{
SHVBool retVal(false);

	if ( (State == StateStarting || State == StateRunning) && DefaultEventQueue.GetThreadID() == SHVThreadBase::GetCurrentThreadID() )
	{
	SHVList<SHVModuleBase*> registeredModules;
	SHVInternalListIterator itr(Modules);

		retVal = true;

		Lock.Lock();
	
		EmitEvent(new SHVEvent(this,SHVModuleList::EventPreRegister));
	
		State = StateRegistering;
	
		while (State == StateRegistering && itr.MoveNext())
		{
			if (!itr.Get()->IsRegistered())
			{
				if (itr.Get()->Register())
				{
					registeredModules.AddTail(itr.Get());
				}
				else
				{
				SHVString errMsg;

					errMsg.Format( _T("%s failed to register"), itr.Get()->GetName().ToStrT().GetSafeBuffer() );
					AddStartupError( errMsg );
					State = StateError;
				}
			}
		}
	
		if (StartupErrors.GetCount())
			State = StateError;


		if (State == StateRegistering)
			State = StateRunning;

		Lock.Unlock();

		if (State == StateRunning)
		{
			EmitEvent(new SHVEvent(this,SHVModuleList::EventEndRegister));
		}

		if (State == StateRunning)
		{
			while (registeredModules.GetCount())
				registeredModules.PopHead()->PostRegister();
		}
	
		if (State == StateRunning || State == StateRunningClosing)
		{
			EmitEvent(new SHVEvent(this,SHVModuleList::EventEndPostRegister));
		}

		// if there wasn't any errors set in the bool object, set it to false
		if (retVal && State != StateRunning && State != StateRunningClosing)
			retVal = false;
	}

	return retVal;
}

/*************************************
 * PreUnregisterModules
 *************************************/
void SHVModuleListImpl::PreUnregisterModules()
{
	Lock.Lock();

	if (State == StateRunningClosing || State == StateRunning)
	{
	SHVInternalListIterator itr(Modules);

		State = StateRunningPreUnreg;

		Lock.Unlock();

		EmitEvent(new SHVEvent(this,SHVModuleList::EventPreUnregister)); // for non-module objects
		while (itr.MoveNext())
			itr.Get()->PreUnregister();
	}
	else
	{
		Lock.Unlock();
	}

}

/*************************************
 * UnregisterModules
 *************************************/
void SHVModuleListImpl::UnregisterModules()
{
SHVInternalListIterator itr(Modules);

	Lock.Lock();

	EventsActive++;

	PreUnregisterModules(); // make sure the preunregister functions have been called

	if (State != StateUnregistered)
	{

		State = StateUnregistering;

		if (EventsActive <= 1) // only UnregisterModules are performing events
		{
			while (itr.MoveNext())
			{
				if (itr.Get()->IsRegistered())
					itr.Get()->Unregister();
			}

			State = StateUnregistered;
		}
	}

	EventDeactivatedInQueue();

	Lock.Unlock();
}

///\cond INTERNAL
/*************************************
 * DelayShutdown
 *************************************/
void SHVModuleListImpl::DelayShutdown(SHVEvent* event)
{
	Lock.Lock();
	if (State > StateRunning && State < StateUnregistering && event && event->GetCaller())
	{
	ShutdownDelay* delay = new ShutdownDelay;

		delay->Target = event->GetCaller();

		if (event->GetID() > 0)
		{
			delay->Timer  = Timer->CreateTimer(new SHVEventSubscriber(this,this));
			delay->Timer->Set(SHVTimerInstance::ModeOnce,event->GetID());
		}

		RemoveDelayedShutdowns(event->GetCaller(),NULL);

		ShutdownDelays.AddTail(delay);
	}
	Lock.Unlock();
}

/*************************************
 * RemoveDelayedShutdowns
 *************************************/
void SHVModuleListImpl::RemoveDelayedShutdowns(SHVEventTarget* target, SHVRefObject* timer)
{
	if (target || timer)
	{
	SHVListIterator<ShutdownDelayPtr,ShutdownDelay*> itr(ShutdownDelays);

		for (itr.MoveNext(); itr;)
		{
			if (target && itr.Get()->Target == target)
				ShutdownDelays.RemoveAt(itr.Pos());
			else if (timer && (SHVRefObject*)itr.Get()->Timer == timer)
				ShutdownDelays.RemoveAt(itr.Pos());
			else
				itr.MoveNext();
		}
	}
}

/*************************************
 * DelayShutdownFinished
 *************************************/
void SHVModuleListImpl::DelayShutdownFinished(SHVEventTarget* target, SHVRefObject* timer)
{
	Lock.Lock();
	RemoveDelayedShutdowns(target,timer);

	if (State > StateRunning && State < StateUnregistering && !ShutdownDelays.GetCount())
	{
		Lock.Unlock();
		DefaultEventQueue.EnqueueEvent(new SHVEvent(this,EventInternalUnregister),InternalSubscriber);
		DefaultEventQueue.SignalDispatcher();
	}
	else
	{
		Lock.Unlock();
	}
}

/*************************************
 * OnInternalEvent
 *************************************/
void SHVModuleListImpl::OnInternalEvent(SHVEvent* event)
{
	if (SHVEvent::Equals(event,EventInternalRegister))
	{
		if (!RegisterModules())
		{
			UnregisterModules();
		}
	}
	else if (SHVEvent::Equals(event,EventInternalUnregister))
	{
		if (State == StateRunningClosing)
		{
			PreUnregisterModules();
			DelayShutdownFinished(NULL,NULL);
		}
		else
			UnregisterModules();
	}
	else if (SHVEvent::Equals(event,EventInternalClose))
	{
		; // nothing - the deactivated thingy will do the rest
	}
}
///\endcond



//=========================================================================================================
// SHVModuleListIteratorImpl class - implementation of module list iterator
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVModuleListIteratorImpl::SHVModuleListIteratorImpl(SHVModuleListImpl::SHVInternalList& list, const SHVChar* group) : List(list), Group(group)
{
}

/*************************************
 * operator bool
 *************************************/
SHVModuleListIteratorImpl::operator SHVBool()
{
	return (bool)List;
}

/*************************************
 * MoveNext
 *************************************/
SHVBool SHVModuleListIteratorImpl::MoveNext()
{
bool retVal = false;

	while (!retVal && List)
	{
		if (List.MoveNext())
		{
			if (List.Get()->GetGroup() == Group)
				retVal = true;
		}
	}

	return retVal;
}

/*************************************
 * Get
 *************************************/
SHVModuleBase* SHVModuleListIteratorImpl::Get()
{
	return List.Get();
}
