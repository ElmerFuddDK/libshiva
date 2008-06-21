// shivaguiwin32.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "../../../include/platformspc.h"

#include "../../../include/utils/shvdll.h"
#include "../../../include/framework/shvmodulelist.h"

#include "../gui/shvmainthreadeventdispatcherwin32.h"


BOOL APIENTRY DllMain( HANDLE hModule, 
                       DWORD  ul_reason_for_call, 
                       LPVOID lpReserved
					 )
{
    return TRUE;
}



extern "C"
{

void* CreateObjectInt(SHVModuleList* list, int id)
{
	switch (id)
	{
	case SHVDll::ClassTypeMainThreadDispatcher:
		return new SHVMainThreadEventDispatcherWin32();
	case SHVDll::ClassTypeModuleFactory:
	default:
		return NULL;
	}
}

void* CreateObjectString(SHVModuleList* list, const SHVTChar* classname)
{
	return NULL;
}

}
