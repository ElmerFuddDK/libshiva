// DataEngine.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../../../include/utils/shvdll.h"
#include "../../../include/framework/shvmodulelist.h"

#include "shvdatafactory.h"



#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{/*
	if (ul_reason_for_call == DLL_PROCESS_ATTACH &&
		!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR,__SHIVA_VERSION_MINOR,__SHIVA_VERSION_RELEASE))
	{
		::MessageBox(NULL,_T("Invalid version of libshiva.dll in module library DataEngine"),_T("Error"),MB_ICONERROR);
		return FALSE;
	}
*/
	return TRUE;
}


extern "C"
{

void* CreateObjectInt(SHVModuleList* list, int id)
{
	switch (id)
	{
	case SHVDll::ClassTypeModuleFactory:
//		return new SHVModuleFactoryDataEngine(*list);
	default:
		return NULL;
	}
}

void* CreateObjectString(SHVModuleList* list, const SHVTChar* classname)
{
	return NULL;
}

}

#ifdef _MANAGED
#pragma managed(pop)
#endif
