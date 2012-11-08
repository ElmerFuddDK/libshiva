#ifndef __SHIVA_DATASESSION_SQLITE_H
#define __SHIVA_DATASESSION_SQLITE_H

#include "../../../include/sqlite/sqlitewrapper.h"
#include "../../../include/modules/dataengine/shvdatasession.h"
#include "../../../include/modules/dataengine/shvdatarow.h"
class SHVStringStreamUTF8;


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
	virtual SHVDataFunction* PrepareFunction(const SHVStringC& query, const SHVDataRowKey* sortKey = NULL);
	virtual SHVDataFunction* GetDataChangeFunction(SHVDataRowList *rowList, int function);
	virtual SHVDataRowListC* QueryTable(const SHVString8C& tableName, const SHVStringC& condition, size_t index);
	virtual SHVDataRowListC* QueryTableIndexed(const SHVString8C& tableName, const SHVStringC& condition, size_t index);
	virtual SHVDataRowList* CopyAlias(const SHVString8C& sourceAlias, const SHVString8C& destAlias);
	virtual SHVBool ExecuteNonQuery(const SHVStringC& sql);

	virtual void SubscribeDataChange(SHVEventSubscriberBase* sub);
	virtual void* GetProvider(bool checkConnection = true);
	virtual SHVDataFactory* GetFactory() const;
	virtual SHVStringBuffer GetErrorMessage() const;

protected:
	virtual ~SHVDataSessionSQLite();
	
	virtual void UpdateRowSQL(const SHVDataRowList* rowList, SHVStringStreamUTF8& sql);
	virtual void InitUpdateFunction(SHVDataFunction* func, SHVDataRow* row);
	virtual void InsertRowSQL(const SHVDataRowList* rowList, SHVStringStreamUTF8& sql, bool replaceIfDuplicate);
	virtual void InitInsertFunction(SHVDataFunction* func, SHVDataRow* row);
	virtual void DeleteRowSQL(const SHVDataRowList* rowList, SHVStringStreamUTF8& sql);
	virtual void InitDeleteFunction(SHVDataFunction* func, const SHVDataRow* row);
	virtual void InitDeleteFunction(SHVDataFunction* func, const SHVDataRowKey* key, const SHVDataStructC* struc);
	virtual SHVStringUTF8 WhereSQL(SHVDataRow* row);

	bool EmptySlot(const SHVDataRowVector& vector, size_t& idx);
	SHVDataRow* Find(const SHVDataRowVector& vector, const SHVDataRowKey* key, size_t& idx) const;

	// from SHVDataSession
	virtual SHVBool UpdateRow(SHVDataRow* row);
	virtual SHVBool IsValid() const;
	virtual void SchemaChanged();
	virtual SHVBool CheckConnection();
	SHVDataFunction* PrepareFunction(const SHVStringUTF8C& query, const SHVDataRowKey* sortKey = NULL);

private:
	SHVBool SessionReset();
	void SessionReposition();
	void StreamWhereSQL(const SHVDataRowList* rowList, SHVStringStreamUTF8 &str, SHVDataRow *row);

	// data
	SHVSQLiteWrapperRef SQLite;
	SHVDataFactoryRef Factory;
	SHVDataRowVector ChangedRows;
	SHVEventSubscriberBaseRef ChangeSubscriber; 
	SHVModuleList& Modules;
	SHVMutex Lock;
	bool SchemaValid;
};
typedef SHVRefObjectContainer<SHVDataSessionSQLite> SHVDataSessionSQLiteRef;

#endif


