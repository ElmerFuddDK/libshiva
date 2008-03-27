#ifndef __SHIVA_DATAENGINE_DATAROW_IMPL_H
#define __SHIVA_DATAENGINE_DATAROW_IMPL_H

#include "../../../include/utils/shvrefobject.h"
#include "../../../include/utils/shvstring.h"
#include "../../../include/shvtypes.h"
#include "../../../include/utils/shvtime.h"
#include "../../../include/sqlite/sqlitewrapper.h"
#include "../../../include/modules/dataengine/shvdatarow.h"
#include "shvdatavariantimpl.h"
#include "shvdatarowlistsqlite.h"

//-=========================================================================================================
/// SHVDataRowImpl class - Implementation for the datarow
/**
 * A generic row class which is a copy a SHVDataRowC object that is editable.
 */
class SHVDataRowImpl: public SHVDataRow
{
public:
	struct RowValues
	{
		SHVDataVariantImpl Value;
		SHVDataVariantImpl	OrgValue;
	};

	SHVDataRowImpl(const SHVDataRowC* copyrow, SHVDataRowList* owner);
	SHVDataRowImpl(SHVDataRowList* owner);

	virtual SHVStringBuffer AsString(size_t colIdx) const;
	virtual void SetString(size_t colIdx, const SHVStringC& val);

	virtual SHVInt AsInt(size_t colIdx) const;
	virtual void SetInt(size_t colIdx, SHVInt val);

	virtual SHVDouble AsDouble(size_t colIdx) const;
	virtual void SetDouble(size_t colIdx, SHVDouble val);

	virtual SHVTime AsTime(size_t colIdx) const;
	virtual void SetTime(size_t colIdx, const SHVTime& time);

	virtual SHVBool AsBool(size_t colIdx) const;
	virtual void SetBool(size_t colIdx, SHVBool val);

	virtual SHVBool IsNull(size_t colIdx) const;
	virtual void SetNull(size_t colIdx);

	virtual size_t ColumnIndex(const SHVString8C& colName) const;

	virtual SHVDataVariant* GetValue(size_t colIdx) const;
	virtual const SHVDataVariant* OriginalValue(size_t colIdx) const;

	virtual SHVDataRowKey* GetKey(size_t index = 0) const;
	virtual SHVBool MatchKey(const SHVDataRowKey* key) const;
	virtual SHVBool RowValid() const;
	virtual const SHVDataStructC* GetStruct() const;
	virtual const SHVString8C GetAlias() const;

	virtual int GetRowState() const;
	virtual const SHVDataRowListC* GetRowListC();

	virtual SHVBool Delete();
	virtual SHVBool HasChanges();

protected:
	virtual ~SHVDataRowImpl();
	inline void SetChanged();
	virtual SHVDataRowList* GetRowList();
	virtual void InternalAcceptChanges();
	virtual void InternalRejectChanges();
private:
	friend class SHVDataRowListSQLite;
	RowValues* ColumnData;
	SHVDataRowList* Owner;
	int RowState;
	int OrgRowState;
};
typedef SHVRefObjectContainer<SHVDataRowImpl> SHVDataRowImplRef;

// ==================================== implementation - SHVDataRowImpl =================================== //
/*************************************
 * SetChanged
 *************************************/
void SHVDataRowImpl::SetChanged()
{
	if (RowState != SHVDataRow::RowStateAdding)
		RowState = SHVDataRow::RowStateChanging;
}

#endif
