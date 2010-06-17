#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shiva/include/shvversion.h"
#include "shiva/include/framework/shvmodule.h"
#include "shiva/include/framework/shvconsole.h"
#include "shiva/include/framework/shvgui.h"
#include "shiva/include/frameworkimpl/shvmainthreadeventqueue.h"
#include "shiva/include/utils/shvdll.h"

#include "shvshvtemplateshv.h"

// The main function - boots up the application
GUIMAIN()
{
SHVDll guilib;
int retVal = -1;

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE))
	{
		SHVConsole::ErrPrintf(_T("WRONG SHIVA VERSION\n"));
	}
	else if (!SHVGUI::LoadLib(guilib))
	{
		SHVConsole::ErrPrintf(_T("Could not load GUI module library\n"));
	}
	else
	{
		// Initialize the main thread event queue - we are a console application
		SHVMainThreadEventQueue mainqueue((SHVMainThreadEventDispatcher*)guilib.CreateObjectInt(NULL,SHVDll::ClassTypeMainThreadDispatcher));

		// Parse any command line arguments into default config
		GUIPARSEARGS(mainqueue.GetModuleList().GetConfig());

		// Add our application modules
		mainqueue.GetModuleList().AddModule(new SHVSHVTemplateSHV(mainqueue.GetModuleList()));

		// run the application
		retVal = mainqueue.Run().GetError();
	}
	
	// Unload the GUI lib - we should now be free of heap allocations
	guilib.Unload();

	return retVal;
}
