#ifndef __SHIVA_DATAFACTORY_IMPL_H
#define __SHIVA_DATAFACTORY_IMPL_H

#include "../shvdatafactory.h"
#include "../../../../shivasqlite/include/sqlitewrapper.h"

class SHVDataFactory_impl: public SHVDataFactory
{
public:
	SHVDataFactory_impl(SHVSQLiteWrapper* sqlite);
	virtual SHVDataTable* CreateTable(const SHVDataStructC* dataStruct, bool inMemory = false);
	virtual SHVDataRowListC* CreateSelect(const SHVStringC& query);
	virtual SHVDataStruct* CreateStruct();
	virtual SHVDataVariant* CreateVariant();
protected:
	virtual ~SHVDataFactory_impl();
private:
	SHVSQLiteWrapperRef SQLite;
};

#endif
