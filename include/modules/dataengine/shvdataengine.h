#ifndef __SHIVA_DATAENGINE_H
#define __SHIVA_DATAENGINE_H

#include "shvdatafactory.h"
#include "../../../include/framework/shvmodule.h"
#include "../../../include/utils/shvdll.h"

class SHVDataEngine: public SHVModule
{
public:
	virtual ~SHVDataEngine() {}
	virtual SHVDataFactory* CreateFactory(const SHVStringC& dbName) = 0;
};


#endif