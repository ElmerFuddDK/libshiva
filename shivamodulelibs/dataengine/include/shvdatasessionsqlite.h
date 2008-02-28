#ifndef __SHIVA_DATASESSION_SQLITE_H
#define __SHIVA_DATASESSION_SQLITE_H

#include "../../../../shivasqlite/include/sqlitewrapper.h"
#include "../shvdatasession.h"
#include "../shvdatafactory.h"

class SHVDataSession_SQLite: public SHVDataSession
{
public:
	SHVDataSession_SQLite(SHVModuleList& modules, SHVSQLiteWrapper* sqlite, SHVDataFactory* factory);
	virtual SHVBool StartEdit();
	virtual SHVBool Commit();
	virtual SHVBool Rollback();

	virtual SHVDataRowList* GetRows(const SHVString8C& tableName, const SHVStringC& condition, size_t index);
	virtual SHVDataRowListC* Query(const SHVStringC& query, const SHVDataRowKey* sortKey);
	virtual SHVDataRowListC* QueryTable(const SHVString8C& tableName, const SHVStringC& condition, size_t index);
	virtual SHVBool ExecuteNonQuery(const SHVStringC& sql);

	virtual SHVBool IsEditting() const;
	virtual SHVBool UpdateRow(SHVDataRow* row);

	virtual void SubscribeDataChange(SHVEventSubscriberBase* sub);
	virtual void* GetProvider();
protected:
	virtual ~SHVDataSession_SQLite();
	bool EmptySlot(const SHVDataRowVector& vector, size_t& idx);
	SHVDataRow* Find(const SHVDataRowVector& vector, const SHVDataRowKey* key, size_t& idx) const;
	virtual SHVBool DoUpdateRow(SHVDataRow* row);
	virtual SHVBool DoInsertRow(SHVDataRow* row);
	virtual SHVBool DoDeleteRow(SHVDataRow* row);
	SHVStringUTF8 WhereSQL(SHVDataRow* row);
private:
	SHVSQLiteWrapperRef SQLite;
	SHVDataFactoryRef Factory;
	SHVDataRowVector ChangedRows;
	SHVEventSubscriberBaseRef ChangeSubscriber; 
	SHVBool Editting;
	SHVModuleList& Modules;
};
typedef SHVRefObjectContainer<SHVDataSession_SQLite> SHVDataSession_SQLiteRef;

#endif