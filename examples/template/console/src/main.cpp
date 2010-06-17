#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shiva/include/shvversion.h"
#include "shiva/include/framework/shvmodule.h"
#include "shiva/include/framework/shvconsole.h"
#include "shiva/include/frameworkimpl/shvmainthreadeventdispatcherconsole.h"

#include "shvshvtemplateshv.h"

// The main function - boots up the application
CONSOLEMAIN()
{
	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE))
	{
		SHVConsole::ErrPrintf(_T("WRONG SHIVA VERSION\n"));
	}
	else
	{
		// Initialize the main thread event queue - we are a console application
		SHVMainThreadEventQueue mainqueue(new SHVMainThreadEventDispatcherConsole());

		// Parse any command line arguments into default config
		CONSOLEPARSEARGS(mainqueue.GetModuleList().GetConfig());

		// Add our application modules
		mainqueue.GetModuleList().AddModule(new SHVSHVTemplateSHV(mainqueue.GetModuleList()));

		// run the application and return
		return mainqueue.Run().GetError();
	}

	return -1;
}
