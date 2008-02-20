#ifndef __SHIVA_DATAENGINE_IMPL_H
#define __SHIVA_DATAENGINE_IMPL_H

#include "../shvdataengine.h"
#include "../../../include/utils/shvdll.h"

class SHVDataEngine_impl: public SHVDataEngine
{
public:
	virtual ~SHVDataEngine_impl() {}
	virtual SHVDataEngine(SHVModuleList& modules);
	virtual SHVDataFactory* CreateFactory(const SHVStringC& dbName);
private:
	SHVDll SQLiteDll;
};


#endif