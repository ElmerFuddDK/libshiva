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

#include "../../../include/framework/shveventqueuelist.h"


//=========================================================================================================
// SHVEventQueueList class - contains the logic for handling event queues
//=========================================================================================================
/// \class SHVEventQueueList shveventqueuelist.h "shiva/include/framework/shveventqueuelist.h"

/*************************************
 * Constructor
 *************************************/
SHVEventQueueList::SHVEventQueueList()
{
	NeedDispatching = IsDispatching = false;
}

/*************************************
 * Destructor
 *************************************/
SHVEventQueueList::~SHVEventQueueList()
{
}

/*************************************
 * EnqueueEvent
 *************************************/
void SHVEventQueueList::EnqueueEvent(SHVModuleList& modules, SHVEvent* event, SHVEventSubscriberBase* subscriber)
{
	EventLock.Lock();
	if (modules.EventActiveInQueue())
	{
	EventEntry* entry = new EventEntry(event,subscriber);
		EventList.AddTail(entry);
	}
	else
	{
		event->ValidateRefCount();
		subscriber->ValidateRefCount();
	}
	EventLock.Unlock();
}

/*************************************
 * DispatchEvents
 *************************************/
void SHVEventQueueList::DispatchEvents(SHVModuleList& modules)
{
	if (!IsDispatching)
	{
	SHVList<EventEntryPtr,EventEntry*> localList;
	EventEntryPtr entry;

		IsDispatching = true;

		do
		{
			NeedDispatching = false;

			EventLock.Lock();
			while (EventList.GetCount())
				localList.AddTail(EventList.PopHead().ReleaseReference());
			EventLock.Unlock();

			// dispatch events
			while (localList.GetCount())
			{
				entry = localList.PopHead().ReleaseReference();
				PerformEvent(modules,entry);
			}
			entry = NULL;
		}
		while (NeedDispatching);

		IsDispatching = false;
	}
	else
	{
		NeedDispatching = true;
	}
}

/*************************************
 * ClearList
 *************************************/
void SHVEventQueueList::Clear(SHVModuleList& modules)
{
	EventLock.Lock();
	while (EventList.GetCount())
	{
		EventList.RemoveHead();
		modules.EventDeactivatedInQueue();
	}
	EventLock.Unlock();
}

/*************************************
 * PendingEvents
 *************************************/
bool SHVEventQueueList::PendingEvents()
{
	return ( EventList.GetCount() ? true : false );
}

/*************************************
 * LockEvent
 *************************************/
bool SHVEventQueueList::LockEvent()
{
	return EventLock.Lock();
}

/*************************************
 * UnlockEvent
 *************************************/
void SHVEventQueueList::UnlockEvent()
{
	EventLock.Unlock();
}

/*************************************
 * PerformEvent
 *************************************/
void SHVEventQueueList::PerformEvent(SHVModuleList& modules, EventEntryPtr& entry)
{
	entry->Subscriber->Perform(entry->Event);
	modules.EventDeactivatedInQueue();
}
