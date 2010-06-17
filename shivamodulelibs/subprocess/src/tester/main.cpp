#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shiva/include/shvversion.h"
#include "shiva/include/framework/shvmodule.h"
#include "shiva/include/framework/shvmodulefactory.h"
#include "shiva/include/framework/shvconsole.h"
#include "shiva/include/utils/shvdll.h"
#include "shiva/include/frameworkimpl/shvmainthreadeventdispatcherconsole.h"

#include "shvsubprocesstester.h"

// The main function - boots up the application
CONSOLEMAIN()
{
SHVDll subprocesslib;

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE))
	{
		SHVConsole::ErrPrintf(_T("WRONG SHIVA VERSION\n"));
	}
	else if (!subprocesslib.Load(subprocesslib.CreateLibFileName(_T("subprocess"))))
	{
		SHVConsole::ErrPrintf(_T("Could not load subprocess library\n"));
	}
	else
	{
	// Initialize the main thread event queue - we are a console application
	SHVMainThreadEventQueue mainqueue(new SHVMainThreadEventDispatcherConsole());
	// Initialize module factory
	SHVModuleFactoryPtr factory = (SHVModuleFactory*)subprocesslib.CreateObjectInt(&mainqueue.GetModuleList(),SHVDll::ClassTypeModuleFactory);

		// Parse any command line arguments into default config
		CONSOLEPARSEARGS(mainqueue.GetModuleList().GetConfig());

		// Resolve default modules from the factory
		factory->ResolveModules(__MODULESYMBOL_DEFAULTS);

		// Add our application modules
		mainqueue.GetModuleList().AddModule(new SHVSubProcessTester(mainqueue.GetModuleList()));

		// run the application and return
		return mainqueue.Run().GetError();
	}

	return -1;
}
