// shivaguitester.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "../../../../include/platformspc.h"
#include "../../../../include/shvversion.h"

#include "../../../../include/frameworkimpl/shvmainthreadeventqueue.h"
#include "../../../../include/utils/shvdll.h"

#include "../../../include/shvguimanager.h"

#include "modules/shvcontroltester.h"

int APIENTRY _tWinMain(HINSTANCE hInstance,
                     HINSTANCE hPrevInstance,
                     LPTSTR    lpCmdLine,
                     int       nCmdShow)
{
SHVDll guilib;
int retVal = -1;

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR,__SHIVA_VERSION_MINOR,__SHIVA_VERSION_RELEASE))
	{
		::MessageBox(NULL,_T("Invalid version of libshiva.dll"),_T("Error"),MB_ICONERROR);
	}
	else if (!guilib.Load(guilib.CreateLibFileName(_T("shivaguiwin32"))))
	{
		::MessageBox(NULL,_T("These are not the droids you are looking for ..."),_T("FætterHat"),0);
	}
	else
	{
//	SHVMainThreadEventQueue mainqueue(new SHVMainThreadEventDispatcherGeneric());
	SHVMainThreadEventQueue mainqueue((SHVMainThreadEventDispatcher*)guilib.CreateObjectInt(NULL,SHVDll::ClassTypeMainThreadDispatcher));

		mainqueue.GetModuleList().GetConfig(SHVModuleList::CfgGUI).SetPtr(SHVGUIManager::CfgInstanceHandle,hInstance);

		mainqueue.GetModuleList().AddModule(new SHVControlTester(mainqueue.GetModuleList()));

		retVal = mainqueue.Run();
	}

	guilib.Unload();

#ifdef DEBUG
	_CrtDumpMemoryLeaks();
#endif

	return retVal;
}
