// shivaguitester.cpp : Defines the entry point for the application.
//

#include "stdafx.h"
#include "../../../../include/platformspc.h"

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

	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	if (!guilib.Load(guilib.CreateLibFileName(_T("shivaguiwin32"))))
	{
		::MessageBox(NULL,_T("These are not the droids you are looking for ..."),_T("FætterHat"),0);
		return -1;
	}
	else
	{
//	SHVMainThreadEventQueue mainqueue(new SHVMainThreadEventDispatcherGeneric());
	SHVMainThreadEventQueue mainqueue((SHVMainThreadEventDispatcher*)guilib.CreateObjectInt(NULL,SHVDll::ClassTypeMainThreadDispatcher));

		mainqueue.GetModuleList().GetConfig(SHVModuleList::CfgGUI).SetPtr(SHVGUIManager::CfgInstanceHandle,hInstance);

		mainqueue.GetModuleList().AddModule(new SHVControlTester(mainqueue.GetModuleList()));

		return mainqueue.Run();
	}
}
