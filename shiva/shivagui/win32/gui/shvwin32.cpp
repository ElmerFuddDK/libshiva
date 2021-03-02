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

#include "shvwin32.h"

#if defined(_MSC_VER) && defined(SHIVASTATICMODULELIB)
# pragma comment(lib, "user32.lib")
#endif


//=========================================================================================================
// Win32
//=========================================================================================================

/*************************************
 * MapFlags
 *************************************/
int Win32::MapFlags(int shivaflags)
{
int retVal = 0;

	if (shivaflags & SHVControl::FlagVisible)
		retVal |= WS_VISIBLE;

	if (shivaflags & SHVControl::FlagDisabled)
		retVal |= WS_DISABLED;

	return retVal;
}
