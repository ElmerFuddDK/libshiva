#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shiva/include/utils/shvdll.h"
#include "shiva/include/framework/shvmodulelist.h"
#include "shvluaenginefactory.h"


extern "C"
{


#ifdef SHIVASTATICMODULELIB
SHVModuleFactory* SHVModuleFactory_LuaEngineNew(SHVModuleList* list)
{
	return new SHVModuleFactoryLuaEngine(*list);
}
#else
void* CreateObjectInt(SHVModuleList* list, int id)
{
	SHVUNUSED_PARAM(list);
	
	switch (id)
	{
	case SHVDll::ClassTypeModuleFactory:
		return new SHVModuleFactoryLuaEngine(*list);
	case SHVDll::ClassTypeMainThreadDispatcher:
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
