// SQLiteWrapper.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "../../include/platformspc.h"

#include "../include/sqlitewrapper.h"
#include "../include/sqliteimpl/sqlitewrapper_impl.h"

#ifdef _WIN32
#  define __REEFT_DLLSPEC __declspec(dllexport) __stdcall
#ifdef _MANAGED
#  pragma managed(push, off)
#endif
#ifndef APIENTRY 
#  define APIENTRY WINAPI
#endif

BOOL APIENTRY DllMain( HANDLE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
    return TRUE;
}

#ifdef _MANAGED
#  pragma managed(pop)
#endif
#else
#  define __REEFT_DLLSPEC
#endif
typedef SHVSQLiteWrapper* LPSHVSQLITEWRAPPER;

LPSHVSQLITEWRAPPER __REEFT_DLLSPEC CreateSHVSQLiteWrapper()
{
	return new SHVSQLiteWrapper_Impl();
}

