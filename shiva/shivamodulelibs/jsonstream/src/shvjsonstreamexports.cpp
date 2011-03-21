#include "shiva/include/platformspc.h"

#include "shiva/include/utils/shvdll.h"
#include "shiva/include/framework/shvmodulelist.h"
#include "shvjsonstreamfactory.h"


extern "C"
{

void* CreateObjectInt(SHVModuleList* list, int id)
{
	SHVUNUSED_PARAM(list);

	switch (id)
	{
	case SHVDll::ClassTypeModuleFactory:
		return new SHVModuleFactoryJsonStream(*list);
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

}
