#ifndef __SHIVA_DATAENGINE_DATAROWC_SQLITE_H
#define __SHIVA_DATAENGINE_DATAROWC_SQLITE_H

#include "../../../../include/utils/shvrefobject.h"
#include "../../../../include/utils/shvstring.h"
#include "../../../../include/shvtypes.h"
#include "../../../../include/utils/shvtime.h"
#include "../../../../shivasqlite/include/sqlitewrapper.h"
#include "../shvdatarowc.h"
#include "shvdatarowlist_sqlite.h"

//-=========================================================================================================
/// SHVDataRowC_SQLite class - Implementation a sqlite datarow
/**
 * This class doesn't not contain any data, but retrieves it directly from a sqlite statement object.
 */
class SHVDataRowC_SQLite: public SHVDataRowC
{
protected:
	SHVDataRowC_SQLite(SHVDataRowListC* select);
	virtual ~SHVDataRowC_SQLite();
public:
	virtual SHVStringBuffer AsString(size_t colIdx) const;
	virtual SHVInt AsInt(size_t colIdx) const;
	virtual SHVDouble AsDouble(size_t colIdx) const;
	virtual SHVTime AsTime(size_t colIdx) const;
	virtual SHVBool AsBool(size_t colIdx) const;
	virtual SHVBool IsNull(size_t colIdx) const;

	virtual size_t ColumnIndex(const SHVString8C& colName) const;

	virtual SHVDataRowKey* GetKey(size_t index = 0) const;
	virtual SHVDataVariant* GetValue(size_t colIdx) const;
	virtual SHVBool MatchKey(const SHVDataRowKey* key) const;
	virtual SHVBool RowValid() const;
	virtual const SHVDataStructC* GetStruct() const;

private:
friend class SHVDataRowList_SQLite;
friend class SHVDataRowListC_SQLite;
	SHVDataRowListC* Select;
};
typedef SHVRefObjectContainer<SHVDataRowC_SQLite> SHVDataRowC_SQLiteRef;

#endif
