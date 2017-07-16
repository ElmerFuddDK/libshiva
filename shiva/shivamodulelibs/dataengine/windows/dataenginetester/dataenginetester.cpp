/*
 *   Copyright (C) 2008 by Mogens Bak Nielsen, Lars Eriksen
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

// dataenginetester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../../../include/platformspc.h"

#include "../../../../include/frameworkimpl/shvmainthreadeventdispatchergeneric.h"
#include "../../../../include/frameworkimpl/shvmainthreadeventqueue.h"


#include "../../../../include/utils/shvdll.h"
#include "../../../../include/framework/shvmodulefactory.h"
#include "../../../../include/framework/shvmodule.h"
#include "../../../../include/modules/dataengine/shvdataengine.h"
#include "shvtestmodule.h"
#include "shvtestserver.h"
#include "shvdataenginetest.h"

class SHVTestDone : public SHVModule
{
	SHVTestServer* TestServer;
public:

	SHVTestDone(SHVModuleList& modules) : SHVModule(modules,"TestDone")
	{ }

	enum { EventInternal };

	virtual SHVBool Register()
	{
	SHVEventSubscriberRef directSubs = new SHVEventSubscriber(this);
	SHVEventSubscriberRef queueSubs = new SHVEventSubscriber(this,&Modules);
		if (!SHVModuleResolver<SHVTestServer>(Modules,TestServer,"TestServer")) return false;
		TestServer->EventSubscribe(SHVTestServer::EventTestDone,queueSubs);

		TestServer->InitiateTest();

		return SHVModule::Register();
	}

	virtual void Unregister()
	{
	}

	virtual void OnEvent(SHVEvent* event)
	{
		if (event->GetCaller() == TestServer)
		{
			if (SHVEvent::Equals(event, SHVTestServer::EventTestDone))
			{
				Modules.CloseApp();
			}
		}
	}
};

SHVString GetAppPath()
{
SHVTChar szBuffer[_MAX_PATH];
SHVString apppath(szBuffer);
SHVString path;
	::GetModuleFileName(NULL, (TCHAR*)szBuffer, _MAX_PATH);
	apppath = szBuffer;
	path = apppath.Left(apppath.ReverseFind(_S("\\")));
	path += _S("\\");
	return path;
}
class SHVTestLoggerConsole : public SHVLogger
{
	SHVStringC sTrue;
	SHVStringC sFalse;
	SHVStringC sSuccess;
	SHVStringC sFailure;
public:
	SHVTestLoggerConsole() : sTrue(_S("true")), sFalse(_S("false")), sSuccess(_S("\033[32msucceeded\033[0m")), sFailure(_S("\033[31mFAILED\033[0m")) {}
	virtual void AddTitle(const SHVStringC& str)   { _tprintf(_T("\n\n%s:\n------------------------------------------\n"), str.GetSafeBuffer()); }
	virtual void AddHeader(const SHVStringC& str)  { _tprintf(_T("\n%s\n"), str.GetSafeBuffer()); }
	virtual void AddHeader(const SHVTChar* s, ...) { SHVString str; SHVVA_LIST args; SHVVA_START(args, s); str.FormatList(s,args); AddHeader(str); SHVVA_END(args); }
	virtual void AddLine(const SHVStringC& str)    { _tprintf(_T("  %s\n"), str.GetSafeBuffer()); }
	virtual void AddLine(const SHVTChar* s, ...)   { SHVString str; SHVVA_LIST args; SHVVA_START(args, s); str.FormatList(s,args); AddLine(str); SHVVA_END(args); }
	
	virtual const SHVStringC& True()  { return sTrue;  }
	virtual const SHVStringC& False() { return sFalse; }

	virtual const SHVTChar* Success(bool success) { return (success ? sSuccess.GetBufferConst() : sFailure.GetBufferConst()); }
};


int myMain(int argc, _TCHAR* argv[])
{
SHVMainThreadEventQueue mainqueue(new SHVMainThreadEventDispatcherGeneric());
SHVTestLoggerConsole logger;
SHVDll dll;
SHVBool retVal;

	for (int i=1; i < argc; i++)
	{
		if (argv[i][0] == '-' && argv[i][1] != '\0' && i+1 < argc)
		{
			mainqueue.GetModuleList().GetConfig().Set(SHVStringC((const SHVTChar*)(argv[i]+1)), SHVStringC((const SHVTChar*)(argv[i+1])));
			i++;
		}
	}

	mainqueue.GetModuleList().GetConfig().Set(__DATAENGINE_DEFAULT_DATABASE, _S("test.db"));
	mainqueue.GetModuleList().GetConfig().Set(__DATAENGINE_DATAPATH, GetAppPath());

	mainqueue.GetModuleList().AddModule(new SHVTestDone(mainqueue.GetModuleList()));
	mainqueue.GetModuleList().AddModule(new SHVTestServer(mainqueue.GetModuleList(),logger));
	mainqueue.GetModuleList().AddModule(new SHVDataEngineTest(mainqueue.GetModuleList()));

	if (!dll.Load(dll.CreateLibFileName(_S("DataEngine"))))
	{
		_putts(_T("Could not load dataengine"));
		return 1;
	}
	SHVModuleFactoryPtr engine = (SHVModuleFactory*) dll.CreateObjectInt(&mainqueue.GetModuleList(), SHVDll::ClassTypeModuleFactory);
	engine->ResolveModules(__MODULESYMBOL_DEFAULTS);

	retVal = mainqueue.Run();
	getchar();
	return retVal;
}

int _tmain(int argc, _TCHAR* argv[])
{
int retVal = myMain(argc, argv);
#ifdef DEBUG	
	_CrtDumpMemoryLeaks();
#endif
	return retVal;
}

