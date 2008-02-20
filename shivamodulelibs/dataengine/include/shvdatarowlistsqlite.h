#ifndef __SHIVA_DATAENGINE_DATAROWLIST_SQLITE_H
#define __SHIVA_DATAENGINE_DATAROWLIST_SQLITE_H

#include "../../../../shivasqlite/include/sqlitewrapper.h"
#include "../../../../include/utils/shvrefobject.h"
#include "../../../../include/utils/shvstring.h"
#include "../../../../include/utils/shvvectorref.h"
#include "../../../../include/shvtypes.h"
#include "../shvdatatable.h"
#include "shvdatatable_sqlite.h"

//-=========================================================================================================
/// SHVDataRowList class - Interface for SHVDataRowList
/**
 * Interface for a generic shiva datarow list
 */
class SHVDataRowC_SQLite;
class SHVDataRowList_SQLite: public SHVDataRowList
{
protected:
	~SHVDataRowList_SQLite();
public:
	SHVDataRowList_SQLite(SHVDataTable_SQLite* dataTable, const SHVStringC& sql);
	virtual const SHVDataRowC* GetCurrentRow() const;
	virtual SHVBool NextRow();
	virtual SHVBool Reset();
	virtual SHVDataRow* EditCurrentRow();
	virtual SHVDataRow* AddRow();
	virtual SHVBool CommitChanges();
	virtual SHVBool CancelChanges();
	virtual const SHVDataRowVector* GetChangedRows();
	virtual SHVBool AcceptChanges(SHVDataRow* dataRow);
	virtual SHVBool RejectChanges(SHVDataRow* dataRow);
	virtual const SHVDataStructC* GetStruct() const;
	virtual const void* GetProvider() const;
	virtual SHVBool IsOk() const;
protected:
	SHVDataRow* Find(const SHVDataRowVector& vector, const SHVDataRowKey* key, size_t& idx) const;
	bool EmptySlot(const SHVDataRowVector& vector, size_t& idx);
private:
	friend class SHVDataRowC_SQLite;

	SHVDataRowCRef CurrentRow;
	SHVSQLiteStatementRef Statement;
	SHVDataRowVector ChangedRows;
	SHVDataRowVector NewRows;
	SHVDataRowVector CommittedRows;
	SHVDataTable_SQLiteRef Table;
	SHVBool Ok;
	SHVBool Eof;
};
typedef SHVRefObjectContainer<SHVDataRowList_SQLite> SHVDataRowList_SQLiteRef;

// ==================================== implementation - SHVDataRowList ==================================== //


#endif