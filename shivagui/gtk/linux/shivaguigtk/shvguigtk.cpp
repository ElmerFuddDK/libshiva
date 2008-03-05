#include "../../../../include/platformspc.h"

#include "../../../../include/utils/shvdll.h"
#include "../../../../include/framework/shvmodulelist.h"

// #include "../../gui/shvmainthreadeventdispatchergtk.h"


extern "C"
{

void* CreateObjectInt(SHVModuleList* list, int id)
{
	SHVUNUSED_PARAM(list);
	
	switch (id)
	{
	case SHVDll::ClassTypeMainThreadDispatcher:
// 		return new SHVMainThreadEventDispatcherWin32();
	case SHVDll::ClassTypeModuleFactory:
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
