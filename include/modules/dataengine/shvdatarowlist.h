#ifndef __SHIVA_DATAENGINE_DATAROWLIST_H
#define __SHIVA_DATAENGINE_DATAROWLIST_H

#include "shvdatarowlistc.h"
#include "shvdatarow.h"
//-=========================================================================================================
/// SHVDataRowListC class - Interface for SHVDataRowList
/**
 * Interface for a generic shiva datarow list
 */

class SHVDataRowList: public SHVDataRowListC
{
protected:
	~SHVDataRowList() {}
public:
	virtual const SHVDataRowC* GetCurrentRow() const = 0;
	virtual SHVBool NextRow() = 0;
	virtual SHVBool Reset() = 0;
	virtual SHVDataRow* EditCurrentRow() = 0;
	virtual SHVDataRow* AddRow() = 0;
	virtual SHVBool CommitChanges() = 0;
	virtual SHVBool CancelChanges() = 0;
	virtual const SHVDataRowVector* GetChangedRows() = 0;
	virtual SHVBool AcceptChanges(SHVDataRow* dataRow) = 0;
	virtual SHVBool RejectChanges(SHVDataRow* dataRow) = 0;
	virtual const SHVDataStructC* GetStruct() const = 0;
	virtual const void* GetProvider() const = 0;
	virtual SHVBool IsOk() const = 0;
};
typedef SHVRefObjectContainer<SHVDataRowList> SHVDataRowListRef;

// ==================================== implementation - SHVDataRowList ==================================== //


#endif