#ifndef __SHIVA_DATAENGINE_DATAROWLISTC_H
#define __SHIVA_DATAENGINE_DATAROWLISTC_H

#include "..\..\..\include\utils\shvrefobject.h"
#include "..\..\..\include\utils\shvstring.h"
#include "..\..\..\include\shvtypes.h"
#include "shvdatarowc.h"
#include "shvdatastructc.h"
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
	virtual SHVBool NextRow() = 0;
	virtual SHVBool Reset() = 0;
	virtual const SHVDataStructC* GetStruct() const = 0;
	virtual const void* GetProvider() const = 0;
	virtual SHVBool IsOk() const = 0;
};
typedef SHVRefObjectContainer<SHVDataRowListC> SHVDataRowListCRef;


// ==================================== implementation - SHVDataRowList ==================================== //


#endif