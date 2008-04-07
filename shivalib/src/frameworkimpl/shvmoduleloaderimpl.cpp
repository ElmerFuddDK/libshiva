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
#include "../../../include/utils/shvfile.h"
#include "../../../include/utils/shvdir.h"

#include "../../../include/frameworkimpl/shvmoduleloaderimpl.h"


//=========================================================================================================
// SHVModuleLoaderImpl class - implementation of module loader
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVModuleLoaderImpl::SHVModuleLoaderImpl(SHVModuleList &modules) : SHVModuleLoader(modules)
{
}

/*************************************
 * Destructor
 *************************************/
SHVModuleLoaderImpl::~SHVModuleLoaderImpl()
{
SHVListIterator<SHVModuleLibraryImplPtr,SHVModuleLibraryImpl*> itr(ModuleLibs);

	ModulesInUse.RemoveAll();
	ModuleNameMap.Clear();
	ModuleSymbols.RemoveAll();
	UnusedModules.Clear();

	while (itr.MoveNext())
	{
		itr.Get()->ModuleFactory = NULL;
		if (itr.Get()->Library.IsLoaded())
			itr.Get()->Library.Unload();
	}

	ModuleLibs.RemoveAll();
}

/*************************************
 * AddModuleLibs
 *************************************/
void SHVModuleLoaderImpl::AddModuleLibs(const SHVStringC modulePath)
{
	///\todo implement SHVModuleLoaderImpl::AddModuleLibs
	SHVUNUSED_PARAM(modulePath);
}

/*************************************
 * AddModuleLib
 *************************************/
SHVBool SHVModuleLoaderImpl::AddModuleLib(const SHVStringC fileName)
{
SHVBool retVal(SHVDir::Exist(fileName));

	if (retVal)
	{
	SHVModuleLibraryImpl* moduleLib = new SHVModuleLibraryImpl();

		retVal = moduleLib->Library.Load(fileName);

		if (retVal)
		{
			moduleLib->ModuleFactory = (SHVModuleFactory*)moduleLib->Library.CreateObjectInt(&Modules,SHVDll::ClassTypeModuleFactory);

			moduleLib->LibraryPath = SHVDir::ExtractPath(fileName);
			moduleLib->LibraryName = SHVDir::ExtractName(fileName);

			if (moduleLib->ModuleFactory.IsNull())
			{
				retVal = SHVBool::False;

				moduleLib->Library.Unload();
				delete moduleLib;
			}
			else
			{
				moduleLib->Name = moduleLib->ModuleFactory->GetName();
				moduleLib->Description = moduleLib->ModuleFactory->GetDescription();
				moduleLib->VersionMajor = moduleLib->ModuleFactory->GetMajor();
				moduleLib->VersionMinor = moduleLib->ModuleFactory->GetMinor();
				moduleLib->VersionRelease = moduleLib->ModuleFactory->GetRelease();
				moduleLib->Build = moduleLib->ModuleFactory->GetBuild();

				if (ModuleNameMap.Find(moduleLib->Name))
				{
					retVal = SHVBool::False;

					moduleLib->ModuleFactory = NULL;
					moduleLib->Library.Unload();
					delete moduleLib;
				}
				else
				{
					ModuleLibs.AddTail(moduleLib);
					ModuleNameMap[moduleLib->Name] = moduleLib;
					UnusedModules[moduleLib] = 1;
				}
			}
		}
		else
		{
			delete moduleLib;
		}
	}

	return retVal;
}

/*************************************
 * GetIterator
 *************************************/
SHVModuleLibIterator* SHVModuleLoaderImpl::GetIterator()
{
	return new SHVModuleLibIteratorImpl(*this);
}

/*************************************
 * GetModuleLib
 *************************************/
SHVModuleLibrary* SHVModuleLoaderImpl::GetModuleLib(const SHVStringC name)
{
SHVModuleLibrary* retVal = NULL;
SHVModuleLibraryImpl** moduleLib = ModuleNameMap.Find(name);

	if (moduleLib)
		retVal = *moduleLib;

	return retVal;
}

/*************************************
 * AddSymbol
 *************************************/
void SHVModuleLoaderImpl::AddSymbol(const SHVString8C symbolName)
{
	if (!ContainsSymbol(symbolName))
		ModuleSymbols.AddTail(symbolName);
}

/*************************************
 * ContainsSymbol
 *************************************/
