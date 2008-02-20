#ifndef __SHIVA_DATAENGINE_DATASTRUCTC_SQLITE_H
#define __SHIVA_DATAENGINE_DATASTRUCTC_SQLITE_H

#include "../../../../shivasqlite/include/sqlitewrapper.h"
#include "../shvdatastruct.h"
#include "../../../../include/utils/shvstring.h"
#include "shvdatastruct_impl.h"

#define __SQLITE_TYPE_INT      "integer"   // affinity INTEGER
#define __SQLITE_TYPE_DOUBLE   "double"    // affinity DOUBLE
#define __SQLITE_TYPE_STRING   "varchar"   // affinity TEXT
#define __SQLITE_TYPE_DATETIME "datetext"  // affinity TEXT
#define __SQLITE_TYPE_BOOL     "boolint"   // affinity INTEGER

//-=========================================================================================================
/// SHVDataStructC_SQLite class - Class for retrieving sqlite data structure
/**
 */

class SHVDataStructC_SQLite: public SHVDataStructC
{
public:
	SHVDataStructC_SQLite(SHVSQLiteWrapper* sqlite, const SHVString8C& tableName);
	~SHVDataStructC_SQLite();
	virtual const SHVString8C& GetTableName() const;
	virtual const SHVDataStructColumnC* operator[](size_t idx) const;
	virtual const SHVBool FindColumnIndex(size_t& index, const SHVString8C& colName) const;
	virtual size_t GetColumnCount() const;
	virtual const SHVDataRowKey* GetPrimaryIndex() const;
	virtual const SHVDataRowKey* GetIndex(size_t IdxID) const;
	virtual const size_t IndexCount() const;

	virtual SHVBool IsEqual(const SHVDataStructC* dataStruct) const;
	inline SHVBool TableExists() { return !Struct.IsNull(); }
private:
	SHVDataStruct_implRef Struct;
};
typedef SHVRefObjectContainer<SHVDataStructColumnC> SHVDataStructColumnCRef;

#endif