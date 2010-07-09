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
#include "../../../include/framework/shveventstdin.h"
#include "../../../include/frameworkimpl/shvmainthreadeventdispatchergeneric.h"
#include "../../../include/shvversion.h"
#include "../../../include/framework/shvtimer.h"
#include "../../../include/utils/shvmath.h"

class SHVTest : public SHVModule
{
private:
	SHVTimerInstanceRef TimerInstance;
	SHVTimer* Timer;
	
public:

	SHVTest(SHVModuleList& modules) : SHVModule(modules,"Test")
	{
		Modules.GetConfig().Set(_S("test"),2);
		Modules.GetConfig().FindInt(_S("test2"),128);
		SHVConsole::Printf8("result 128-2^6 : %g\n", SHVMath::EvalMap(_S("test2-test^6"),Modules.GetConfig()));
	}
	
	SHVBool Register()
	{
		SHVConsole::Printf8("In register\n");
				
		if (!SHVModuleResolver<SHVTimer>(Modules, Timer, "Timer"))
			return SHVBool::False;
		
		SHVASSERT(Modules.ResolveModule("Test"));
		
		Modules.EventSubscribe(SHVModuleList::EventClosing, new SHVEventSubscriber(this));
		
		return SHVModule::Register();
	}
	
	void PostRegister()
	{
	SHVTime now;
		now.SetNow();
		SHVConsole::Printf(_S("Started: Time now %s\n"), now.ToDateString().GetBufferConst());
		now.AddSeconds(5);
		SHVConsole::Printf8("Application running...\n");
		TimerInstance = Timer->CreateTimer(new SHVEventSubscriber(this));
		TimerInstance->SetAbsolute(now);
	}
	
	void OnEvent(SHVEvent* event)
	{
		if (event->GetCaller() == Timer)
		{
		SHVTime now;
			now.SetNow();
			SHVConsole::Printf8(_S("Stopped: Time now %s\n"), now.ToDateString().GetBufferConst());
			SHVConsole::Printf8("Shutting it down\n");
			Modules.CloseApp();
		}
		else
		{
			SHVConsole::Printf8("Delaying shutdown by 1000 ms\n");
			Modules.EmitEvent(new SHVEventString(this,__EVENT_GLOBAL_DELAYSHUTDOWN,1000));
		}
	}

	void Unregister()
	{
		SHVConsole::Printf8("In unregister\n");
		SHVModule::Unregister();
	}
};


CONSOLEMAIN()
{
	SHVUNUSED_PARAM(argc);
	SHVUNUSED_PARAM(argv);

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE))
	{
		SHVConsole::ErrPrintf8("WRONG SHIVA VERSION\n");
	}
	else
	{
	SHVMainThreadEventQueue mainqueue(new SHVMainThreadEventDispatcherGeneric());
	SHVString testStr;

		CONSOLEPARSEARGS(mainqueue.GetModuleList().GetConfig());

		mainqueue.GetModuleList().AddModule(new SHVTest(mainqueue.GetModuleList()));

		SHVConsole::Printf8("Testing assertions - should fail in debug mode\n");
		SHVASSERT(false);
	
		testStr.Format(_S("This is a test %s %d.%d.%d\n"), _T("of SHIVA version"), __SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE);
	
		SHVConsole::Printf(_S("%s"), testStr.GetSafeBuffer());
		
		return mainqueue.Run().GetError();
	}
	
	return -1;
}
