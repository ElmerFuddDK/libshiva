// dataengine.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "Shiva/include/platformspc.h"
#include "Shiva/include/utils/shvdll.h"
#include "Shiva/include/framework/shvmodulelist.h"

#include "shvdataenginefactory.h"

extern "C"
{

void* CreateObjectInt(SHVModuleList* list, int id)
{
	switch (id)
	{
	case SHVDll::ClassTypeModuleFactory:
		return new SHVModuleFactorydataengine(*list);
	default:
		return NULL;
	}
}

void* CreateObjectString(SHVModuleList* list, const SHVTChar* classname)
{
	SHVUNUSED_PARAM(list);
	SHVUNUSED_PARAM(classname);
	return NULL;
}

}
