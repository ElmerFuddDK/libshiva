#ifndef __SHIVA_DATAENGINE_DATAROW_IMPL_H
#define __SHIVA_DATAENGINE_DATAROW_IMPL_H

#include "../../../../include/utils/shvrefobject.h"
#include "../../../../include/utils/shvstring.h"
#include "../../../../include/shvtypes.h"
#include "../../../../include/utils/shvtime.h"
#include "../../../../shivasqlite/include/sqlitewrapper.h"
#include "../shvdatarow.h"
#include "shvdatavariant_impl.h"
#include "shvdatarowlist_sqlite.h"

//-=========================================================================================================
/// SHVDataRow_impl class - Interface for SHVDataRow
/**
 * Interface for a generic shiva datarow
 */

class SHVDataRow_impl: public SHVDataRow
{
public:
	struct KeyValuePair
	{
		SHVString8			Name;
		SHVDataVariant_impl Value;
		SHVDataVariant_impl	OrgValue;
	};

	SHVDataRow_impl(SHVDataRowC* copyrow, SHVDataRowList* owner);
	SHVDataRow_impl(SHVDataRowList* owner);

	virtual SHVStringBuffer AsString(const SHVString8C& colName) const;
	virtual void SetValue(const SHVString8C& colName, const SHVStringC& val);

	virtual SHVInt AsInt(const SHVString8C& colName) const;
	virtual void SetValue(const SHVString8C& colName, SHVInt val);

	virtual SHVDouble AsDouble(const SHVString8C& colName) const;
	virtual void SetValue(const SHVString8C& colName, SHVDouble val);

	virtual SHVTime AsTime(const SHVString8C& colName) const;
	virtual void SetValue(const SHVString8C& colName, const SHVTime& time);

	virtual SHVBool AsBool(const SHVString8C& colName) const;
	virtual void SetValue(const SHVString8C& colName, SHVBool val);

	virtual SHVBool IsNull(const SHVString8C& colName) const;
	virtual void SetNull(const SHVString8C& colName);

	virtual SHVDataVariant* GetValue(const SHVString8C& colName) const;
	virtual SHVDataVariant* GetValue(size_t colIdx) const;
	virtual const SHVDataVariant* OrginalValue(const SHVString8C& colName) const;

	virtual SHVDataRowKey* GetKey(size_t index = 0) const;
	virtual SHVBool MatchKey(const SHVDataRowKey* key) const;
	virtual SHVBool RowValid() const;
	virtual int GetRowState() const;

	virtual SHVBool Delete();
	virtual SHVBool AcceptChanges();
	virtual SHVBool RejectChanges();
	virtual SHVBool HasChanges();
	virtual void ClearOwnership();

protected:
	virtual ~SHVDataRow_impl();
	virtual KeyValuePair* Find(const SHVString8C& columnName) const;
private:
	friend class SHVDataRowList_SQLite;
	KeyValuePair* ColumnData;
	SHVDataRowList* Owner;
	int RowState;
};
typedef SHVRefObjectContainer<SHVDataRow_impl> SHVDataRow_implRef;

#endif
