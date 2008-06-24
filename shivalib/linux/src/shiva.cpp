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
#include "../../../include/framework/shvmodule.h"
#include "../../../include/frameworkimpl/shvmainthreadeventdispatchergeneric.h"
#include "../../../include/shvversion.h"


class SHVTest : public SHVModule
{
public:

	SHVTest(SHVModuleList& modules) : SHVModule(modules,"Test")
	{
	}
	
	SHVBool Register()
	{
		printf("In register\n");
		
		SHVASSERT(Modules.ResolveModule("Test"));
		SHVASSERT(Modules.ResolveModule("Timer"));
		
		Modules.EventSubscribe(SHVModuleList::EventClosing, new SHVEventSubscriber(this));
		
		return SHVModule::Register();
	}
	
	void PostRegister()
	{
		printf("Application running...\n");
		printf("Shutting it down\n");
		Modules.CloseApp();
	}
	
	void OnEvent(SHVEvent* event)
	{
		SHVUNUSED_PARAM(event);
		printf("Delaying shutdown by 1000 ms\n");
		Modules.EmitEvent(new SHVEventString(this,__EVENT_GLOBAL_DELAYSHUTDOWN,1000));
	}

	void Unregister()
	{
		printf("In unregister\n");
		SHVModule::Unregister();
	}
};


int main(int argc, char *argv[])
{
	SHVUNUSED_PARAM(argc);
	SHVUNUSED_PARAM(argv);

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE))
	{
		fprintf(stderr,"WRONG SHIVA VERSION\n");
	}
	else
	{
	SHVMainThreadEventQueue mainqueue(new SHVMainThreadEventDispatcherGeneric());
	SHVString testStr;

		mainqueue.GetModuleList().AddModule(new SHVTest(mainqueue.GetModuleList()));

		printf("Testing assertions - should fail in debug mode\n");
		SHVASSERT(false);
	
		testStr.Format(_T("This is a test %s %d.%d.%d\n"), _T("of SHIVA version"), __SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE);
	
		printf("%s", testStr.GetSafeBuffer());
		
		return mainqueue.Run().GetError();
	}
	
	return -1;
}
