#ifndef __SHIVA_DATAENGINE_DATATABLE_SQLITE_H
#define __SHIVA_DATAENGINE_DATATABLE_SQLITE_H

#include "../shvdatatable.h"
#include "../shvdatastructc.h"
#include "../../../../shivasqlite/include/sqlitewrapper.h"

class SHVDataTable_SQLite: public SHVDataTable
{
protected:
	virtual ~SHVDataTable_SQLite() {}
public:
	SHVDataTable_SQLite(SHVSQLiteWrapper* sqlite, const SHVDataStructC* dataStruct, SHVBool inMemory);	
	virtual const SHVDataStructC* GetDataStruct() const;
	virtual SHVDataRowList* Select(const SHVStringC& select);
	virtual SHVBool InMemory() const;
private:
friend class SHVDataRowList_SQLite;
	SHVSQLiteWrapperRef SQLite;
	SHVDataStructCRef Struct;
	SHVBool IsInMemory;
	SHVVectorRef<SHVDataRow> PendingRows;
};
typedef SHVRefObjectContainer<SHVDataTable_SQLite> SHVDataTable_SQLiteRef;


#endif