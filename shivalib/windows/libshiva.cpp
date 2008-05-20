// libshiva.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include <locale.h>


#ifdef _MANAGED
#pragma managed(push, off)
#endif

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
					 )
{
#ifndef DEBUG
	if (ul_reason_for_call == DLL_PROCESS_ATTACH)
	{
		setlocale(LC_ALL, "");
	}
#endif
    return TRUE;
}

#ifdef _MANAGED
#pragma managed(pop)
#endif

