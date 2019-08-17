#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shiva/include/shvversion.h"
#include "shiva/include/framework/shvmodule.h"
#include "shiva/include/framework/shvmodulefactory.h"
#include "shiva/include/framework/shvconsole.h"
#include "shiva/include/utils/shvdll.h"
#include "shiva/include/utils/shvdir.h"
#include "shiva/include/frameworkimpl/shvmainthreadeventdispatcherconsole.h"
#include "shiva/include/frameworkimpl/shvmoduleloaderimpl.h"
#include "shiva/include/modules/shvmodulefactories.h"

#include "shvsubprocesstester.h"

// The main function - boots up the application
CONSOLEMAIN()
{
SHVDll subprocesslib;

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE))
	{
		SHVConsole::ErrPrintf(_S("WRONG SHIVA VERSION\n"));
	}
	else
	{
	// Initialize the main thread event queue - we are a console application
	SHVMainThreadEventQueue mainqueue(new SHVMainThreadEventDispatcherConsole());
	// Initialize module factory
	SHVModuleLoaderImpl loader(mainqueue.GetModuleList());
#ifdef SHIVASTATICMODULELIB
		loader.AddModuleFactory(SHVModuleFactory_SubProcessNew(&mainqueue.GetModuleList()));
#else
		// Load from subdir
		loader.AddModuleLibs(mainqueue.GetModuleList().GetConfig().Find(SHVModuleList::DefaultCfgAppPath)->ToString() + SHVDir::Delimiter() + _S("modules"));
		// Or load from application path
		loader.AddModuleLib(subprocesslib.CreateLibFileName(_S("subprocess")));
#endif

		// Parse any command line arguments into default config
		CONSOLEPARSEARGS(mainqueue.GetModuleList().GetConfig());

		// Resolve default modules from the factory
		loader.AddSymbol(__MODULESYMBOL_DEFAULTS);
		loader.LoadModules();

		// Add our application modules
		mainqueue.GetModuleList().AddModule(new SHVSubProcessTester(mainqueue.GetModuleList()));

		// run the application and return
		return mainqueue.Run().GetError();
	}

	return -1;
}
