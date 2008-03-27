// dataenginetester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../../../include/platformspc.h"

#include "../../../../include/frameworkimpl/shvmainthreadeventdispatchergeneric.h"
#include "../../../../include/frameworkimpl/shvmainthreadeventqueue.h"

#include "../../../../include/utils/shvstringutf8.h"

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
TCHAR szBuffer[_MAX_PATH];
SHVString apppath(szBuffer);
SHVString path;
	::GetModuleFileName(NULL, szBuffer, _MAX_PATH);
	apppath = szBuffer;
	path = apppath.Left(apppath.ReverseFind(_T("\\")));
	path += _T("\\");
	return path;
}
class SHVTestLoggerConsole : public SHVLogger
{
	SHVStringC sTrue;
	SHVStringC sFalse;
	SHVStringC sSuccess;
	SHVStringC sFailure;
public:
	SHVTestLoggerConsole() : sTrue(_T("true")), sFalse(_T("false")), sSuccess(_T("\033[32msucceeded\033[0m")), sFailure(_T("\033[31mFAILED\033[0m")) {}
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
			mainqueue.GetModuleList().GetConfig().Set(SHVStringC(argv[i]+1), SHVStringC(argv[i+1]));
			i++;
		}
	}

	mainqueue.GetModuleList().GetConfig().Set(__DATAENGINE_DEFAULT_DATABASE, _T("test.db"));
	mainqueue.GetModuleList().GetConfig().Set(__DATAENGINE_DATAPATH, GetAppPath());

	mainqueue.GetModuleList().AddModule(new SHVTestDone(mainqueue.GetModuleList()));
	mainqueue.GetModuleList().AddModule(new SHVTestServer(mainqueue.GetModuleList(),logger));
	mainqueue.GetModuleList().AddModule(new SHVDataEngineTest(mainqueue.GetModuleList()));

	if (!dll.Load(dll.CreateLibFileName(_T("DataEngine"))))
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

