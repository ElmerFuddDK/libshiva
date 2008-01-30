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

#include "../../../include/framework/shveventemitter.h"


//=========================================================================================================
// SHVEventEmitter class - Used for event registering and emitting events
//=========================================================================================================


/*************************************
 * Constructor
 *************************************/
SHVEventEmitter::SHVEventEmitter()
{
}

/*************************************
 * EventSubscribe
 *************************************/
/// Registers an event if it doesn't exist, and subscribes to it
void SHVEventEmitter::EventSubscribe(SHVEvent* event, SHVEventSubscriberBase* subscriber)
{
SHVEventSubscriptionList* list = Find(event);

	if (!list)
	{
		list = new SHVEventSubscriptionList(event);
		EventSubscriptions.AddTail(list);
	}
	else if (event)
		event->ValidateRefCount();

	if (subscriber)
		list->EventSubscribe(subscriber);
}

/*************************************
 * EmitEvent
 *************************************/
/// emit an event
/**
 \param modules Reference to the module list
 \param event Event to emit
 \return true if it was registered, and there were subscribers
 */
bool SHVEventEmitter::EmitEvent(SHVModuleList& modules, SHVEvent* event)
{
SHVEventSubscriptionList* list = Find(event);
bool retVal = (list ? true : false);

	if (retVal)
	{
		retVal = list->EmitEvent(modules,event);
	}
	else if (event)
	{
		event->ValidateRefCount();
	}


	return retVal;
}

/*************************************
 * Clear
 *************************************/
/// Clear the subscriptions
/**
 * Use this function to clear the subscriptions during unregister.
 */
void SHVEventEmitter::Clear()
{
	EventSubscriptions.RemoveAll();
}


///\cond INTERNAL
/*************************************
 * Find
 *************************************/
SHVEventSubscriptionList* SHVEventEmitter::Find(SHVEvent* event)
{
SHVEventSubscriptionList* retVal = NULL;
SHVListIterator<SHVEventSubscriptionListPtr,SHVEventSubscriptionList*> itr(EventSubscriptions);

	while (!retVal && itr.MoveNext())
	{
		if (*itr.Get() == event)
			retVal = itr.Get();
	}

	return retVal;
}
///\endcond
