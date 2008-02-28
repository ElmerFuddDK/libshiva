#ifndef __SHIVA_DATAFACTORY_IMPL_H
#define __SHIVA_DATAFACTORY_IMPL_H

#include "../shvdatafactory.h"
#include "../shvdataengine.h"
#include "../../../../shivasqlite/include/sqlitewrapper.h"

//-=========================================================================================================
/// SHVDataFactory_impl class - Implementation of the data factory
/**
 * Is the factory for the data engine objects. 
 */
class SHVDataFactory_impl: public SHVDataFactory
{
public:
	SHVDataFactory_impl(SHVDataEngine& engine, const SHVStringC& database);
	SHVDataFactory_impl(SHVDataEngine& engine, SHVSQLiteWrapper* sqlite, const SHVStringC& database);
	SHVDataFactory_impl(SHVDataEngine& engine, SHVSQLiteWrapper* sqlite, const SHVStringC& database, const SHVDataSchema* Scheme);
	virtual SHVBool RegisterTable(const SHVDataStructC* dataStruct);
	virtual const SHVDataStructC* FindStruct(const SHVString8C& table) const;
	virtual const SHVDataSchema& GetDataSchema() const;

	virtual SHVDataSession* CreateSession();
	virtual SHVDataStruct* CreateStruct() const;
	virtual SHVDataVariant* CreateVariant() const;
	virtual SHVDataRowKey* CreateKey() const;
	virtual const SHVStringC& GetDatabase() const;
	virtual void BuildKeySQL(const SHVDataRowKey* key, SHVString8& condition, SHVString8& orderby, bool reverse = false) const;

protected:
	virtual ~SHVDataFactory_impl();
	virtual SHVBool CreateTable(const SHVDataStructC* dataStruct);
	virtual SHVBool CreateIndex(const SHVDataStructC* dataStruct, size_t index);
	virtual void SetSQLite(SHVSQLiteWrapper* sqlite);
private:
	friend class SHVDataEngine_impl;
	SHVDataEngine& DataEngine;
	SHVSQLiteWrapperRef SQLite;
	SHVString Database;
	SHVDataSchema Schema;
};

#endif
