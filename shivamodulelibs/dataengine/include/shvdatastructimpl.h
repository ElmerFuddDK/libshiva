#ifndef __SHIVA_DATAENGINE_DATASTRUCT_IMPL_H
#define __SHIVA_DATAENGINE_DATASTRUCT_IMPL_H

#include "../../../../include/utils/shvvectorref.h"
#include "../shvdatastruct.h"

//-=========================================================================================================
/// SHVDataStructColumn_impl class - Implementation of a datastructure column definition.
/**
 * Contains the column definition information.
 */
class SHVDataStructColumn_impl: public SHVDataStructColumn
{
public:
	SHVDataStructColumn_impl();
	SHVDataStructColumn_impl(const SHVDataStructColumnC* col);
	virtual ~SHVDataStructColumn_impl();

	virtual const SHVString8C& GetColumnName() const;
	virtual void SetColumnName(const SHVString8C& name);

	virtual int GetDataType() const;
	virtual void SetDataType(int dataType);

	virtual int GetDataLength() const;
	virtual void SetDataLength(int len);

	virtual SHVBool GetAllowNull() const;
	virtual void SetAllowNull(SHVBool flag);

	virtual SHVBool GetAutoInc() const;
	virtual void SetAutoInc(SHVBool flag);

	virtual void ClearOwnership();
private:
	SHVString8 ColumnName;
	int DataType;
	int DataLength;
	SHVBool AllowNull;
	SHVBool AutoInc;
};
typedef SHVRefObjectContainer<SHVDataStructColumn_impl> SHVDataStructColumn_implRef;

//-=========================================================================================================
///  SHVDataStruct_impl class - The non const implementation of datastruct
/**
 * Objects of the class is the schema for the datatable.
 */
class SHVDataStruct_impl: public SHVDataStruct
{
public:
	SHVDataStruct_impl();
	SHVDataStruct_impl(const SHVDataStructC* dataStruct);
	virtual ~SHVDataStruct_impl();
	virtual const SHVString8C& GetTableName() const;
	virtual void SetTableName(const SHVString8C& name);

	virtual const SHVDataStructColumnC* operator[](size_t idx) const;
	virtual SHVDataStructColumn* operator[](size_t idx);
	virtual const SHVBool FindColumnIndex(size_t& index, const SHVString8C& colName) const;

	virtual SHVDataStructColumn* CreateNew();
	virtual void InsertAt(size_t idx, SHVDataStructColumn& col);
	virtual SHVDataStructColumn* RemoveAt(size_t idx);
	virtual size_t Add(SHVDataStructColumn* col);
	virtual size_t Add(const SHVString8C& colName, int dataType, int dataLength = -1, SHVBool PrimaryKey = SHVBool::False, SHVBool AllowNull = SHVBool::False, SHVBool AutoInc = SHVBool::False);
	virtual size_t GetColumnCount() const;
	virtual const SHVDataRowKey* GetPrimaryIndex() const;
	virtual void SetPrimaryIndex(const SHVDataRowKey* key);
	virtual const SHVDataRowKey* GetIndex(size_t IdxID) const;
	virtual const size_t IndexCount() const;
	virtual SHVDataRowKey* CreateIndexKey() const;
	virtual void AddIndex(SHVDataRowKey* index);
	virtual SHVBool IsEqual(const SHVDataStructC* dataStruct) const;

private:
	SHVVectorRef<SHVDataStructColumn> Columns;
	SHVVectorRef<SHVDataRowKey> Indexes;
	SHVString8 TableName;
};
typedef SHVRefObjectContainer<SHVDataStruct_impl> SHVDataStruct_implRef;

#endif