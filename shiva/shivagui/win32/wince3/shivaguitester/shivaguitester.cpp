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

// shivaguitester.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "../../../../include/platformspc.h"
#include "../../../../include/shvversion.h"

#include "../../../../include/frameworkimpl/shvmainthreadeventqueue.h"
#include "../../../../include/utils/shvdll.h"

#include "../../../../include/gui/shvguimanager.h"
#include "../../../../include/gui/shvregisterbitmap.h"

#include "modules/shvcontroltester.h"
#include "resource.h"

struct test { int a,b; };

int WINAPI WinMain(	HINSTANCE hInstance,
					HINSTANCE hPrevInstance,
					LPTSTR    lpCmdLine,
					int       nCmdShow)
{
SHVDll guilib;
int retVal = -1;

	SHVUNUSED_PARAM(hPrevInstance);
	SHVUNUSED_PARAM(lpCmdLine);

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR,__SHIVA_VERSION_MINOR,__SHIVA_VERSION_RELEASE))
	{
		::MessageBoxW(NULL,L"Invalid version of libshiva.dll",L"Error",MB_ICONERROR);
	}
	else if (!guilib.Load(guilib.CreateLibFileName(_S("shivaguiwin32"))))
	{
		::MessageBoxW(NULL,L"Failed loading GUI module",L"GUI Tester",MB_ICONERROR);
	}
	else
	{
	SHVMainThreadEventQueue mainqueue((SHVMainThreadEventDispatcher*)guilib.CreateObjectInt(NULL,SHVDll::ClassTypeMainThreadDispatcher));

		// register bitmap resources
		SHVRegisterBitmap::Registration regs[] = { {IDB_HEYYOU,1}, {0,0} };
		SHVRegisterBitmap(mainqueue.GetModuleList(), regs);

		mainqueue.GetModuleList().AddModule(new SHVControlTester(mainqueue.GetModuleList()));

		retVal = mainqueue.Run();
	}

	guilib.Unload();

	return retVal;
}
