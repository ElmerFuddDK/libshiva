#ifndef __SHIVA_DATAENGINE_DATAROWLISTC_SQLITE_H
#define __SHIVA_DATAENGINE_DATAROWLISTC_SQLITE_H

#include "../../../../shivasqlite/include/sqlitewrapper.h"
#include "../../../../include/utils/shvrefobject.h"
#include "../../../../include/utils/shvstring.h"
#include "../../../../include/utils/shvvectorref.h"
#include "../../../../include/shvtypes.h"
#include "../shvdatarowlistc.h"

//-=========================================================================================================
/// SHVDataRowListC_SQLite class - SQLite implementation of a constant datarow list.
/**
 * This object does not contain any data, but is merely an interface for a sqlite statement.
 */
class SHVDataRowListC_SQLite: public SHVDataRowListC
{
public:
	SHVDataRowListC_SQLite(SHVSQLiteWrapper* sqlLite, const SHVStringC& sql, const SHVDataRowKey* sortKey);
	SHVDataRowListC_SQLite(SHVSQLiteWrapper* sqlLite, const SHVDataStructC* dataStruct, const SHVStringC& condition, size_t index);
	virtual const SHVDataRowC* GetCurrentRow() const;
	virtual SHVBool NextRow();
	virtual SHVBool Reset();
	virtual SHVDataRowC* Find(const SHVDataRowKey* key);
	virtual SHVDataRowListC* Reverse(const SHVStringC& condition);
	virtual const SHVDataStructC* GetStruct() const;
	virtual const void* GetRowProvider() const;
	virtual SHVBool IsOk() const;
protected:
	~SHVDataRowListC_SQLite();
	virtual SHVStringUTF8 BuildQuery(const SHVStringC& condition, bool reverse);
private:
	SHVDataRowListC_SQLite(SHVSQLiteWrapper* sqlLite, SHVSQLiteStatement* statement, const SHVDataStructC* dataStruct, size_t index);
	SHVBool Eof;
	SHVBool Ok;
	SHVDataStructCRef StructCache;
	size_t SortIndex;
	SHVSQLiteStatementRef Statement;
	SHVDataRowCRef CurrentRow;
	SHVSQLiteWrapperRef SQLite;
};
typedef SHVRefObjectContainer<SHVDataRowListC> SHVDataRowListCRef;


// ==================================== implementation - SHVDataRowList ==================================== //


#endif