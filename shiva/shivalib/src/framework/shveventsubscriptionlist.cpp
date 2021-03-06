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

#include "../../../include/framework/shveventsubscriptionlist.h"


//=========================================================================================================
// SHVEventSubscriptionList class - Contains a list of subscriptions for a specific event
//=========================================================================================================
/// \class SHVEventSubscriptionList shveventsubscriptionlist.h "shiva/include/framework/shveventsubscriptionlist.h"

/*************************************
 * Constructor
 *************************************/
SHVEventSubscriptionList::SHVEventSubscriptionList(SHVEvent* event) : Event(event)
{
}

/*************************************
 * EventSubscribe
 *************************************/
void SHVEventSubscriptionList::EventSubscribe(SHVEventSubscriberBase* subscriber)
{
	EventSubscribers.AddTail(subscriber);
}

/*************************************
 * GetEvent
 *************************************/
SHVEvent* SHVEventSubscriptionList::GetEvent()
{
	return Event;
}

/*************************************
 * operator==
 *************************************/
bool SHVEventSubscriptionList::operator==(const SHVEvent* event)
{
	if (!event || Event.IsNull())
		return false;

	if (event->GetType() != Event->GetType())
		return false;

	switch(event->GetType())
	{
	case SHVEvent::TypeBase:
		return SHVEvent::Equals(event,Event->GetID());
	case SHVEvent::TypeString:
		return SHVEventString::Equals(event,((SHVEventString*)(SHVEvent*)Event)->GetName().GetSafeBuffer());
	}

	return false;
}

/*************************************
 * EmitEvent
 *************************************/
bool SHVEventSubscriptionList::EmitEvent(SHVModuleList& modules, SHVEvent* event)
{
SHVEventRef eLock = event;
bool retVal = modules.EventActiveInQueue(); // Make sure the module list doesn't close whilst distributing the event

	if (retVal)
	{
	SHVEventQueue* queue;
	SHVList<SHVEventQueue*> queuelist;
	SHVListIterator<SHVEventSubscriberBaseRef,SHVEventSubscriberBase*> itr(EventSubscribers);
	SHVListIterator<SHVEventQueue*> queueItr(queuelist);
	bool found;


		// only return true if there was any subscribers
		retVal = ( EventSubscribers.GetCount() ? true : false );


		// emit the event
		while (itr.MoveNext())
		{
			queue = itr.Get()->Emit(modules,event);
	
			if (queue)
			{
				found = false;
	
				while (!found && queueItr.MoveNext())
					found = (queueItr.Get() == queue);
	
				if (!found)
					queuelist.AddTail(queue);
				else
					queueItr.Pos() = NULL; // reset the iterator
			}
		}

		// signal event queues
		while (queueItr.MoveNext())
			queueItr.Get()->SignalDispatcher();

		modules.EventDeactivatedInQueue(); // Now we are done
	}

	return retVal;
}
