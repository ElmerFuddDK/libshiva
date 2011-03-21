#ifndef __SHIVA_DATAENGINE_DATASTRUCTC_H
#define __SHIVA_DATAENGINE_DATASTRUCTC_H

#include "../../utils/shvrefobject.h"
#include "../../utils/shvstring.h"
#include "../../utils/shvvectorref.h"
#include "../../utils/shvlist.h"
#include "shvdatarowkey.h"

// forward declare
class SHVDataStruct;
class SHVDataFactory;

//-=========================================================================================================
///  SHVDataStructColumnC class - The const interface for a datastruct column definition.
/**
 */
class SHVDataStructColumnC: public SHVRefObject
{
public:
	virtual const SHVString8C& GetColumnName() const = 0;
	virtual int GetDataType() const = 0;
	virtual int GetDataLength() const = 0;
	virtual SHVBool GetAllowNull() const = 0;
	virtual SHVBool GetAutoInc() const = 0;


protected:
	virtual ~SHVDataStructColumnC() { }
};
typedef SHVRefObjectContainer<SHVDataStructColumnC> SHVDataStructColumnCRef;

//-=========================================================================================================
///  SHVDataStructC class - The const interface for a datastruct definition.
/**
 */
class SHVDataStructC: public SHVRefObject
{
public:
	virtual const SHVString8C& GetTableName() const = 0;
	virtual bool GetIsMultiInstance() const = 0;
	virtual const SHVDataStructColumnC* operator[](size_t idx) const = 0;
	virtual SHVBool FindColumnIndex(size_t& index, const SHVString8C& colName) const = 0;
	virtual size_t GetColumnCount() const = 0;
	virtual const SHVDataRowKey* GetPrimaryIndex() const = 0;
	virtual const SHVDataRowKey* GetIndex(size_t IdxID) const = 0;
	virtual size_t FindIndex(const SHVDataRowKey* key) const = 0;
	virtual size_t IndexCount() const = 0;
	virtual SHVBool IsEqual(const SHVDataStructC* dataStruct) const = 0;
	virtual SHVBool IsIndexesEqual(const SHVDataStructC* dataStruct) const = 0;


protected:
friend class SHVDataFactory;


	virtual ~SHVDataStructC() { }
	virtual SHVDataStruct* GetInternalStruct() = 0;
};
typedef SHVRefObjectContainer<SHVDataStructC> SHVDataStructCRef;
typedef	SHVVectorRef<SHVDataStructC> SHVDataSchema;

#endif