SHVBool SHVModuleLoaderImpl::ContainsSymbol(const SHVString8C symbolName)
{
SHVListIterator<SHVString8,const SHVString8C> itr(ModuleSymbols);
SHVBool retVal(SHVBool::False);

	while (itr.MoveNext() && !retVal)
	{
		if (itr.Get() == symbolName)
			retVal = SHVBool::True;
	}

	return retVal;
}

/*************************************
 * LoadModules
 *************************************/
SHVBool SHVModuleLoaderImpl::LoadModules()
{
SHVBool retVal(SHVBool::True);
SHVListIterator<SHVString8,const SHVString8C> symbolItr(ModuleSymbols);
SHVListIterator<SHVModuleLibraryImplPtr,SHVModuleLibraryImpl*> moduleItr(ModuleLibs);
SHVBool inUse;

	ModulesInUse.RemoveAll();

	while (symbolItr.MoveNext())
	{
		if (!moduleItr.Get()->ModuleFactory.IsNull())
		{
			// Mark the module as "in use"
			if (moduleItr.Get()->ModuleFactory->ResolveModules(symbolItr.Get())
				&& UnusedModules.Find(moduleItr.Get()))
			{
				UnusedModules.Remove(moduleItr.Get());
				moduleItr.Get()->Used = SHVBool::True;
			}

			if (moduleItr.Get()->Used)
			{
				ModulesInUse.AddTail(moduleItr.Get());
			}
		}
		else
		{
			moduleItr.Get()->Used = SHVBool::False;
		}
	}

	return retVal;
}


//=========================================================================================================
// SHVModuleLibraryImpl class - implementation of module library for module loader
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVModuleLibraryImpl::SHVModuleLibraryImpl() : SHVModuleLibrary()
{}

/*************************************
 * Destructor
 *************************************/
SHVModuleLibraryImpl::~SHVModuleLibraryImpl()
{
	if (Library.IsLoaded())
	{
		ModuleFactory = NULL;
		Library.Unload();
	}
}

/*************************************
 * GetLibPath
 *************************************/
const SHVStringC SHVModuleLibraryImpl::GetLibPath() const
{
	return LibraryPath;
}

/*************************************
 * GetLibName
 *************************************/
const SHVStringC SHVModuleLibraryImpl::GetLibName() const
{
	return LibraryName;
}

/*************************************
 * GetName
 *************************************/
const SHVStringC SHVModuleLibraryImpl::GetName() const
{
	return Name;
}

/*************************************
 * GetDescription
 *************************************/
const SHVStringC SHVModuleLibraryImpl::GetDescription() const
{
	return Description;
}

/*************************************
 * GetVersionMajor
 *************************************/
SHVInt SHVModuleLibraryImpl::GetVersionMajor() const
{
	return VersionMajor;
}

/*************************************
 * GetVersionMinor
 *************************************/
SHVInt SHVModuleLibraryImpl::GetVersionMinor() const
{
	return VersionMinor;
}

/*************************************
 * GetVersionRelease
 *************************************/
SHVInt SHVModuleLibraryImpl::GetVersionRelease() const
{
	return VersionRelease;
}

/*************************************
 * GetBuild
 *************************************/
SHVInt SHVModuleLibraryImpl::GetBuild() const
{
	return Build;
}

/*************************************
 * InUse
 *************************************/
SHVBool SHVModuleLibraryImpl::InUse() const
{
	return Used;
}

/*************************************
 * GetFactory
 *************************************/
SHVModuleFactory* SHVModuleLibraryImpl::GetFactory()
{
	return ModuleFactory;
}


//=========================================================================================================
// SHVModuleLibIteratorImpl class - iterator for module libraries
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVModuleLibIteratorImpl::SHVModuleLibIteratorImpl(SHVModuleLoaderImpl& moduleLoader) : Iterator(moduleLoader.ModulesInUse)
{
}

/*************************************
 * operator SHVBool
 *************************************/
SHVModuleLibIteratorImpl::operator SHVBool()
{
	return (bool)Iterator;
}

/*************************************
 * MoveNext
 *************************************/
SHVBool SHVModuleLibIteratorImpl::MoveNext()
{
	return Iterator.MoveNext();
}

/*************************************
 * Get
 *************************************/
SHVModuleLibrary* SHVModuleLibIteratorImpl::Get()
{
	return Iterator.Get();
}
