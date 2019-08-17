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

#include "shvshvtemplateshv.h"

// The main function - boots up the application
CONSOLEMAIN()
{
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
		// Example of how to add a module factory in static mode:
		// loader.AddModuleFactory(SHVModuleFactory_<LibName>New(&mainqueue.GetModuleList()));
#else
		// Load modules from subdir
		loader.AddModuleLibs(mainqueue.GetModuleList().GetConfig().Find(SHVModuleList::DefaultCfgAppPath)->ToString() + SHVDir::Delimiter() + _S("modules"));
#endif

		// Parse any command line arguments into default config
		CONSOLEPARSEARGS(mainqueue.GetModuleList().GetConfig());

		// Add our application modules
		mainqueue.GetModuleList().AddModule(new SHVSHVTemplateSHV(mainqueue.GetModuleList()));

		// run the application and return
		return mainqueue.Run().GetError();
	}

	return -1;
}
