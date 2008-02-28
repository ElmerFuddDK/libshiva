#ifndef __SHIVA_DATAENGINE_DATATABLE_SQLITE_H
#define __SHIVA_DATAENGINE_DATATABLE_SQLITE_H

#include "../shvdatastructc.h"
#include "../../../../shivasqlite/include/sqlitewrapper.h"

//-=========================================================================================================
///  SHVDataTable_SQLite class - Implements a sqlite datatable.
/**
 * Objects of the class is the link to a sqlite datatable. Be aware that the "Select" automatically
 * prepends 'SELECT * FROM tablename WHERE '. So only the select condition and/or order by clause can be
 * given.
 */
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