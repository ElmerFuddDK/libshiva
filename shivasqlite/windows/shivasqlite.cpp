// SQLiteWrapper.cpp : Defines the entry point for the DLL application.
//
#include "stdafx.h"
#include "../../include/platformspc.h"

#include "../../include/utils/shvdll.h"
#include "../../include/framework/shvmodulelist.h"
#include "../include/sqlitewrapperimpl.h"


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
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
	case SHVDll::ClassTypeUser:
		return new SHVSQLiteWrapperImpl();
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

