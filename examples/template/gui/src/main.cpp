#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shiva/include/shvversion.h"
#include "shiva/include/framework/shvmodule.h"
#include "shiva/include/framework/shvconsole.h"
#include "shiva/include/frameworkimpl/shvmainthreadeventqueue.h"
#include "shiva/include/utils/shvdll.h"

#include "shvshvtemplateshv.h"

// The main function - boots up the application
#ifdef __SHIVA_WINCE
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
#elif defined(__SHIVA_WIN32)
int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
#else
int main(int argc, char *argv[])
#endif
{
SHVDll guilib;
int retVal = -1;

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE))
	{
		SHVConsole::ErrPrintf(_T("WRONG SHIVA VERSION\n"));
	}
#ifdef __SHIVA_WIN32
	else if (!guilib.Load(guilib.CreateLibFileName(_T("shivaguiwin32"))))
#else
	else if (!guilib.Load(guilib.CreateLibFileName(_T("shivaguigtk"))))
#endif
	{
		SHVConsole::ErrPrintf(_T("Could not load GUI module library\n"));
	}
	else
	{
		// Initialize the main thread event queue - we are a console application
		SHVMainThreadEventQueue mainqueue((SHVMainThreadEventDispatcher*)guilib.CreateObjectInt(NULL,SHVDll::ClassTypeMainThreadDispatcher));

		// Add our application modules
		mainqueue.GetModuleList().AddModule(new SHVSHVTemplateSHV(mainqueue.GetModuleList()));

		// run the application
		retVal = mainqueue.Run().GetError();
	}
	
	// Unload the GUI lib - we should now be free of heap allocations
	guilib.Unload();

	return retVal;
}
