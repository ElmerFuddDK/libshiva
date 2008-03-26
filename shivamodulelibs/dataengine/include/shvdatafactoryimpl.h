#ifndef __SHIVA_DATAFACTORY_IMPL_H
#define __SHIVA_DATAFACTORY_IMPL_H

#include "../shvdatafactory.h"
#include "../shvdataengine.h"
#include "../shvdatarow.h"

#include "../../../../include/utils/shvlist.h"
#include "../../../../include/threadutils/shvmutex.h"
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
	SHVDataFactory_impl(SHVDataEngine& engine, const SHVStringC& database, const SHVDataSchema* Scheme);
	virtual SHVBool RegisterTable(const SHVDataStructC* dataStruct, bool createTable = false);
	virtual SHVBool RegisterAlias(const SHVString8C& table, const SHVString8C& alias, bool clear = false, SHVDataSession* useSession = NULL);
	virtual SHVBool UnregisterAlias(const SHVString8C& alias);
	virtual const SHVDataStructC* FindStruct(const SHVString8C& table) const;
	virtual const SHVDataSchema& GetDataSchema() const;

	virtual SHVDataSession* CreateSession();
	virtual SHVDataStruct* CreateStruct() const;
	virtual SHVDataVariant* CreateVariant() const;
	virtual SHVDataRowKey* CreateKey() const;
	virtual SHVDataRowKey* CopyKey(const SHVDataRowKey* key) const;
	virtual const SHVStringC& GetDatabase() const;
	virtual void BuildKeySQL(const SHVDataRowKey* key, SHVString8& condition, SHVString8& orderby, const SHVString8C& table, bool reverse = false) const;
	virtual void SubscribeRowChange(SHVEventSubscriberBase* sub);
	virtual SHVDataEngine& GetDataEngine();
	virtual SHVStringBuffer GetErrorMessage() const;
	virtual SHVBool IsOk() const;

protected:
	virtual ~SHVDataFactory_impl();
	virtual void RegisterDataList(SHVDataRowListC* rowList);
	virtual void UnregisterDataList(SHVDataRowListC* rowList);
	virtual bool HasPendingDataLists(const SHVDataSession* session) const;
	virtual void RegisterDataSession(SHVDataSession* session);
	virtual void UnregisterDataSession(SHVDataSession* session);
	virtual void RowChanged(SHVDataRow* row);
	virtual SHVBool SessionReset(SHVDataSession* session);
	virtual void SessionReposition(SHVDataSession* session);

	virtual SHVBool CreateTable(SHVSQLiteWrapper* sqlite, const SHVDataStructC* dataStruct, const SHVString8C& tableName);
	virtual SHVBool CreateIndex(SHVSQLiteWrapper* sqlite, const SHVDataStructC* dataStruct, const SHVString8C& tableName, size_t index);
	virtual bool TableMatch(SHVSQLiteWrapper* sqlite, const SHVDataStructC* dataStruct, const SHVString8C& tableName, bool& exists);
	virtual void SetSQLite(SHVSQLiteWrapper* sqlite);
	virtual const SHVDataStructC* InternalFindAlias(const SHVString8C& table) const;
	virtual const SHVDataStructC* InternalFindStruct(const SHVString8C& table) const;
	virtual SHVBool InternalUnregisterAlias(SHVSQLiteWrapper* sqlite, const SHVString8C& alias);
private:
	friend class SHVDataEngine_impl;
	SHVDataEngine& DataEngine;
	SHVSQLiteWrapperRef SQLite;
	SHVString Database;
	SHVDataSchema Schema;
	SHVDataSchemaAlias Alias;
	SHVList<SHVDataRowListC*> ActiveDataLists;
	SHVList<SHVDataSession*> ActiveSessions;
	SHVEventSubscriberBaseRef DataChangedSubscription;
	SHVMutex FactoryLock;
	SHVBool Ok;
};

#endif
