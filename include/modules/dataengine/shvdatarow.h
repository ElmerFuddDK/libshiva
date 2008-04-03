#ifndef __SHIVA_DATAENGINE_DATAROW_H
#define __SHIVA_DATAENGINE_DATAROW_H

#include "../../shvtypes.h"
#include "../../utils/shvrefobject.h"
#include "../../utils/shvstring.h"
#include "../../utils/shvvectorref.h"
#include "../../utils/shvlist.h"
#include "../../framework/shveventdata.h"
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

	virtual void SetString(size_t colIdx, const SHVStringC& val) = 0;
	virtual void SetInt(size_t colIdx, SHVInt val) = 0;
	virtual void SetDouble(size_t colIdx, SHVDouble val) = 0;
	virtual void SetTime(size_t colIdx, const SHVTime& time) = 0;
	virtual void SetBool(size_t colIdx, SHVBool val) = 0;
	virtual void SetNull(size_t colIdx) = 0;
	virtual const SHVDataVariant* OriginalValue(size_t colIdx) const = 0;
	virtual void SetCorr(const SHVDataRowC* row) = 0;

	virtual SHVDataRowList* GetRowList() = 0;

	virtual SHVBool Delete() = 0;
	virtual SHVBool HasChanges() = 0;

	// from SHVDataRowC
	virtual SHVStringBuffer AsString(size_t colIdx) const = 0;
	virtual SHVInt AsInt(size_t colIdx) const = 0;
	virtual SHVDouble AsDouble(size_t colIdx) const = 0;
	virtual SHVTime AsTime(size_t colIdx) const = 0;
	virtual SHVBool AsBool(size_t colIdx) const = 0;
	virtual SHVBool IsNull(size_t colIdx) const = 0;

	virtual size_t ColumnIndex(const SHVString8C& colName) const = 0;

	virtual SHVDataRowKey* GetKey(size_t index = 0) const = 0;
	
	virtual SHVDataVariant* GetValue(size_t colIdx) const = 0;

	virtual SHVBool MatchKey(const SHVDataRowKey* key) const = 0;
	virtual SHVBool RowValid() const = 0;
	virtual const SHVDataStructC* GetStruct() const = 0;
	virtual const SHVString8C GetAlias() const = 0;

	virtual int GetRowState() const = 0;
	virtual const SHVDataRowListC* GetRowListC() = 0;

	// inlines
	inline void SetString(const SHVString8C& colName, const SHVStringC& val);
	inline void SetInt(const SHVString8C& colName, SHVInt val);
	inline void SetDouble(const SHVString8C& colName, SHVDouble val);
	inline void SetTime(const SHVString8C& colName, const SHVTime& time);
	inline void SetBool(const SHVString8C& colName, const SHVBool val);
	inline void SetNull(const SHVString8C& colName);
	inline const SHVDataVariant* OriginalValue(const SHVString8C& colName) const;

	inline SHVBool AcceptChanges();
	inline SHVBool RejectChanges();
protected:
	friend class SHVDataRowList;
	virtual ~SHVDataRow() { }
	virtual void InternalAcceptChanges() = 0;
	virtual void InternalRejectChanges() = 0;
};

// typedefs
typedef SHVRefObjectContainer<SHVDataRow> SHVDataRowRef;
typedef SHVVectorRef<SHVDataRow> SHVDataRowVector;
typedef SHVList<SHVDataRowRef, SHVDataRow*> SHVDataRowCollection;
typedef SHVPtrContainer<SHVDataRowVector> SHVDataRowVectorPtr;

// event types involving SHVDataRow
typedef SHVEventData<SHVDataRowRef, SHVDataRow*> SHVEventDataRowChanged;
typedef SHVEventData<SHVDataRowVectorPtr> SHVEventDataChangeSet;

#endif

// ============================================= implementation =========================================== //
#ifndef __SHIVA_DATAENGINE_DATAROW_INL
#define __SHIVA_DATAENGINE_DATAROW_INL

#include "shvdatarowlist.h"
/*************************************
 * Set*
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
	return GetRowList()->RejectChanges(this);
}

#endif