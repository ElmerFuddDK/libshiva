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
/// SHVDataRow class - Interface for SHVDataRow
/**
 * Interface for a generic shiva datarow
 */

class SHVDataRowC_SQLite: public SHVDataRowC
{
protected:
	SHVDataRowC_SQLite(SHVDataRowListC* select);
	virtual ~SHVDataRowC_SQLite();
public:
	virtual SHVStringBuffer AsString(const SHVString8C& colName) const;
	virtual SHVInt AsInt(const SHVString8C& colName) const;
	virtual SHVDouble AsDouble(const SHVString8C& colName) const;
	virtual SHVTime AsTime(const SHVString8C& colName) const;
	virtual SHVBool AsBool(const SHVString8C& colName) const;
	virtual SHVBool IsNull(const SHVString8C& colName) const;

	SHVStringBuffer AsString(size_t colIdx) const;
	SHVInt AsInt(size_t colIdx) const;
	SHVDouble AsDouble(size_t colIdx) const;
	SHVTime AsTime(size_t colIdx) const;
	SHVBool AsBool(size_t colIdx) const;
	SHVBool IsNull(size_t colIdx) const;

	virtual SHVDataRowKey* GetKey(size_t index = 0) const;
	virtual SHVDataVariant* GetValue(const SHVString8C& colName) const;
	virtual SHVDataVariant* GetValue(size_t colIdx) const;
	virtual SHVBool MatchKey(const SHVDataRowKey* key) const;
	virtual SHVBool RowValid() const;
	virtual void ClearOwnership();
protected:
	virtual size_t ColIndex(const SHVString8C& colName, bool& found) const;
private:
friend class SHVDataRowList_SQLite;
friend class SHVDataRowListC_SQLite;
	SHVDataRowListC* Select;
};
typedef SHVRefObjectContainer<SHVDataRowC_SQLite> SHVDataRowC_SQLiteRef;

#endif
