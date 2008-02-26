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

#include "../../../include/framework/shveventsubscriberdynamic.h"


//=========================================================================================================
/// SHVEventSubscriberDynamic class - Subscriber class for dynamic targets
//=========================================================================================================


/*************************************
 * Constructor
 *************************************/
SHVEventSubscriberDynamic::SHVEventSubscriberDynamic(SHVEventQueue* queue, bool directIfSameThread)
{
}

/*************************************
 * Destructor
 *************************************/
SHVEventSubscriberDynamic::~SHVEventSubscriberDynamic()
{
	Subscribers.Clear();
}

/*************************************
 * Subscribe
 *************************************/
void SHVEventSubscriberDynamic::Subscribe(SHVEventTargetDynamic* subscriber)
{
size_t i,count;

	count = Subscribers.CalculateCount();

	for (i=0; i<count && subscriber; i++)
	{
		if (Subscribers[i] == NULL)
		{
			Subscribers.Replace(i,subscriber);
			subscriber = NULL;
		}
	}

	if (subscriber)
		Subscribers.Add(subscriber,SubscriberGrowSize);
}

/*************************************
 * Unsubscribe
 *************************************/
void SHVEventSubscriberDynamic::Unsubscribe(SHVEventTargetDynamic* subscriber)
{
size_t i,count;

	count = Subscribers.CalculateCount();

	for (i=0; i<count; i++)
	{
		if (Subscribers[i] == subscriber)
			Subscribers.Replace(i,NULL);
	}

	Subscribers.Compress(SubscriberGrowSize);
}

/*************************************
 * ContainsSubscriber
 *************************************/
SHVBool SHVEventSubscriberDynamic::ContainsSubscriber(SHVEventTargetDynamic* subscriber)
{
size_t i,count;

	count = Subscribers.CalculateCount();

	for (i=0; i<count; i++)
	{
		if (Subscribers[i] == subscriber)
			break;
	}

	return (i<count);
}

/*************************************
 * Emit
 *************************************/
SHVEventQueue* SHVEventSubscriberDynamic::Emit(SHVModuleList& modules, SHVEvent* event)
{
SHVEventQueue* retVal = NULL;

	if (Subscribers.CalculateCount())
	{
		if ( Queue && (!DirectIfSameThread || SHVThreadBase::GetCurrentThreadID() != Queue->GetThreadID()) )
		{
			Queue->EnqueueEvent(event,this);
			retVal = Queue;
		}
		else
		{
			Perform(event);
		}
	}
	else
	{
		event->ValidateRefCount();
	}

	return retVal;
}

/*************************************
 * Perform
 *************************************/
void SHVEventSubscriberDynamic::Perform(SHVEvent* event)
{
size_t i,count;

	count = Subscribers.CalculateCount();

	for (i=0; i<count; i++)
	{
		if (Subscribers[i])
			((SHVEventTargetDynamic*)Subscribers[i])->OnEvent(event);
	}

	event->ValidateRefCount();
}
