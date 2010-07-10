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

#include "../../../include/frameworkimpl/shvmainthreadeventdispatchergeneric.h"
#include "../../../include/framework/shveventsubscriber.h"
#include "../../../include/framework/shvconsole.h"
#include "../../../include/utils/shvdir.h"



//=========================================================================================================
// SHVMainThreadEventQueueImpl class - OS specific main thread event queue
//=========================================================================================================


/*************************************
 * Constructor
 *************************************/
SHVMainThreadEventDispatcherGeneric::SHVMainThreadEventDispatcherGeneric() : SHVMainThreadEventDispatcher()
{
}

/*************************************
 * Destructor
 *************************************/
SHVMainThreadEventDispatcherGeneric::~SHVMainThreadEventDispatcherGeneric()
{
}

/*************************************
 * SetupDefaults
 *************************************/
void SHVMainThreadEventDispatcherGeneric::SetupDefaults(SHVModuleList& modules)
{
	SHVUNUSED_PARAM(modules);
}

/*************************************
 * SignalDispatcher
 *************************************/
void SHVMainThreadEventDispatcherGeneric::SignalDispatcher()
{
	Signal.Unlock();
}

/*************************************
 * InitializeEventLoop
 *************************************/
SHVBool SHVMainThreadEventDispatcherGeneric::InitializeEventLoop()
{
	Signal.Lock();
	return SHVBool::True;
}

/*************************************
 * RunEventLoop
 *************************************/
void SHVMainThreadEventDispatcherGeneric::RunEventLoop()
{
	while (Running())
	{
		Signal.Lock();
		DispatchEvents();
	}
}

/*************************************
 * StopEventLoop
 *************************************/
void SHVMainThreadEventDispatcherGeneric::StopEventLoop(SHVBool errors)
{
	Signal.Unlock();
	if (!errors)
	{
	SHVString errStr = Queue->GetModuleList().GetStartupErrors();
		SHVConsole::ErrPrintf(_S("\n\nRegistering failed:\n\"%s\"\n\n"), errStr.GetSafeBuffer());
	}
}

/*************************************
 * OnEvent
 *************************************/
void SHVMainThreadEventDispatcherGeneric::OnEvent(SHVEvent* event)
{
	SHVUNUSED_PARAM(event);
}
