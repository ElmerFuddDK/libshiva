#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shiva/include/utils/shvdll.h"
#include "shvfreetdswrapperimpl.h"


extern "C"
{

void* CreateObjectInt(SHVModuleList* list, int id)
{
	SHVUNUSED_PARAM(list);
	
	switch (id)
	{
	case SHVDll::ClassTypeUser:
		return new SHVFreeTDSWrapperImpl(list);
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

#ifdef  __SHIVA_FREETDSVSLIBHACK
// HACK: Enables compilation agains VS compiled sybdb for mingw
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) { return 0; }
#endif

}
