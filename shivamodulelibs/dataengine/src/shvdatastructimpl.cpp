#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../include/shvdatastructimpl.h"
#include "../include/shvdatarowkeyimpl.h"

/*************************************
 * Constructor
 *************************************/
SHVDataStructColumnImpl::SHVDataStructColumnImpl()
{
}

SHVDataStructColumnImpl::SHVDataStructColumnImpl(const SHVDataStructColumnC* col)
{
	SetColumnName(col->GetColumnName());
	SetDataType(col->GetDataType());
	SetDataLength(col->GetDataLength());
	SetAllowNull(col->GetAllowNull());
	SetAutoInc(col->GetAutoInc());
}

SHVDataStructColumnImpl::~SHVDataStructColumnImpl()
{
}

/*************************************
 * ClearOwnerShip
 *************************************/
void SHVDataStructColumnImpl::ClearOwnership()
{
}

/*************************************
 * GetColumnName
 *************************************/
const SHVString8C& SHVDataStructColumnImpl::GetColumnName() const
{
	return ColumnName;
}

/*************************************
 * SetColumnName
 *************************************/
void SHVDataStructColumnImpl::SetColumnName(const SHVString8C& name)
{
	ColumnName = name;
}

/*************************************
 * GetDataType
 *************************************/
int SHVDataStructColumnImpl::GetDataType() const
{
	return DataType;
}

/*************************************
 * SetDataType
 *************************************/
void SHVDataStructColumnImpl::SetDataType(int dataType)
{
	DataType = dataType;
}

/*************************************
 * GetDataLength
 *************************************/
int SHVDataStructColumnImpl::GetDataLength() const
{
	return DataLength;
}

/*************************************
 * SetDataLength
 *************************************/
void SHVDataStructColumnImpl::SetDataLength(int len)
{
	DataLength = len;
}

/*************************************
 * GetAllowNull
 *************************************/
SHVBool SHVDataStructColumnImpl::GetAllowNull() const
{
	return AllowNull;
}

/*************************************
 * SetAllowNull
 *************************************/
void SHVDataStructColumnImpl::SetAllowNull(SHVBool flag)
{
	AllowNull = flag;
}

/*************************************
 * GetAutoInc
 *************************************/
SHVBool SHVDataStructColumnImpl::GetAutoInc() const
{
	return AutoInc;
}

/*************************************
 * SetAutoInc
 *************************************/
void SHVDataStructColumnImpl::SetAutoInc(SHVBool flag)
{
	AutoInc = flag;
}

/*************************************
 * Constructor
 *************************************/
SHVDataStructImpl::SHVDataStructImpl()
{
}
	
SHVDataStructImpl::SHVDataStructImpl(const SHVDataStructC* dataStruct)
{
	for (size_t i = 0; i < dataStruct->GetColumnCount(); i++)
	{
		Add(new SHVDataStructColumnImpl((const SHVDataStructColumnC*)(*dataStruct)[i]));
	}
	SetTableName(dataStruct->GetTableName());
	for (size_t i = 0; i < dataStruct->IndexCount(); i++)
		AddIndex((SHVDataRowKey*) dataStruct->GetIndex(i));
}

SHVDataStructImpl::~SHVDataStructImpl()
{
	for (size_t i = 0; i < GetColumnCount(); i++)
	{
		(*this)[i]->ClearOwnership();
	}
}

/*************************************
 * GetTableName
 *************************************/
const SHVString8C& SHVDataStructImpl::GetTableName() const
{
	return TableName;
}

/*************************************
 * SetTableName
 *************************************/
void SHVDataStructImpl::SetTableName(const SHVString8C& name)
{
	TableName = name;
}

/*************************************
 * ::operator[]
 *************************************/
const SHVDataStructColumnC* SHVDataStructImpl::operator[](size_t idx) const
{
	return Columns[idx];
}

/*************************************
 * GetAt
 *************************************/
SHVDataStructColumn* SHVDataStructImpl::operator[](size_t idx)
{
	return Columns[idx];
}

/*************************************
 * FindColumnIndex
 *************************************/
const SHVBool SHVDataStructImpl::FindColumnIndex(size_t& index, const SHVString8C& colName) const
{
SHVBool retVal(SHVBool::False);
	for (index = Columns.CalculateCount(); index && !retVal; )
	{
		retVal = colName == Columns[--index]->GetColumnName();
	}
	return retVal;
}

/*************************************
 * CreateNew
 *************************************/
SHVDataStructColumn* SHVDataStructImpl::CreateNew()
{
	return new SHVDataStructColumnImpl();
}

