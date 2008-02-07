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

#include "shvguimanagerimpl.h"


//=========================================================================================================
// SHVGUIManagerImpl - Module for registering control types, and instantiating them
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVGUIManagerImpl::SHVGUIManagerImpl(SHVModuleList& modules) : SHVGUIManager(modules)
{
}

/*************************************
 * Register
 *************************************/
SHVBool SHVGUIManagerImpl::Register()
{
	return SHVModule::Register();
}

/*************************************
 * Unregister
 *************************************/
void SHVGUIManagerImpl::Unregister()
{
	ControlsByType.Clear();
	ControlsByName.Clear();

	SHVModule::Unregister();
}

/*************************************
 * RegisterFactory
 *************************************/
SHVBool SHVGUIManagerImpl::RegisterFactory(int controlType, int controlSubType, SHVControlCreatorBase* factory)
{
SHVBool retVal(SHVBool::True);

	ControlsByType[ControlPair(controlType,controlSubType)] = factory;
	
	return retVal;
}
SHVBool SHVGUIManagerImpl::RegisterFactory(const SHVString8C& controlName, SHVControlCreatorBase* factory)
{
SHVBool retVal(SHVBool::True);

	ControlsByName[controlName] = factory;
	
	return retVal;
}

/*************************************
 * CreateControl
 *************************************/
SHVControl* SHVGUIManagerImpl::NewControl(int controlType, int controlSubType)
{
SHVControl* retVal;
SHVControlCreatorBaseRef* creator;

	creator = ControlsByType.Find(ControlPair(controlType, controlSubType));
	if (!creator)
		creator = ControlsByType.Find(ControlPair(controlType, SHVControl::SubTypeDefault));
	retVal = (creator ? (*creator)->New(this) : NULL);
	
	return retVal;
}
SHVControl* SHVGUIManagerImpl::NewControl(const SHVString8C& controlName)
{
SHVControl* retVal;
SHVControlCreatorBaseRef* creator;

	creator = ControlsByName.Find(controlName);
	retVal = (creator ? (*creator)->New(this) : NULL);
	
	return retVal;
}

/*************************************
 * GetModuleList
 *************************************/
SHVModuleList& SHVGUIManagerImpl::GetModuleList()
{
	return Modules;
}

/*************************************
 * GetConfig
 *************************************/
SHVConfig& SHVGUIManagerImpl::GetConfig()
{
	return Modules.GetConfig(SHVModuleList::CfgGUI);
}

/*************************************
 * GetMainWindow
 *************************************/
SHVControlContainer* SHVGUIManagerImpl::GetMainWindow()
{
	return MainWindow;
}

