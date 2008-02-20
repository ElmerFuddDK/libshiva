#ifndef __SHIVA_DATAFACTORY_H
#define __SHIVA_DATAFACTORY_H

#include "shvdatatable.h"
#include "shvdatastruct.h"


class SHVDataFactory: public SHVRefObject
{
public:
	virtual SHVDataTable* CreateTable(const SHVDataStructC* dataStruct, bool inMemory = true) = 0;
	virtual SHVDataRowListC* CreateSelect(const SHVStringC& query) = 0;
	virtual SHVDataStruct* CreateStruct() = 0;
	virtual SHVDataVariant* CreateVariant() = 0;
protected:
	virtual ~SHVDataFactory() {}
};

#endif