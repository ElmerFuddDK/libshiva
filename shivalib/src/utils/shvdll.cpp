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

#include "../../../include/utils/shvdll.h"


//=========================================================================================================
// SHVDll class - Load and use SHIVA libraries
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVDll::SHVDll() : SHVDllBase()
{
	CreateClassInt = NULL;
	CreateClassString = NULL;
}

/*************************************
 * Desctructor
 *************************************/
SHVDll::~SHVDll()
{
	SHVDllBase::Unload();
}

/*************************************
 * CreateLibFileName
 *************************************/
SHVStringBuffer SHVDll::CreateLibFileName(const SHVStringC libName, const SHVStringC defaultPath)
{
	return SHVDllBase::CreateLibFileName(libName,defaultPath);
}


/*************************************
 * IsLoaded
 *************************************/
SHVBool SHVDll::IsLoaded()
{
	return SHVDllBase::IsLoaded();
}

/*************************************
 * Load
 *************************************/
SHVBool SHVDll::Load(const SHVStringC libFile)
{
SHVBool retVal = SHVDllBase::Load(libFile);

	if (retVal)
	{
#ifdef __SHVDLLBASE_TEXTMODE
		retVal = retVal && Resolve((void**)&CreateClassInt,_T("CreateClassInt"));
		retVal = retVal && Resolve((void**)&CreateClassString,_T("CreateClassString"));
#else
		retVal = retVal && Resolve((void**)&CreateClassInt,1);
		retVal = retVal && Resolve((void**)&CreateClassString,2);
#endif
		if (!retVal)
			Unload();
	}

	return retVal;
}

/*************************************
 * Unload
 *************************************/
void SHVDll::Unload()
{
	SHVDllBase::Unload();
	CreateClassInt = NULL;
	CreateClassString = NULL;
}

/*************************************
 * CreateClass
 *************************************/
void* SHVDll::CreateClass(int classType)
{
void* retVal = NULL;

	if (IsLoaded())
	{
		retVal = CreateClassInt(classType);
	}

	return retVal;
}
void* SHVDll::CreateClass(const SHVStringC classType)
{
void* retVal = NULL;

	if (IsLoaded())
	{
		retVal = CreateClassString(classType.GetBufferConst());
	}

	return retVal;
}
