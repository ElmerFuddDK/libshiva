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
	virtual SHVBool RegisterTable(const SHVDataStructC* dataStruct);
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
	virtual void RegisterDataSession(SHVDataSession* session);
	virtual void UnregisterDataSession(SHVDataSession* session);
	virtual void RowChanged(SHVDataRow* row);
	virtual SHVBool SessionReset(SHVDataSession* session);
	virtual void SessionReposition(SHVDataSession* session);

	virtual SHVBool CreateTable(const SHVDataStructC* dataStruct);
	virtual SHVBool CreateIndex(const SHVDataStructC* dataStruct, size_t index);
	virtual void SetSQLite(SHVSQLiteWrapper* sqlite);
private:
	friend class SHVDataEngine_impl;
	SHVDataEngine& DataEngine;
	SHVSQLiteWrapperRef SQLite;
	SHVString Database;
	SHVDataSchema Schema;
	SHVList<SHVDataRowListC*> ActiveDataLists;
	SHVList<SHVDataSession*> ActiveSessions;
	SHVEventSubscriberBaseRef DataChangedSubscription;
	SHVMutex FactoryLock;
	SHVBool Ok;
};

#endif
