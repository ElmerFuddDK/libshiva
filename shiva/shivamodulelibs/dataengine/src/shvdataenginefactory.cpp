/*
 *   Copyright (C) 2008 by Mogens Bak Nielsen, Lars Eriksen
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
#include "../include/shvdataengineimpl.h"
#include "../include/shvdataenginefactory.h"


//=========================================================================================================
// SHVModuleFactoryDataEngine - factory for module library DataEngine
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVModuleFactoryDataEngine::SHVModuleFactoryDataEngine(SHVModuleList& modules)
	: SHVModuleFactory(), Modules(modules)
{
}

/*************************************
 * CheckVersion
 *************************************/
SHVBool SHVModuleFactoryDataEngine::CheckVersion(int verMajor,int verMinor,int verRelease)
{
	return ( verMajor == __DATAENGINE_VERSION_MAJOR &&
			 verMinor == __DATAENGINE_VERSION_MINOR &&
			 verRelease == __DATAENGINE_VERSION_RELEASE );
}

/*************************************
 * GetMajor
 *************************************/
SHVInt SHVModuleFactoryDataEngine::GetMajor()
{
	return __DATAENGINE_VERSION_MAJOR;
}

/*************************************
 * GetMinor
 *************************************/
SHVInt SHVModuleFactoryDataEngine::GetMinor()
{
	return __DATAENGINE_VERSION_MINOR;
}

/*************************************
 * GetRelease
 *************************************/
SHVInt SHVModuleFactoryDataEngine::GetRelease()
{
	return __DATAENGINE_VERSION_RELEASE;
}

/*************************************
 * GetBuild
 *************************************/
SHVInt SHVModuleFactoryDataEngine::GetBuild()
{
	return __DATAENGINE_VERSION_RELEASE;
}

/*************************************
 * GetDescription
 *************************************/
SHVStringBuffer SHVModuleFactoryDataEngine::GetDescription()
{
SHVString retVal(_S("Module library for DataEngine"));
	return retVal.ReleaseBuffer();
}

/*************************************
 * ResolveModules
 *************************************/
SHVBool SHVModuleFactoryDataEngine::ResolveModules(const SHVString8C symbol)
{
	if (symbol == __MODULESYMBOL_DEFAULTS)
	{
		Modules.AddModule(new SHVDataEngineImpl(Modules));
		return SHVBool::True;
	}

	return SHVBool::False;
}

/*************************************
 * OnEvent
 *************************************/
void SHVModuleFactoryDataEngine::OnEvent(SHVEvent* event)
{
}

SHVStringBuffer SHVModuleFactoryDataEngine::GetName()
{
	return SHVString(_S("DataEngine")).ReleaseBuffer();
}
