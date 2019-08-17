// dataengine.cpp : Defines the entry point for the DLL application.
//

#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../../../include/utils/shvdll.h"
#include "../../../include/framework/shvmodulelist.h"

#include "../include/shvdataenginefactory.h"

extern "C"
{
#ifdef SHIVASTATICMODULELIB
SHVModuleFactory* SHVModuleFactory_DataEngineNew(SHVModuleList* list)
{
	return new SHVModuleFactoryDataEngine(*list);
}
#else
void* CreateObjectInt(SHVModuleList* list, int id)
{
	switch (id)
	{
	case SHVDll::ClassTypeModuleFactory:
		return new SHVModuleFactoryDataEngine(*list);
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
#endif
}
