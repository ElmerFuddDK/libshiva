#ifndef __SHIVA_DATAENGINE_DATASTRUCT_IMPL_H
#define __SHIVA_DATAENGINE_DATASTRUCT_IMPL_H

#include "../../../include/utils/shvvectorref.h"
#include "../../../include/modules/dataengine/shvdatastruct.h"

//-=========================================================================================================
/// SHVDataStructColumnImpl class - Implementation of a datastructure column definition.
/**
 * Contains the column definition information.
 */
class SHVDataStructColumnImpl: public SHVDataStructColumn
{
public:
	SHVDataStructColumnImpl();
	SHVDataStructColumnImpl(const SHVDataStructColumnC* col);
	virtual ~SHVDataStructColumnImpl();

	// from SHVDataStructColumn
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


private:
	SHVString8 ColumnName;
	int DataType;
	int DataLength;
	SHVBool AllowNull;
	SHVBool AutoInc;
};
typedef SHVRefObjectContainer<SHVDataStructColumnImpl> SHVDataStructColumnImplRef;

//-=========================================================================================================
///  SHVDataStructImpl class - The non const implementation of datastruct
/**
 * Objects of the class is the schema for the datatable.
 */
class SHVDataStructImpl: public SHVDataStruct
{
public:
	SHVDataStructImpl();
	SHVDataStructImpl(const SHVDataStructC* dataStruct);
	virtual ~SHVDataStructImpl();
	virtual const SHVString8C& GetTableName() const;
	virtual void SetTableName(const SHVString8C& name);

	virtual bool GetIsMultiInstance() const;
	virtual void SetIsMultiInstance(bool isMultiInstance);

	virtual const SHVDataStructColumnC* operator[](size_t idx) const;
	virtual SHVDataStructColumn* operator[](size_t idx);
	virtual const SHVBool FindColumnIndex(size_t& index, const SHVString8C& colName) const;

	virtual SHVDataStructColumn* CreateNew();
	virtual void InsertAt(size_t idx, SHVDataStructColumn& col);
	virtual void RemoveAt(size_t idx);
	virtual size_t Add(SHVDataStructColumn* col);
	virtual size_t Add(const SHVString8C& colName, int dataType, int dataLength = -1, SHVBool PrimaryKey = SHVBool::False, SHVBool AllowNull = SHVBool::False, SHVBool AutoInc = SHVBool::False);
	virtual size_t GetColumnCount() const;
	virtual const SHVDataRowKey* GetPrimaryIndex() const;
	virtual void SetPrimaryIndex(const SHVDataRowKey* key);
	virtual const SHVDataRowKey* GetIndex(size_t IdxID) const;
	virtual const size_t IndexCount() const;
	virtual SHVDataRowKey* CreateIndexKey() const;
	virtual size_t AddIndex(SHVDataRowKey* index);
	virtual size_t RemoveLastIndex();
	virtual SHVBool IsEqual(const SHVDataStructC* dataStruct) const;
	virtual SHVBool IsIndexesEqual(const SHVDataStructC* dataStruct) const;

protected:
	// from SHVDataStructC
	virtual SHVDataStruct* GetInternalStruct();

private:
	SHVVectorRef<SHVDataStructColumn> Columns;
	SHVVectorRef<SHVDataRowKey> Indexes;
	SHVString8 TableName;
	bool IsMultiInstance;
};
typedef SHVRefObjectContainer<SHVDataStructImpl> SHVDataStructImplRef;

#endif