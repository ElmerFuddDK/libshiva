#ifndef __SHIVA_DATAENGINE_DATAROWLISTC_H
#define __SHIVA_DATAENGINE_DATAROWLISTC_H

#include "../../../include/utils/shvrefobject.h"
#include "../../../include/utils/shvstring.h"
#include "../../../include/shvtypes.h"
#include "shvdatarowc.h"
#include "shvdatastructc.h"

// forward declare
class SHVDataSession;
//-=========================================================================================================
/// SHVDataRowList class - Interface for SHVDataRowList
/**
 * Interface for a generic shiva datarow list
 */
class SHVDataRowListC: public SHVRefObject
{
protected:
	~SHVDataRowListC() {}
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
};
typedef SHVRefObjectContainer<SHVDataRowListC> SHVDataRowListCRef;


// ==================================== implementation - SHVDataRowList ==================================== //


#endif