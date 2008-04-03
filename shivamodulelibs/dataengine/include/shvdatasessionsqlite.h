#ifndef __SHIVA_DATASESSION_SQLITE_H
#define __SHIVA_DATASESSION_SQLITE_H

#include "../../../include/sqlite/sqlitewrapper.h"
#include "../../../include/modules/dataengine/shvdatasession.h"
#include "../../../include/modules/dataengine/shvdatarow.h"

class SHVDataSessionSQLite: public SHVDataSession
{
public:
	SHVDataSessionSQLite(SHVModuleList& modules, SHVSQLiteWrapper* sqlite, SHVDataFactory* factory);

    // from SHVDataSession
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
	virtual bool AliasActive(const SHVString8C& alias);
	virtual void* GetProvider();
	virtual SHVDataFactory* GetFactory() const;
	virtual SHVStringBuffer GetErrorMessage() const;


protected:
	virtual ~SHVDataSessionSQLite();
	
	virtual SHVBool DoUpdateRow(SHVDataRow* row);
	virtual SHVBool DoInsertRow(SHVDataRow* row);
	virtual SHVBool DoDeleteRow(SHVDataRow* row);
	virtual SHVStringUTF8 WhereSQL(SHVDataRow* row);
	virtual void CheckSchema();

	bool EmptySlot(const SHVDataRowVector& vector, size_t& idx);
	SHVDataRow* Find(const SHVDataRowVector& vector, const SHVDataRowKey* key, size_t& idx) const;

	// from SHVDataSession
	virtual void ClearOwnership();
	virtual SHVBool UpdateRow(SHVDataRow* row);
	virtual SHVBool IsValid() const;
	virtual bool SchemaChanged();
	virtual void RegisterDataList(SHVDataRowListC* rowList);
	virtual void UnregisterDataList(SHVDataRowListC* rowList);


private:
	SHVBool SessionReset();
	void SessionReposition();

	// inlines
	inline bool HasPendingDataLists() const;

	// data
	SHVSQLiteWrapperRef SQLite;
	SHVDataFactory* Factory;
	SHVDataRowVector ChangedRows;
	SHVEventSubscriberBaseRef ChangeSubscriber; 
	SHVBool Editting;
	SHVModuleList& Modules;
	SHVList<SHVDataRowListC*> ActiveDataLists;
	bool Valid;
};
typedef SHVRefObjectContainer<SHVDataSessionSQLite> SHVDataSessionSQLiteRef;

// =========================================== implementation ============================================ //
bool SHVDataSessionSQLite::HasPendingDataLists() const
{
	return ActiveDataLists.GetCount() != 0;
}

#endif


