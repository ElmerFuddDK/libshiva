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

#include "../../../include/framework/shvgui.h"

//=========================================================================================================
// SHVGUI class - GUI static helper class
//=========================================================================================================
/// \class SHVGUI shvgui.h "shiva/include/framework/shvgui.h"

/*************************************
 * LibNameWin32
 *************************************/
const SHVTChar* SHVGUI::LibNameWin32()
{
	return _S("shivaguiwin32");
}

/*************************************
 * LibNameGtk
 *************************************/
const SHVTChar* SHVGUI::LibNameGtk()
{
	return _S("shivaguigtk");
}


/*************************************
 * LoadLib
 *************************************/
/// Will attempt to load the appropriate GUI libs for the current platform
SHVBool SHVGUI::LoadLib(SHVDll& guilib, const SHVStringC defaultPath)
{
#ifdef __SHIVA_WIN32
	return LoadLibExtended(guilib, defaultPath.GetBufferConst(), LibNameWin32(), NULL);
#else
	return LoadLibExtended(guilib, defaultPath.GetBufferConst(), LibNameGtk(), NULL);
#endif
}

/*************************************
 * LoadLibExtended
 *************************************/
/// Will attempt to load a GUI lib from the order given in the list, end with NULL
/**
 Example : SHVGUI::LoadLibExtended(guilib,NULL,SHVGUI::LibNameWin32(),SHVGUI::LibNameGtk(),NULL);
 */
SHVBool SHVGUI::LoadLibExtended(SHVDll& guilib, const SHVTChar* defaultPath, ...)
{
SHVBool retVal(SHVBool::False);
const SHVTChar* name;
SHVVA_LIST args;

	SHVVA_START(args,defaultPath);
	for (name = SHVVA_ARG(args,const SHVTChar*); name; name = SHVVA_ARG(args,const SHVTChar*))
	{
		if (!retVal)
			retVal = guilib.Load(guilib.CreateLibFileName(name, defaultPath));
	}
	SHVVA_END(args);

	return retVal;
}
