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
#include "../../include/platformspc.h"


#include "shvformimpl.h"
#include "../../include/framework/shveventsubscriber.h"
#include "../../include/framework/shvevent.h"
#include "../../include/framework/shveventstructs.h"

//=========================================================================================================
// SHVFormImpl - base form interface
//=========================================================================================================
SHVFormImpl::SHVFormImpl(SHVFormBase* owner, SHVGUIManager* manager, SHVControlContainer* controlContainer, SHVString8C entityName): 
	Owner(owner),
	GUIManager(manager), 
	EntityName(entityName),
	ControlContainer(controlContainer)
{
	ControlContainer->SubscribePreDestroy(new SHVEventSubscriberFunc<SHVFormImpl>(this,&SHVFormImpl::OnPreDestroyEvent));
}

/*************************************
 * Show
 *************************************/
void SHVFormImpl::Show()
{
	ControlContainer->SetFlag(SHVControl::FlagVisible, true);
}

/*************************************
 * Hide
 *************************************/
void SHVFormImpl::Hide()
{
	ControlContainer->SetFlag(SHVControl::FlagVisible, false);
}

/*************************************
 * Enable
 *************************************/
void SHVFormImpl::Enable()
{
	ControlContainer->SetFlag(SHVControl::FlagDisabled, false);
}

/*************************************
 * Disable
 *************************************/
void SHVFormImpl::Disable()
{
	ControlContainer->SetFlag(SHVControl::FlagDisabled, true);
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVFormImpl::GetData(SHVDataBinder* data)
{
	return ControlContainer->GetData(data);
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVFormImpl::SetData(SHVDataBinder* data)
{
	return ControlContainer->SetData(data);
}

/*************************************
 * GetEntityName
 *************************************/
SHVString8C SHVFormImpl::GetEntityName()
{
	return EntityName;
}

/*************************************
 * GetManager
 *************************************/
SHVGUIManager* SHVFormImpl::GetManager()
{
	return GUIManager;
}

/*************************************
 * GetContainer
 *************************************/
SHVControlContainer* SHVFormImpl::GetContainer()
{
	return ControlContainer;
}

/*************************************
 * OnPreDestroyEvent
 *************************************/
void SHVFormImpl::OnPreDestroyEvent(SHVEvent* event)
{
	SHVEventDataBool::SetValue(event, Owner->PreClose());
}
