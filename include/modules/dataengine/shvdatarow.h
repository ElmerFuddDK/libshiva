#ifndef __SHIVA_DATAENGINE_DATAROW_H
#define __SHIVA_DATAENGINE_DATAROW_H

#include "../../../include/utils/shvrefobject.h"
#include "../../../include/utils/shvstring.h"
#include "../../../include/utils/shvvectorref.h"
#include "../../../include/utils/shvlist.h"
#include "../../../include/framework/shveventdata.h"
#include "../../../include/shvtypes.h"
#include "shvdatarowc.h"

//forward declares
class SHVDataRowList;

//-=========================================================================================================
/// SHVDataRow class - Interface for SHVDataRow
/**
 * Interface for a generic shiva datarow
 */
class SHVDataRow: public SHVDataRowC
{
public:
	enum SHVDataRowState
	{
		RowStateInvalid = -1,
		RowStateDeleted,
		RowStateAdded,
		RowStateChanged,
		RowStateUnchanged,
		RowStateDeleting,
		RowStateAdding,
		RowStateChanging
	};
	virtual SHVStringBuffer AsString(size_t colIdx) const = 0;
	virtual void SetString(size_t colIdx, const SHVStringC& val) = 0;
	inline void SetString(const SHVString8C& colName, const SHVStringC& val);

	virtual SHVInt AsInt(size_t colIdx) const = 0;
	virtual void SetInt(size_t colIdx, SHVInt val) = 0;
	inline void SetInt(const SHVString8C& colName, SHVInt val);

	virtual SHVDouble AsDouble(size_t colIdx) const = 0;
	virtual void SetDouble(size_t colIdx, SHVDouble val) = 0;
	inline void SetDouble(const SHVString8C& colName, SHVDouble val);

	virtual SHVTime AsTime(size_t colIdx) const = 0;
	virtual void SetTime(size_t colIdx, const SHVTime& time) = 0;
	inline void SetTime(const SHVString8C& colName, const SHVTime& time);

	virtual SHVBool AsBool(size_t colIdx) const = 0;
	virtual void SetBool(size_t colIdx, SHVBool val) = 0;
	inline void SetBool(const SHVString8C& colName, const SHVBool val);

	virtual SHVBool IsNull(size_t colIdx) const = 0;
	virtual void SetNull(size_t colIdx) = 0;
	inline void SetNull(const SHVString8C& colName);

	virtual size_t ColumnIndex(const SHVString8C& colName) const = 0;

	virtual SHVDataVariant* GetValue(size_t colIdx) const = 0;
	virtual const SHVDataVariant* OriginalValue(size_t colIdx) const = 0;
	inline const SHVDataVariant* OriginalValue(const SHVString8C& colName) const;

	virtual SHVDataRowKey* GetKey(size_t index = 0) const = 0;
	virtual SHVBool MatchKey(const SHVDataRowKey* key) const = 0;
	virtual SHVBool RowValid() const  = 0; 
	virtual const SHVDataStructC* GetStruct() const = 0;
	virtual const SHVString8C GetAlias() const = 0;

	virtual int GetRowState() const = 0;

	virtual SHVBool Delete() = 0;
	inline SHVBool AcceptChanges();
	inline SHVBool RejectChanges();
	virtual SHVBool HasChanges() = 0;

protected:
	friend class SHVDataRowList;
	virtual SHVDataRowList* GetRowList() = 0;
	virtual ~SHVDataRow() { }
	virtual void InternalAcceptChanges() = 0;
	virtual void InternalRejectChanges() = 0;
};

typedef SHVRefObjectContainer<SHVDataRow> SHVDataRowRef;
typedef SHVVectorRef<SHVDataRow> SHVDataRowVector;
typedef SHVList<SHVDataRowRef, SHVDataRow*> SHVDataRowCollection;
typedef SHVPtrContainer<SHVDataRowVector> SHVDataRowVectorPtr;

typedef SHVEventData<SHVDataRowRef, SHVDataRow*> SHVEventDataRowChanged;
typedef SHVEventData<SHVDataRowVectorPtr> SHVEventDataChangeSet;

#endif

// ====================================== implementation - SHVDataRow ===================================== //
#ifndef __SHIVA_DATAENGINE_DATAROW_INL
#define __SHIVA_DATAENGINE_DATAROW_INL

#include "shvdatarowlist.h"

/*************************************
 * SetValue
 *************************************/
void SHVDataRow::SetString(const SHVString8C& colName, const SHVStringC& val)
{
	SetString(ColumnIndex(colName), val);
}
void SHVDataRow::SetInt(const SHVString8C& colName, SHVInt val)
{
	SetInt(ColumnIndex(colName), val);
}
void SHVDataRow::SetDouble(const SHVString8C& colName, SHVDouble val)
{
	SetDouble(ColumnIndex(colName), val);
}
void SHVDataRow::SetTime(const SHVString8C& colName, const SHVTime& time)
{
	SetTime(ColumnIndex(colName), time);
}
void SHVDataRow::SetBool(const SHVString8C& colName, const SHVBool val)
{
	SetBool(ColumnIndex(colName), val);
}
void SHVDataRow::SetNull(const SHVString8C& colName)
{
	SetNull(ColumnIndex(colName));
}

/*************************************
 * OrginalValue
 *************************************/
const SHVDataVariant* SHVDataRow::OriginalValue(const SHVString8C& colName) const
{
	return OriginalValue(ColumnIndex(colName));
}

/*************************************
 * AcceptChanges
 *************************************/
SHVBool SHVDataRow::AcceptChanges()
{
	return GetRowList()->AcceptChanges(this);
}

/*************************************
 * RejectChanges
 *************************************/
SHVBool SHVDataRow::RejectChanges()
{
	return GetRowList()->AcceptChanges(this);
}

#endif