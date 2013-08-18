#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shiva/include/shvversion.h"
#include "shiva/include/framework/shvmodule.h"
#include "shiva/include/framework/shvconsole.h"
#include "shiva/include/framework/shvgui.h"
#include "shiva/include/frameworkimpl/shvmainthreadeventqueue.h"
#include "shiva/include/frameworkimpl/shvmainthreadeventdispatcherconsole.h"
#include "shiva/include/frameworkimpl/shvconfigimpl.h"
#include "shiva/include/utils/shvdll.h"
#include "shiva/include/utils/shvdir.h"
#include "shiva/include/frameworkimpl/shvmoduleloaderimpl.h"

#include "shvunittestimpl.h"

#include "tests/utils/shvbase64tester.h"
#include "tests/utils/shvbooltester.h"
#include "tests/utils/shvbuffertester.h"
#include "tests/utils/shvdoubletester.h"
#include "tests/utils/shvdynarraytester.h"
#include "tests/utils/shvfiletester.h"
#include "tests/utils/shvhashtabletester.h"
#include "tests/utils/shvinttester.h"
#include "tests/utils/shvlisttester.h"
#include "tests/utils/shvmd5sumtester.h"
#include "tests/utils/shvstringtester.h"
#include "tests/utils/shvtimetester.h"
#include "tests/utils/shvvectortester.h"

#include "tests/threadutils/shvmutextester.h"
#include "tests/threadutils/shvsemaphoretester.h"
#include "tests/threadutils/shvthreadpooltester.h"
#include "tests/threadutils/shvthreadtester.h"


// The main function - boots up the application
GUIMAIN()
{
SHVDll guilib;
int retVal = -1;
SHVMainThreadEventDispatcher* dispatcher = NULL;

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE))
	{
		SHVConsole::ErrPrintf(_S("WRONG SHIVA VERSION\n"));
	}
	
	// temporarily parse the arguments to see if we are forcing console mode
	{
	SHVMutex lock;
	SHVConfigImpl cfg(lock);
		
		GUIPARSEARGS(cfg);
		
		if (cfg.FindInt(_S("console"), 0)->ToInt().IfNull(0) == 0)
		{
			if (!SHVGUI::LoadLib(guilib))
			{
				SHVConsole::ErrPrintf(_S("Failed to load a suitable GUI lib, starting in console mode\n"));
			}
		}
	}

	// Create the dispatcher
	if (guilib.IsLoaded())
	{
		// We are a GUI application
		dispatcher = (SHVMainThreadEventDispatcher*)guilib.CreateObjectInt(NULL,SHVDll::ClassTypeMainThreadDispatcher);
	}
	if (!dispatcher)
	{
		// We are a console application
		dispatcher = new SHVMainThreadEventDispatcherConsole();
	}
	
	if (dispatcher)
	{
	SHVUnitTestImpl* unitTest;
	// Initialize the main thread event queue
	SHVMainThreadEventQueue mainqueue(dispatcher);
	SHVModuleLoaderImpl ModuleLoader(mainqueue.GetModuleList());

		// Parse any command line arguments into default config
		GUIPARSEARGS(mainqueue.GetModuleList().GetConfig());

		// Add our application modules
		mainqueue.GetModuleList().AddModule(unitTest = new SHVUnitTestImpl(mainqueue.GetModuleList()));

		// Add the tests
		unitTest->RegisterTest(new SHVBase64Tester());
		unitTest->RegisterTest(new SHVBoolTester());
		unitTest->RegisterTest(new SHVBufferTester());
		unitTest->RegisterTest(new SHVDoubleTester());
		unitTest->RegisterTest(new SHVDynArrayTester());
		unitTest->RegisterTest(new SHVFileTester());
		unitTest->RegisterTest(new SHVHashTableTester());
		unitTest->RegisterTest(new SHVIntTester());
		unitTest->RegisterTest(new SHVMd5SumTester());
		unitTest->RegisterTest(new SHVListTester());
		unitTest->RegisterTest(new SHVStringTester());
		unitTest->RegisterTest(new SHVTimeTester());
		unitTest->RegisterTest(new SHVVectorTester());

		unitTest->RegisterTest(new SHVMutexTester(mainqueue.GetModuleList()));
		unitTest->RegisterTest(new SHVSemaphoreTester(mainqueue.GetModuleList()));
		unitTest->RegisterTest(new SHVThreadTester(mainqueue.GetModuleList()));
		unitTest->RegisterTest(new SHVThreadPoolTester(mainqueue.GetModuleList()));

		// Load unit tests from module libraries
		ModuleLoader.AddModuleLibs(mainqueue.GetModuleList().GetConfig().Find(SHVModuleList::DefaultCfgAppPath)->ToString() + SHVDir::Delimiter() + _S("modules"));
		ModuleLoader.AddSymbol(__MODULESYMBOL_DEFAULTS);
		ModuleLoader.AddSymbol(__MODULESYMBOL_UNITTESTS);
		ModuleLoader.LoadModules();

		// run the application
		retVal = mainqueue.Run().GetError();
	}
	
	// Unload the GUI lib - we should now be free of heap allocations
	guilib.Unload();

	return retVal;
}
