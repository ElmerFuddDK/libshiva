#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shiva/include/shvversion.h"
#include "shiva/include/framework/shvmodule.h"
#include "shiva/include/framework/shvmodulefactory.h"
#include "shiva/include/framework/shvconsole.h"
#include "shiva/include/utils/shvdll.h"
#include "shiva/include/utils/shvdir.h"
#include "shiva/include/frameworkimpl/shvmainthreadeventdispatcherconsole.h"

#include "shvfreetdstester.h"

#ifdef SHIVASTATICMODULELIB
#include "../shvfreetdswrapperimpl.h"
#endif


// The main function -boots up the application
// Macro exists for windows CE compatibility
CONSOLEMAIN()
{
#ifndef SHIVASTATICMODULELIB
SHVDll shivafreetdslib;
#endif

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE))
	{
		SHVConsole::ErrPrintf(_S("WRONG SHIVA VERSION\n"));
	}
#ifndef SHIVASTATICMODULELIB
	else if (!shivafreetdslib.Load(shivafreetdslib.CreateLibFileName(_S("shivafreetds"))))
	{
		SHVConsole::ErrPrintf(_S("Could not load shivafreetds library\n"));
	}
#endif
	else
	{
	// Initialize the main thread event queue - we are a console application
	SHVMainThreadEventQueue mainqueue(new SHVMainThreadEventDispatcherConsole());
	SHVString appName, appPath;

		appPath = mainqueue.GetModuleList().GetConfig().Find(SHVModuleList::DefaultCfgAppPath)->ToString();
		appName = mainqueue.GetModuleList().GetConfig().Find(SHVModuleList::DefaultCfgAppName)->ToString();
		mainqueue.GetModuleList().GetConfig().Load(appPath + SHVDir::Delimiter() + appName + _S(".ini"));
	
		CONSOLEPARSEARGS(mainqueue.GetModuleList().GetConfig());

		// Add our application modules
		mainqueue.GetModuleList().AddModule(new SHVFreeTDSTester(mainqueue.GetModuleList(),
#ifdef SHIVASTATICMODULELIB
																 new SHVFreeTDSWrapperImpl(&mainqueue.GetModuleList())
#else
																 (SHVFreeTDSWrapper*)shivafreetdslib.CreateObjectInt(&mainqueue.GetModuleList(),SHVDll::ClassTypeUser)
#endif
										   ));

		// run the application and return
		return mainqueue.Run().GetError();
	}

	return -1;
}
