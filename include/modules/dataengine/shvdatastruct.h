#ifndef __SHIVA_DATAENGINE_DATASTRUCT_H
#define __SHIVA_DATAENGINE_DATASTRUCT_H

#include "shvdatastructc.h"

//-=========================================================================================================
///  SHVDataStructColumn class - The non const interface for a datastruct column definition.
/**
 */
class SHVDataStructColumn: public SHVDataStructColumnC
{
public:
	virtual ~SHVDataStructColumn() {};

	virtual const SHVString8C& GetColumnName() const = 0;
	virtual void SetColumnName(const SHVString8C& name) = 0;

	virtual int GetDataType() const = 0;
	virtual void SetDataType(int dataType) = 0;

	virtual int GetDataLength() const = 0;
	virtual void SetDataLength(int len) = 0;

	virtual SHVBool GetAllowNull() const = 0;
	virtual void SetAllowNull(SHVBool flag) = 0;

	virtual SHVBool GetAutoInc() const = 0;
	virtual void SetAutoInc(SHVBool flag) = 0;
};

//-=========================================================================================================
///  SHVDataStruct class - The non const interface for a datastruct definition.
/**
 */
class SHVDataStruct: public SHVDataStructC
{
public:
	virtual void SetTableName(const SHVString8C& name) = 0;
	virtual void SetIsMultiInstance(bool isMultiInstance) = 0;

	virtual SHVDataStructColumn* operator[](size_t idx) = 0;

	virtual SHVDataStructColumn* CreateNew() = 0;

	virtual void InsertAt(size_t idx, SHVDataStructColumn& col) = 0;
	virtual void RemoveAt(size_t idx) = 0;
	virtual size_t Add(SHVDataStructColumn* col) = 0;
	virtual size_t Add(const SHVString8C& colName, int dataType, int dataLength = -1, SHVBool PrimaryKey = SHVBool::False, SHVBool AllowNull = SHVBool::False, SHVBool AutoInc = SHVBool::False) = 0;
	virtual SHVDataRowKey* CreateIndexKey() const = 0;
	virtual size_t AddIndex(SHVDataRowKey* index) = 0;
	virtual size_t RemoveLastIndex() = 0;

	virtual void SetPrimaryIndex(const SHVDataRowKey* key) = 0;

	// from SHVDataStructC
	virtual const SHVString8C& GetTableName() const = 0;
	virtual bool GetIsMultiInstance() const = 0;
	virtual const SHVDataStructColumnC* operator[](size_t idx) const = 0;
	virtual SHVBool FindColumnIndex(size_t& index, const SHVString8C& colName) const = 0;
	virtual size_t GetColumnCount() const = 0;
	virtual const SHVDataRowKey* GetPrimaryIndex() const = 0;
	virtual const SHVDataRowKey* GetIndex(size_t IdxID) const = 0;
	virtual size_t IndexCount() const = 0;
	virtual SHVBool IsEqual(const SHVDataStructC* dataStruct) const = 0;
	virtual SHVBool IsIndexesEqual(const SHVDataStructC* dataStruct) const = 0;
};
typedef SHVRefObjectContainer<SHVDataStruct> SHVDataStructRef;

#endif
