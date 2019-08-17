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

#include "../../../include/platformspc.h"
#include "../../../include/shvversion.h"
#include "../../../include/threadutils/shvthread.h"

#include "../../../include/framework/shvgui.h"
#include "../../../include/frameworkimpl/shvmainthreadeventqueue.h"
#include "../../../include/utils/shvdll.h"

#include "../../../include/gui/shvguimanager.h"
#include "../../../include/gui/shvregisterbitmap.h"

#if defined(__SHIVA_GTK)
# include "gtk/heyyou.xpm"
#elif defined(__SHIVA_WIN32)
# include "win32/Resource.h"
#endif

#include "modules/shvcontroltester.h"


GUIMAIN()
{
#ifndef SHIVASTATICMODULELIB
SHVDll guilib;
#endif
int retVal = -1;

	SHVUNUSED_PARAM(argc);
	SHVUNUSED_PARAM(argv);

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR,__SHIVA_VERSION_MINOR,__SHIVA_VERSION_RELEASE))
	{
		::fprintf(stderr,"Invalid version of libshiva.dll\n");
	}
#ifndef SHIVASTATICMODULELIB
	else if (!SHVGUI::LoadLib(guilib))
	{
		::fprintf(stderr,"Could not load GUI module library shivaguigtk\n");
	}
#endif
	else
	{
#ifdef SHIVASTATICMODULELIB
	SHVMainThreadEventQueue mainqueue(SHVGUI_CreateStaticGuiDispatcher());
#else
	SHVMainThreadEventQueue mainqueue((SHVMainThreadEventDispatcher*)guilib.CreateObjectInt(NULL,SHVDll::ClassTypeMainThreadDispatcher));
#endif

#if defined(__SHIVA_GTK)
	SHVRegisterBitmap(mainqueue.GetModuleList(),1,(char**)heyyou_xpm);
#else
	SHVRegisterBitmap::Registration regs[] = { {IDB_HEYYOU,1}, {0,SHVInt()} };
	SHVRegisterBitmap(mainqueue.GetModuleList(), regs);
#endif

		mainqueue.GetModuleList().AddModule(new SHVControlTester(mainqueue.GetModuleList()));

		retVal = mainqueue.Run();
	}

#ifndef SHIVASTATICMODULELIB
	guilib.Unload();
#endif

	return retVal;
}
