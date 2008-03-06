#ifndef __SHIVA_DATAENGINE_DATAROWLIST_H
#define __SHIVA_DATAENGINE_DATAROWLIST_H

#include "shvdatarowlistc.h"
#include "shvdatavariant.h"

// forward declare
class SHVDataRow;

//-=========================================================================================================
/// SHVDataRowListC class - Interface for SHVDataRowList
/**
 * Interface for a generic shiva datarow list
 */
class SHVDataRowList: public SHVDataRowListC
{
public:
	virtual const SHVDataRowC* GetCurrentRow() const = 0;
	virtual SHVBool IsOk() const = 0;
	virtual const SHVDataStructC* GetStruct() const = 0;
	virtual int GetRowCount() const = 0;

	virtual SHVDataRowC* Find(const SHVDataRowKey* key) = 0;
	virtual SHVBool NextRow() = 0;
	virtual SHVDataRowListC* Reverse(const SHVStringC& condition) = 0;
	virtual SHVBool Reset() = 0;

	virtual const void* GetRowProvider() const = 0;
	virtual SHVDataSession* GetDataSession() = 0;

	virtual SHVDataRow* EditCurrentRow() = 0;
	virtual SHVDataRow* AddRow() = 0;	
	virtual void EnableEvents(bool enable) = 0;
	virtual bool GetEventsEnabled() const = 0;
	virtual void EnableNonAccepted(bool enable) = 0;
	virtual bool GetNonAcceptedEnabled() const = 0;

	virtual SHVBool RowListValid() const = 0;
protected:
	friend class SHVDataRow;
	virtual ~SHVDataRowList() {}
	virtual SHVBool AcceptChanges(SHVDataRow* row) = 0;
	virtual SHVBool RejectChanges(SHVDataRow* row) = 0;

	inline SHVBool UpdateRow(SHVDataRow* row);
	inline void RowChanged(SHVDataRow* row);
	inline void InternalAcceptChanges(SHVDataRow* row);
	inline void InternalRejectChanges(SHVDataRow* row);
	inline void InternalAdjustRowCount(SHVDataRowListC* rowList, int delta);
	virtual void AdjustRowCount(int delta) = 0;
};
typedef SHVRefObjectContainer<SHVDataRowList> SHVDataRowListRef;
#endif

// ==================================== implementation - SHVDataRowList ==================================== //
#ifndef __SHIVA_DATAENGINE_DATAROWLIST_INL
#define __SHIVA_DATAENGINE_DATAROWLIST_INL

#include "shvdatasession.h"
#include "shvdatarow.h"

/*************************************
 * UpdateRow
 *************************************/
SHVBool SHVDataRowList::UpdateRow(SHVDataRow* row)
{ 
	return GetDataSession()->UpdateRow(row); 
}

/*************************************
 * RowChanged
 *************************************/
void SHVDataRowList::RowChanged(SHVDataRow* row)
{
	if (GetEventsEnabled())
		GetDataSession()->RowChanged(row); 
}
/*************************************
 * InternalAcceptChanges
 *************************************/
void SHVDataRowList::InternalAcceptChanges(SHVDataRow* row)
{
	row->InternalAcceptChanges();
}

/*************************************
 * InternalRejectChanges
 *************************************/
void SHVDataRowList::InternalRejectChanges(SHVDataRow* row)
{
	row->InternalRejectChanges();
}
/*************************************
 * AdjustRowCount
 *************************************/
void SHVDataRowList::InternalAdjustRowCount(SHVDataRowListC* rowList, int delta)
{
	rowList->AdjustRowCount(delta);
}

#endif
