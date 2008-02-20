#ifndef __SHIVA_DATAENGINE_DATAROWLISTC_SQLITE_H
#define __SHIVA_DATAENGINE_DATAROWLISTC_SQLITE_H

#include "../../../../shivasqlite/include/sqlitewrapper.h"
#include "../../../../include/utils/shvrefobject.h"
#include "../../../../include/utils/shvstring.h"
#include "../../../../include/utils/shvvectorref.h"
#include "../../../../include/shvtypes.h"
#include "../shvdatatable.h"
#include "shvdatatable_sqlite.h"

//-=========================================================================================================
/// SHVDataRowListC_SQLite class - SQLite implementation of SHVDataRowList
/**
 * Interface for a generic shiva datarow list
 */
class SHVDataRowListC_SQLite: public SHVDataRowListC
{
public:
	SHVDataRowListC_SQLite(SHVSQLiteWrapper* sqlLite, const SHVStringC& sql);
	virtual const SHVDataRowC* GetCurrentRow() const;
	virtual SHVBool NextRow();
	virtual SHVBool Reset();
	virtual const SHVDataStructC* GetStruct() const;
	virtual const void* GetProvider() const;
	virtual SHVBool IsOk() const;
protected:
	~SHVDataRowListC_SQLite();
private:
	SHVBool Eof;
	SHVBool Ok;
	SHVDataStructCRef StructCache;
	SHVSQLiteStatementRef Statement;
	SHVDataRowCRef CurrentRow;
};
typedef SHVRefObjectContainer<SHVDataRowListC> SHVDataRowListCRef;


// ==================================== implementation - SHVDataRowList ==================================== //


#endif