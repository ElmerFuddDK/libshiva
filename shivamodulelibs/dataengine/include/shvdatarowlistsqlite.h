#ifndef __SHIVA_DATAENGINE_DATAROWLIST_SQLITE_H
#define __SHIVA_DATAENGINE_DATAROWLIST_SQLITE_H

#include "../../../../shivasqlite/include/sqlitewrapper.h"
#include "../../../../include/utils/shvrefobject.h"
#include "../../../../include/utils/shvstring.h"
#include "../../../../include/utils/shvvectorref.h"
#include "../../../../include/shvtypes.h"
#include "../shvdatarowlist.h"
#include "../shvdatasession.h"

//-=========================================================================================================
/// SHVDataRowList_SQLite class - SQLite implementation of a datarow list.
/**
 * Objects of this class handles all the necessary logic for maintaining a sqlite datatable.
 */
class SHVDataRowC_SQLite;
class SHVDataRowList_SQLite: public SHVDataRowList
{
protected:
	~SHVDataRowList_SQLite();
public:
	SHVDataRowList_SQLite(SHVDataSession* dataSession, SHVDataRowListC *rowList);
	virtual const SHVDataRowC* GetCurrentRow() const;
	virtual SHVBool IsOk() const;
	virtual const SHVDataStructC* GetStruct() const;

	virtual SHVDataRowC* Find(const SHVDataRowKey* key);
	virtual SHVBool NextRow();
	virtual SHVDataRowListC* Reverse(const SHVStringC& condition);
	virtual SHVBool Reset();

	virtual const void* GetRowProvider() const;
	virtual SHVDataSession* GetDataSession();

	virtual SHVDataRow* EditCurrentRow();
	virtual SHVDataRow* AddRow();	
private:
	SHVDataRowListCRef RowList;
	SHVDataSessionRef DataSession;
};
typedef SHVRefObjectContainer<SHVDataRowList_SQLite> SHVDataRowList_SQLiteRef;



#endif