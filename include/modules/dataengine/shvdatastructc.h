#ifndef __SHIVA_DATAENGINE_DATASTRUCTC_H
#define __SHIVA_DATAENGINE_DATASTRUCTC_H

#include "../../../include/utils/shvrefobject.h"
#include "../../../include/utils/shvstring.h"
#include "shvdatarowkey.h"

//-=========================================================================================================
///  SHVDataStructColumnC class - The const interface for a datastruct column definition.
/**
 */
class SHVDataStructColumnC: public SHVRefObject
{
protected:
	virtual ~SHVDataStructColumnC() { }
public:
	virtual const SHVString8C& GetColumnName() const = 0;
	virtual int GetDataType() const = 0;
	virtual int GetDataLength() const = 0;
	virtual SHVBool GetPrimaryKey() const = 0;
	virtual SHVBool GetAllowNull() const = 0;
	virtual SHVBool GetAutoInc() const = 0;
	virtual void ClearOwnership() = 0;
};
typedef SHVRefObjectContainer<SHVDataStructColumnC> SHVDataStructColumnCRef;

//-=========================================================================================================
///  SHVDataStructC class - The const interface for a datastruct definition.
/**
 */
class SHVDataStructC: public SHVRefObject
{
protected:
	virtual ~SHVDataStructC() { }
public:
	virtual const SHVString8C& GetTableName() const = 0;
	virtual const SHVDataStructColumnC* operator[](size_t idx) const = 0;
	virtual const SHVBool FindColumnIndex(size_t& index, const SHVString8C& colName) const = 0;
	virtual size_t GetColumnCount() const = 0;
	virtual const SHVDataRowKey* GetPrimaryIndex() const = 0;
	virtual const SHVDataRowKey* GetIndex(size_t IdxID) const = 0;
	virtual const size_t IndexCount() const = 0;
	virtual SHVBool IsEqual(const SHVDataStructC* dataStruct) const = 0;
};
typedef SHVRefObjectContainer<SHVDataStructC> SHVDataStructCRef;


#endif