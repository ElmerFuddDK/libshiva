#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shiva/include/shvversion.h"
#include "shiva/include/framework/shvmodule.h"
#include "shiva/include/framework/shvmodulefactory.h"
#include "shiva/include/framework/shvconsole.h"
#include "shiva/include/utils/shvdll.h"
#include "shiva/include/frameworkimpl/shvmainthreadeventdispatcherconsole.h"

#include "shvluaenginetester.h"

// The main function -boots up the application
// Macro exists for windows CE compatibility
CONSOLEMAIN()
{
SHVDll luaenginelib;

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE))
	{
		SHVConsole::ErrPrintf(_S("WRONG SHIVA VERSION\n"));
	}
	else if (!luaenginelib.Load(luaenginelib.CreateLibFileName(_S("luaengine"))))
	{
		SHVConsole::ErrPrintf(_S("Could not load luaengine library\n"));
	}
	else
	{
	// Initialize the main thread event queue - we are a console application
	SHVMainThreadEventQueue mainqueue(new SHVMainThreadEventDispatcherConsole());
	// Initialize module factory
	SHVModuleFactoryPtr factory = (SHVModuleFactory*)luaenginelib.CreateObjectInt(&mainqueue.GetModuleList(),SHVDll::ClassTypeModuleFactory);

		CONSOLEPARSEARGS(mainqueue.GetModuleList().GetConfig());
	
		// Resolve default modules from the factory
		factory->ResolveModules(__MODULESYMBOL_DEFAULTS);

		// Add our application modules
		mainqueue.GetModuleList().AddModule(new SHVLuaEngineTester(mainqueue.GetModuleList()));

		// run the application and return
		return mainqueue.Run().GetError();
	}

	return -1;
}
