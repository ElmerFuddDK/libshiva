#ifndef __SHIVA_DATASESSION_SQLITE_H
#define __SHIVA_DATASESSION_SQLITE_H

#include "../../../include/sqlite/sqlitewrapper.h"
#include "../../../include/modules/dataengine/shvdatasession.h"
#include "../../../include/modules/dataengine/shvdatarow.h"

class SHVDataSessionSQLite: public SHVDataSession
{
public:
	SHVDataSessionSQLite(SHVModuleList& modules, SHVSQLiteWrapper* sqlite, SHVDataFactory* factory);
	virtual SHVBool StartEdit();
	virtual SHVBool Commit();
	virtual SHVBool Rollback();

	virtual SHVDataRowList* GetRows(const SHVString8C& tableName, const SHVStringC& condition, size_t index);
	virtual SHVDataRowList* GetRowsIndexed(const SHVString8C& tableName, const SHVStringC& condition, size_t index);
	virtual SHVDataRowListC* Query(const SHVStringC& query, const SHVDataRowKey* sortKey);
	virtual SHVDataRowListC* QueryTable(const SHVString8C& tableName, const SHVStringC& condition, size_t index);
	virtual SHVDataRowListC* QueryTableIndexed(const SHVString8C& tableName, const SHVStringC& condition, size_t index);
	virtual SHVBool ExecuteNonQuery(const SHVStringC& sql);

	virtual SHVBool IsEditting() const;

	virtual void SubscribeDataChange(SHVEventSubscriberBase* sub);
	virtual void* GetProvider();
	virtual SHVDataFactory* GetFactory() const;
protected:
	virtual ~SHVDataSessionSQLite();
	virtual void ClearOwnership();
	virtual SHVBool IsValid() const;
	virtual bool SchemaChanged();
	virtual void CheckSchema();
	virtual SHVBool DoUpdateRow(SHVDataRow* row);
	virtual SHVBool DoInsertRow(SHVDataRow* row);
	virtual SHVBool DoDeleteRow(SHVDataRow* row);
	virtual SHVStringUTF8 WhereSQL(SHVDataRow* row);
	virtual SHVBool UpdateRow(SHVDataRow* row);

	bool EmptySlot(const SHVDataRowVector& vector, size_t& idx);
	SHVDataRow* Find(const SHVDataRowVector& vector, const SHVDataRowKey* key, size_t& idx) const;
private:
	SHVSQLiteWrapperRef SQLite;
	SHVDataFactory* Factory;
	SHVDataRowVector ChangedRows;
	SHVEventSubscriberBaseRef ChangeSubscriber; 
	SHVBool Editting;
	SHVModuleList& Modules;
	bool Valid;
};
typedef SHVRefObjectContainer<SHVDataSessionSQLite> SHVDataSessionSQLiteRef;

#endif