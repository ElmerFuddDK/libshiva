#include "../../../include/platformspc.h"

#include "../../../include/utils/shvdll.h"
#include "../../../include/framework/shvmodulelist.h"
#include "../../include/shvmysqlconnectionimpl.h"

extern "C"
{

	void* CreateObjectInt(SHVModuleList* list, int id)
	{
		switch (id)
		{
			case SHVDll::ClassTypeUser:
				return new SHVMySQLConnectionImpl();
			default:
				return NULL;
		}
	}

	void* CreateObjectString(SHVModuleList* list, const SHVTChar* classname)
	{
		return NULL;
	}

}
