#ifndef __SHIVA_DATAENGINE_DATAROWLIST_H
#define __SHIVA_DATAENGINE_DATAROWLIST_H

#include "shvdatarowlistc.h"
#include "shvdatarow.h"
#include "shvdatavariant.h"

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
	virtual SHVBool IsOk() const = 0;
	virtual const SHVDataStructC* GetStruct() const = 0;

	virtual SHVDataRowC* Find(const SHVDataRowKey* key) = 0;
	virtual SHVBool NextRow() = 0;
	virtual SHVDataRowListC* Reverse(const SHVStringC& condition) = 0;
	virtual SHVBool Reset() = 0;

	virtual const void* GetRowProvider() const = 0;
	virtual SHVDataSession* GetDataSession() = 0;

	virtual SHVDataRow* EditCurrentRow() = 0;
	virtual SHVDataRow* AddRow() = 0;	
};
typedef SHVRefObjectContainer<SHVDataRowList> SHVDataRowListRef;

// ==================================== implementation - SHVDataRowList ==================================== //


#endif