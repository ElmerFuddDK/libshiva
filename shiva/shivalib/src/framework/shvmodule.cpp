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

#include "../../../include/framework/shvmodule.h"


//=========================================================================================================
// SHVModule class - base implementation of a module
//=========================================================================================================
/// \class SHVModule shvmodule.h "shiva/include/framework/shvmodule.h"

/*************************************
 * Constructor
 *************************************/
SHVModule::SHVModule(SHVModuleList& modules, const SHVChar* name, const SHVChar* group) : SHVModuleBase(name,group), Modules(modules)
{
	Registered = false;
}

/*************************************
 * Register
 *************************************/
SHVBool SHVModule::Register()
{
	Registered = true;
	return true;
}

/*************************************
 * PostRegister
 *************************************/
void SHVModule::PostRegister()
{}

/*************************************
 * PreUnregister
 *************************************/
void SHVModule::PreUnregister()
{}

/*************************************
 * Unregister
 *************************************/
void SHVModule::Unregister()
{
	Registered = false;
	Events.Clear();
}


/*************************************
 * IsRegistered
 *************************************/
SHVBool SHVModule::IsRegistered()
{
	return Registered;
}

/*************************************
 * EventSubscribe
 *************************************/
void SHVModule::EventSubscribe(SHVEvent* event, SHVEventSubscriberBase* subscriber)
{
	Events.EventSubscribe(event,subscriber);
}
void SHVModule::EventSubscribe(SHVInt id, SHVEventSubscriberBase* subscriber)
{
	EventSubscribe(new SHVEvent(NULL,id), subscriber);
}
void SHVModule::EventSubscribe(const SHVChar* name, SHVEventSubscriberBase* subscriber)
{
	EventSubscribe(new SHVEventString(NULL,name), subscriber);
}


/*************************************
 * OnEvent
 *************************************/
void SHVModule::OnEvent(SHVEvent* event)
{
	SHVUNUSED_PARAM(event);
}

/*************************************
 * EmitEvent
 *************************************/
/// emit an event
/**
 \param event Event to emit
 \return true if it was registered, and there were subscribers
 */
bool SHVModule::EmitEvent(SHVEvent* event)
{
	return Events.EmitEvent(Modules,event);
}