/*************************************
 * InsertAt
 *************************************/
void SHVDataStructImpl::InsertAt(size_t idx, SHVDataStructColumn& col)
{
	Columns.Add(NULL);
	for (size_t i = Columns.CalculateCount(); i > idx; i--)
	{
		Columns.Replace(idx, Columns[idx - 1]);
	}
	Columns.Replace(idx, &col);
}

/*************************************
 * RemoveAt
 *************************************/
SHVDataStructColumn* SHVDataStructImpl::RemoveAt(size_t idx)
{
size_t size = Columns.CalculateCount()-1;
SHVDataStructColumn* result = Columns[idx];
	for (size_t i = idx; i < size; i++)
	{
		Columns.Replace(idx, Columns[idx+1]);
	}
	Columns.Remove(size-1);
	return result;
}

/*************************************
 * Add
 *************************************/
size_t SHVDataStructImpl::Add(SHVDataStructColumn* col)
{
	Columns.Add(col);
	return Columns.CalculateCount();
}

size_t SHVDataStructImpl::Add(const SHVString8C& colName, int dataType, int dataLength, SHVBool PrimaryKey, SHVBool AllowNull, SHVBool AutoInc)
{
SHVDataStructColumnImpl* col = new SHVDataStructColumnImpl();
	col->SetColumnName(colName);
	col->SetDataType(dataType);
	col->SetDataLength(dataLength);
	col->SetAllowNull(AllowNull);
	col->SetAutoInc(AutoInc);
	return Add(col);
}


/*************************************
 * GetColumnCount
 *************************************/
size_t SHVDataStructImpl::GetColumnCount() const
{
	return Columns.CalculateCount();
}

/*************************************
 * GetPrimaryIndex
 *************************************/
const SHVDataRowKey* SHVDataStructImpl::GetPrimaryIndex() const
{
	return GetIndex(0);
}

/*************************************
 * SetPrimaryIndex
 *************************************/
void SHVDataStructImpl::SetPrimaryIndex(const SHVDataRowKey* key)
{
	if (Indexes.CalculateCount() > 0)
		Indexes.Replace(0, (SHVDataRowKey*) key);
	else
		Indexes.Add((SHVDataRowKey*) key);
}

/*************************************
 * IsEqual
 *************************************/
SHVBool SHVDataStructImpl::IsEqual(const SHVDataStructC* dataStruct) const
{
SHVBool retVal = SHVBool::False;
const SHVDataStructC& This = *this;
const SHVDataStructC& Struct = *dataStruct;

	if (Struct.GetColumnCount() == GetColumnCount() && IndexCount() == Struct.IndexCount())
	{
		retVal = SHVBool::True;
		for (size_t i = 0; i < GetColumnCount() && retVal; i++)
		{
		size_t j;
			retVal = SHVBool::False;
			for (j = 0; j < Struct.GetColumnCount() && This[i]->GetColumnName() != This[j]->GetColumnName(); j++);
			if (j != dataStruct->GetColumnCount())
			{
				retVal = 
					This[i]->GetColumnName() == Struct[j]->GetColumnName() &&
					This[i]->GetDataType() == Struct[j]->GetDataType() &&
					This[i]->GetDataLength() == Struct[j]->GetDataLength() &&
					This[i]->GetAutoInc() == Struct[j]->GetAutoInc() &&
					This[i]->GetAllowNull() == Struct[j]->GetAllowNull();
			}
			else
				retVal = SHVBool::False;
		}
		for (size_t i = 0; i < IndexCount() && retVal; i++)
		{
			retVal = Struct.GetIndex(i)->KeyDefEquals(GetIndex(i));			
		}
	}
	return retVal;
}

/*************************************
 * GetIndex
 *************************************/
const SHVDataRowKey* SHVDataStructImpl::GetIndex(size_t IdxID) const
{
	SHVASSERT(IdxID < Indexes.CalculateCount());
	if (IdxID >= Indexes.CalculateCount())
		return NULL;
	return (const SHVDataRowKey*) Indexes[IdxID];
}

/*************************************
 * IndexCount
 *************************************/
const size_t SHVDataStructImpl::IndexCount() const
{
	return Indexes.CalculateCount();
}

/*************************************
 * AddIndex
 *************************************/
void SHVDataStructImpl::AddIndex(SHVDataRowKey* index)
{
	Indexes.Add(index);
}

/*************************************
 * CreateIndexKey
 *************************************/
SHVDataRowKey* SHVDataStructImpl::CreateIndexKey() const
{
	return new SHVDataRowKeyImpl();
}
