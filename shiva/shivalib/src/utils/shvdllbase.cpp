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

#include "../../../include/utils/shvdllbase.h"
#include "../../../include/utils/shvdir.h"

#ifdef __SHIVA_POSIX
# include <dlfcn.h>
#endif


//=========================================================================================================
// SHVDllBase class - Base class for loading dlls
//=========================================================================================================
/// \class SHVDllBase shvdllbase.h "shiva/include/utils/shvdllbase.h"

/*************************************
 * Constructor
 *************************************/
/// Constructor
SHVDllBase::SHVDllBase()
{
#if defined(__SHIVA_WIN32)
	hDll = NULL;
#elif defined(__SHIVA_POSIX)
	hDll = NULL;
#elif defined(__SHIVA_EPOC)
	hDll = NULL;
#endif
}

/*************************************
 * Desctructor
 *************************************/
/// Destructor
/**
 * Will unload the dll if it is still loaded.
 */
SHVDllBase::~SHVDllBase()
{
	Unload();
}

/*************************************
 * FileExtension
 *************************************/
/// Returns the default file dll file extension for the platform
const SHVStringC SHVDllBase::FileExtension()
{
#ifdef __SHIVA_POSIX
	return _S("so");
#else
	return _S("dll");
#endif
}

/*************************************
 * CreateLibFileName
 *************************************/
/// Generates a platform specific library file name
/**
 \param libName Logical name of library, eg libshiva
 \param defaultPath Optional location of library
 *
 * Generates the platform specific file name and returns it.
 * This can be used directly for Load.
 */
SHVStringBuffer SHVDllBase::CreateLibFileName(const SHVStringC libName, const SHVStringC defaultPath)
{
SHVString retVal;
#ifdef __SHIVA_POSIX
SHVString prefix(libName.Left(3) == _S("lib") ? NULL : _S("lib"));
SHVString extension(_S(".so"));
const int extLen = 3;
#else
SHVString prefix;
SHVString extension(_S(".dll"));
const int extLen = 4;
#endif

	retVal.Format(_S("%s%s%s%s%s"),
		defaultPath.GetSafeBuffer(),
		defaultPath.IsNull() || defaultPath.Right(1) == SHVDir::Delimiter() ? _S("") : SHVDir::Delimiter().GetBufferConst(),
		prefix.GetSafeBuffer(),
		libName.GetSafeBuffer(),
		libName.Right(extLen) == extension ? _S("") : extension.GetBufferConst());


	return retVal.ReleaseBuffer();
}


/*************************************
 * IsLoaded
 *************************************/
/// Returns true if there is a dll loaded
SHVBool SHVDllBase::IsLoaded()
{
#if defined(__SHIVA_WIN32)
	return hDll != NULL;
#elif defined(__SHIVA_POSIX)
	return hDll != NULL;
#elif defined(__SHIVA_EPOC)
	return hDll != NULL;
#endif
}

/*************************************
 * Load
 *************************************/
/// Load a dll
/**
 \param libFile The platform specific dll file name to load
 \return Success
 *
 * Attempts to load the dll by the given file name. Use
 * SHVDllBase::CreateLibFileName to create the file name to
 * load for cross platform specific code:
 \code
SHVBool LoadMyDll(SHVDllBase& dll)
{
	// Loads "mylibrary.dll" on windows
	// Loads "libmylibrary.so" on linux
	return dll.Load( dll.CreateLibFileName(_S("mylibrary")) );
}
 \endcode
 \note Will Unload an already loaded dll
 */
SHVBool SHVDllBase::Load(const SHVStringC libFile)
{
SHVBool retVal(SHVBool::False);

	Unload();

#if defined(__SHIVA_WIN32)
	hDll = ::LoadLibrary((const TCHAR*)libFile.GetBufferConst());
	retVal = IsLoaded();
#elif defined(__SHIVA_POSIX)
	dlerror(); // reset error
	hDll = dlopen(libFile.GetBufferConst(), RTLD_NOW);
	if (hDll == NULL) fprintf(stderr, "dlopen(%s) %d, %s\n", libFile.GetBufferConst(), (int)reinterpret_cast<long>(hDll), dlerror());
	retVal = IsLoaded();
#elif defined(__SHIVA_EPOC)
TInt errNum;
TParse parse;

	hDll = new RLibrary();
	parse.Set(libFile.ToPtr(),NULL,NULL);

	retVal = ( (errNum = hDll->Load(parse.FullName())) == KErrNone);
#endif

	return retVal;
}

/*************************************
 * Unload
 *************************************/
/// Unloads a dll
void SHVDllBase::Unload()
{
	if (IsLoaded())
	{
#if defined(__SHIVA_WIN32)
		FreeLibrary(hDll);
		hDll = NULL;
#elif defined(__SHIVA_POSIX)
		dlclose(hDll);
		hDll = NULL;
#elif defined(__SHIVA_EPOC)
		hDll->Close();
		delete hDll;
		hDll = NULL;
#endif
	}
}

/*************************************
 * Resolve
 *************************************/
#if defined(__SHVDLLBASE_TEXTMODE)
SHVBool SHVDllBase::Resolve(void** symbol, const SHVStringC name)
{
SHVBool retVal(SHVBool::False);

	*symbol = NULL;

	if (IsLoaded())
	{
#if defined(__SHIVA_WIN32)
# if defined(UNICODE) && !defined(__SHIVA_WINCE)
	SHVString8 name8(name.ToStr8());
		*symbol = (void*)GetProcAddress(hDll,name8.GetBufferConst());
# else
		*symbol = (void*)GetProcAddress(hDll,(const TCHAR*)name.GetBufferConst());
# endif
#elif defined(__SHIVA_POSIX)
		*symbol = dlsym(hDll, name.GetBufferConst());
#elif defined(__SHIVA_EPOC)
#endif
		retVal = ( *symbol ? SHVBool::True : SHVBool::False );
	}

	return retVal;
}
#elif defined(__SHVDLLBASE_OCTALMODE)
SHVBool SHVDllBase::Resolve(void** symbol, long id)
{
SHVBool retVal(SHVBool::False);

	*symbol = NULL;

	if (IsLoaded())
	{
#if defined(__SHIVA_WIN32)
#elif defined(__SHIVA_POSIX)
#elif defined(__SHIVA_EPOC)
		*symbol = (void*)hDll->Lookup(id);
#endif
		retVal = ( *symbol ? SHVBool::True : SHVBool::False );
	}

	return retVal;
}
#endif
