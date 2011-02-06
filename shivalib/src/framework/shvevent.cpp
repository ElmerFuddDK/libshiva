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

#include "../../../include/framework/shvevent.h"
#include "../../../include/framework/shvmodulelist.h"


//=========================================================================================================
// SHVEvent class - Contains implementation of a basic event
//=========================================================================================================
/// \class SHVEventTarget shvevent.h "shiva/include/framework/shvevent.h"
/// \class SHVEvent shvevent.h "shiva/include/framework/shvevent.h"

/*************************************
 * Constructor
 *************************************/
SHVEvent::SHVEvent(SHVEventTarget* caller, SHVInt id, SHVInt subid, SHVRefObject* obj)
	: Caller(caller), ID(id), SubID(subid), Object(obj)
{
}

/*************************************
 * GetType
 *************************************/
SHVEvent::Types SHVEvent::GetType() const
{
	return SHVEvent::TypeBase;
}

/*************************************
  * GetData
 *************************************/
/// Gets a data pointer to a data block attached to the event
/**
 \return NULL or a data block pointer if present
 *
 * ADVANCED usage only.\n
 * This method is meant to be used in classes that inherit from either
 * SHVEvent or SHVEventString, and is a generic way to query additional
 * data attached to an event. The class that inherits from SHVEvent will
 * need to override this method and return a pointer to the contained
 * data.
 */
void* SHVEvent::GetData()
{
	return NULL;
}


//=========================================================================================================
// SHVEventQueue class - Interface class for event queues
//=========================================================================================================
/// \class SHVEventQueue shvevent.h "shiva/include/framework/shvevent.h"

/*************************************
 * SHVEventEventActiveInQueue
 *************************************/
/// tell the module list there is an event active
/**
 \param modules Reference to the module list
 \return true if the event system is active (the list is registered)
 *
 * INTERNAL function for implementing event queues. If you use the
 * builtin SHVEventQueueList then you don't need this function.\n
 * Use this function to validate that you can insert an event into
 * the queue.\n
 * Use the counterpart EventDeactivatedInQueue when the event has been
 * dispatched:
\code
// Example for enqueueing
void SHVMyEventQ::EnqueueEvent(SHVEvent* event, SHVEventSubscriberBase* subscriber)
{
	if (LockEvent())
	{
		if (EventActiveInQueue(Modules))
		{
			// Insert code here for enqueueing
		}
		UnlockEvent();
	}
}

// Example for performing the event from the queue
void SHVMyEventQ::PerformEvent(SHVEvent* event, SHVEventSubscriberBase* subscriber)
{
	subscriber->Perform(event);
	EventDeactivatedInQueue(Modules); // always call after perform
}
\endcode
 */
SHVBool SHVEventQueue::EventActiveInQueue(SHVModuleList& modules)
{
	return modules.EventActiveInQueue();
}

/*************************************
 * EventDeactivatedInQueue
 *************************************/
/// tell the module list that a previously enqueued event has been performed
/**
 \param modules Reference to the module list
 \see SHVEventQueue::EventActiveInQueue
*/
void SHVEventQueue::EventDeactivatedInQueue(SHVModuleList& modules)
{
	modules.EventDeactivatedInQueue();
}
