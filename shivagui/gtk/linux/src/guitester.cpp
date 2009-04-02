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

#include "../../../../include/platformspc.h"
#include "../../../../include/shvversion.h"

#include "../../../../include/frameworkimpl/shvmainthreadeventqueue.h"
#include "../../../../include/utils/shvdll.h"

#include "../../../../include/gui/shvguimanager.h"


class SHVTest : public SHVModule
{
public:
	
	SHVGUIManager* GUIManager;

	SHVTest(SHVModuleList& modules) : SHVModule(modules,"Test")
	{
	}

	SHVBool Register()
	{
		printf("In register\n");
		
		if (!SHVModuleResolver<SHVGUIManager>(Modules,GUIManager,"GUIManager"))
			return false;
	
		SHVASSERT(Modules.ResolveModule("Test"));
		SHVASSERT(Modules.ResolveModule("Timer"));
	
		Modules.EventSubscribe(SHVModuleList::EventClosing, new SHVEventSubscriber(this));
		
		GUIManager->GetMainWindow()->SetFont(GUIManager->GetFont(SHVGUIManager::CfgFontLargeBold)->CreateCopy(250),false);
	
		return SHVModule::Register();
	}
	
	void PostRegister()
	{
		GUIManager->GetMainWindow()->SetTitle(_T("SHIVA GUI test application"));

		GUIManager->GetMainWindow()->SetFlag(SHVControl::FlagVisible);
		GUIManager->GetMainWindow()->SetMinimumSize(150,100);
		GUIManager->GetMainWindow()->SetColor(GUIManager->CreateColor(0xFF,0x00,0xFF));
		GUIManager->GetMainWindow()->ResizeControls();
	}

	void OnEvent(SHVEvent* event)
	{
		SHVUNUSED_PARAM(event);
		printf("Delaying shutdown by 2500 ms\n");
		Modules.EmitEvent(new SHVEventString(this,__EVENT_GLOBAL_DELAYSHUTDOWN,2500));
	}

	void Unregister()
	{
		printf("In unregister\n");
		SHVModule::Unregister();
	}
};



int main(int argc, char *argv[])
{
SHVDll guilib;
int retVal = -1;

	SHVUNUSED_PARAM(argc);
	SHVUNUSED_PARAM(argv);

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR,__SHIVA_VERSION_MINOR,__SHIVA_VERSION_RELEASE))
	{
		::fprintf(stderr,_T("Invalid version of libshiva.dll\n"));
	}
	else if (!guilib.Load(guilib.CreateLibFileName(_T("shivaguigtk"))))
	{
		::fprintf(stderr,_T("Could not load GUI module library shivaguigtk\n"));
	}
	else
	{
	SHVMainThreadEventQueue mainqueue((SHVMainThreadEventDispatcher*)guilib.CreateObjectInt(NULL,SHVDll::ClassTypeMainThreadDispatcher));

// 		mainqueue.GetModuleList().AddModule(new SHVControlTester(mainqueue.GetModuleList()));
 		mainqueue.GetModuleList().AddModule(new SHVTest(mainqueue.GetModuleList()));

		retVal = mainqueue.Run();
	}

	guilib.Unload();

	return retVal;
}
