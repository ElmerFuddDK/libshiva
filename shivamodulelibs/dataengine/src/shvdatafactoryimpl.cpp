#include "stdafx.h"

#include "../../../../include/platformspc.h"
#include "../../include/dataengineimpl/shvdatafactory_impl.h"
#include "../../include/dataengineimpl/shvdatatable_sqlite.h"
#include "../../include/dataengineimpl/shvdatarowlistc_sqlite.h"
#include "../../include/dataengineimpl/shvdatastruct_impl.h"
#include "../../include/dataengineimpl/shvdatavariant_impl.h"
#include "../../include/shvdatastructc.h"


SHVDataFactory_impl::SHVDataFactory_impl(SHVSQLiteWrapper* sqlite)
{
	SQLite = sqlite;
}

SHVDataTable* SHVDataFactory_impl::CreateTable(const SHVDataStructC* dataStruct, bool inMemory)
{
	return new SHVDataTable_SQLite(SQLite, dataStruct, inMemory);
}

SHVDataRowListC* SHVDataFactory_impl::CreateSelect(const SHVStringC& query)
{
	return new SHVDataRowListC_SQLite(SQLite, query);
}

SHVDataStruct* SHVDataFactory_impl::CreateStruct()
{
	return new SHVDataStruct_impl();
}

SHVDataVariant* SHVDataFactory_impl::CreateVariant()
{
	return new SHVDataVariant_impl();
}

SHVDataFactory_impl::~SHVDataFactory_impl()
{
}
