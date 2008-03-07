#ifndef __SHIVA_DATAENGINE_DATAROWLISTC_H
#define __SHIVA_DATAENGINE_DATAROWLISTC_H

#include "../../../include/utils/shvrefobject.h"
#include "../../../include/utils/shvstring.h"
#include "../../../include/shvtypes.h"

// forward declares
class SHVDataRow;
class SHVDataRowC;
class SHVDataStructC;
class SHVDataRowList;
class SHVDataSession;
class SHVDataRowKey;
class SHVDataFactory;

//-=========================================================================================================
/// SHVDataRowList class - Interface for SHVDataRowList
/**
 * Interface for a generic shiva datarow list
 */
class SHVDataRowListC: public SHVRefObject
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
	virtual SHVBool RowListValid() const = 0;
protected:
	friend class SHVDataFactory;
	friend class SHVDataRowList;
	virtual void AdjustRowCount(int delta) = 0;
	virtual SHVBool TempReset() = 0;
	virtual void Reposition() = 0;
	virtual SHVBool InternalRowChanged(SHVDataRow* row) = 0;
	virtual ~SHVDataRowListC() {}
	inline void UnregisterDataList();
	inline SHVBool TempReset(SHVDataRowListC* list);
	inline void Reposition(SHVDataRowListC* list);
	inline SHVBool InternalRowChanged(SHVDataRowListC* list, SHVDataRow* row);
private:
	int RowCount;
};
typedef SHVRefObjectContainer<SHVDataRowListC> SHVDataRowListCRef;

#endif


// =========================================== implementation ============================================ //

#ifndef __SHIVA_DATAENGINE_DATAROWLISTC_INL
#define __SHIVA_DATAENGINE_DATAROWLISTC_INL

#include "shvdatasession.h"
// #include "shvdatarow.h"

/*************************************
 * UnregisterDataList
 *************************************/
void SHVDataRowListC::UnregisterDataList()
{ 
	if (GetDataSession())
		GetDataSession()->UnregisterDataList(this);   
}

/*************************************
 * TempReset
 *************************************/
SHVBool SHVDataRowListC::TempReset(SHVDataRowListC* list)
{
	return list->TempReset();
}

/*************************************
 * Reposition
 *************************************/
void SHVDataRowListC::Reposition(SHVDataRowListC* list)
{
	list->Reposition();
}

/*************************************
 * RowChanged
 *************************************/
SHVBool SHVDataRowListC::InternalRowChanged(SHVDataRowListC* list, SHVDataRow* row)
{
	return list->InternalRowChanged(row);
}

#endif