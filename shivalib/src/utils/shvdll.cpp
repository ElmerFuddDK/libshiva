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
/// \class SHVDll shvdll.h "shiva/include/utils/shvdll.h"

/*************************************
 * Constructor
 *************************************/
/// Constructor
SHVDll::SHVDll() : SHVDllBase()
{
	CreateObjectIntPtr = NULL;
	CreateObjectStringPtr = NULL;
}

/*************************************
 * Desctructor
 *************************************/
/// Destructor
/**
 * Will unload the dll if it is still loaded.
 */
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
		retVal = retVal && Resolve((void**)&CreateObjectIntPtr,SHIVALIB_CREATEOBJECTINTSYMBOL);
		retVal = retVal && Resolve((void**)&CreateObjectStringPtr,SHIVALIB_CREATEOBJECTSTRINGSYMBOL);
#else
		retVal = retVal && Resolve((void**)&CreateObjectIntPtr,SHIVALIB_CREATEOBJECTINTOCTAL);
		retVal = retVal && Resolve((void**)&CreateObjectStringPtr,SHIVALIB_CREATEOBJECTSTRINGOCTAL);
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
	CreateObjectIntPtr = NULL;
	CreateObjectStringPtr = NULL;
}

/*************************************
 * CreateObjectInt
 *************************************/
/// Creates an object according to the class type
/**
 \param list An optional SHVModuleList for the system
 \param classType an enum definition for what class type to create
 *
 \see SHVDll::ClassTypes
 *
 * The library loaded needs to be a SHIVA library, and implement
 * the CreateObjectInt/String C functions. An example:
 \code
// -> In the program :
MyClass* CreateMyClassFromDll(SHVDll& dll)
{
	return (MyClass*)dll.CreateObjectInt(NULL,ClassTypeUser+0);
}

// -> In the dll :
extern "C"
{

void* CreateObjectInt(SHVModuleList* list, int id)
{
	SHVUNUSED_PARAM(list);

	switch (id)
	{
	case ClassTypeUser+0:
		return new MyClass();
	default:
		return NULL;
	}
}

void* CreateObjectString(SHVModuleList* list, const SHVTChar* classname)
{
	SHVUNUSED_PARAM(list);
	SHVUNUSED_PARAM(classname);
	return NULL;
}

}
 \endcode
 * Within SHIVA this is used for creating module libraries. In order to
 * implement a module library the CreateObjectInt function will need to
 * return a SHVModuleFactory derived class when the classType is
 * SHVDll::ClassTypeModuleFactory.\n
 * In case the library contains a main thread event system the
 * SHVDll::ClassTypeMainThreadDispatcher enum must return a
 * SHVMainThreadEventDispatcher derived class. This can then be used
 * for initializing the SHIVA event system.
 */
void* SHVDll::CreateObjectInt(SHVModuleList* list, int classType)
{
void* retVal = NULL;

	if (IsLoaded())
	{
		retVal = CreateObjectIntPtr(list, classType);
	}

	return retVal;
}

/*************************************
 * CreateObjectString
 *************************************/
/// Creates an object according to a defined classType string
/**
 \param list An optional SHVModuleList for the system
 \param classType a string definition for what class type to create
 *
 * The library loaded needs to be a SHIVA library, and implement
 * the CreateObjectString C function. An example:
 \code
// -> In the program :
MyClass* CreateMyClassFromDll(SHVDll& dll)
{
	return (MyClass*)dll.CreateObjectString(NULL,_S("myclass"));
}

// -> In the dll :
extern "C"
{

void* CreateObjectInt(SHVModuleList* list, int id)
{
	SHVUNUSED_PARAM(list);
	SHVUNUSED_PARAM(id);
	return NULL;
}

void* CreateObjectString(SHVModuleList* list, const SHVTChar* classname)
{
void* retVal = NULL;
	SHVUNUSED_PARAM(list);

	if (SHVStringC(classname) == _S("myclass"))
	{
		retVal = new MyClass();
	}

	return retVal;
}

}
 \endcode
 */
void* SHVDll::CreateObjectString(SHVModuleList* list, const SHVStringC classType)
{
void* retVal = NULL;

	if (IsLoaded())
	{
		retVal = CreateObjectStringPtr(list, classType.GetBufferConst());
	}

	return retVal;
}
