#ifndef __SHIVA_DATAENGINE_DATASTRUCTC_SQLITE_H
#define __SHIVA_DATAENGINE_DATASTRUCTC_SQLITE_H

#include "../../../include/sqlite/sqlitewrapper.h"
#include "../../../include/modules/dataengine/shvdatastruct.h"
#include "../../../include/utils/shvstring.h"
#include "shvdatastructimpl.h"

#define __SQLITE_TYPE_INT      "integer"   // affinity INTEGER
#define __SQLITE_TYPE_INT64    "int64"     // affinity INTEGER
#define __SQLITE_TYPE_DOUBLE   "double"    // affinity DOUBLE
#define __SQLITE_TYPE_STRING   "varchar"   // affinity TEXT
#define __SQLITE_TYPE_DATETIME "datetext"  // affinity TEXT
#define __SQLITE_TYPE_BOOL     "boolint"   // affinity INTEGER

//-=========================================================================================================
/// SHVDataStructCSQLite class - Class for retrieving sqlite data structure
/**
 * Objects of the class caches the schema information for a sqlite datatable.
 */
class SHVDataStructCSQLite: public SHVDataStructC
{
public:
	SHVDataStructCSQLite(SHVSQLiteWrapper* psqlite, const SHVString8C& tableName);

	// from SHVDataStructC
	virtual const SHVString8C& GetTableName() const;
	virtual const SHVDataStructColumnC* operator[](size_t idx) const;
	virtual const SHVBool FindColumnIndex(size_t& index, const SHVString8C& colName) const;
	virtual size_t GetColumnCount() const;
	virtual const SHVDataRowKey* GetPrimaryIndex() const;
	virtual const SHVDataRowKey* GetIndex(size_t IdxID) const;
	virtual const size_t IndexCount() const;
	virtual SHVBool IsEqual(const SHVDataStructC* dataStruct) const;

	inline SHVBool TableExists() { return !Struct.IsNull(); }


protected:
	virtual ~SHVDataStructCSQLite();


private:
	void ResolveIndexes(SHVSQLiteWrapper* psqlite, const SHVStringUTF8C& tableName, const SHVStringUTF8C& condition);

	// data
	SHVDataStructImplRef Struct;
};
typedef SHVRefObjectContainer<SHVDataStructColumnC> SHVDataStructColumnCRef;

#endif