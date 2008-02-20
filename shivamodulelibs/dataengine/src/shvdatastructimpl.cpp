#include "stdafx.h"
#include "../../../../include/platformspc.h"
#include "../../include/dataengineimpl/shvdatastruct_impl.h"
#include "../../include/dataengineimpl/shvdatarowkey_impl.h"

/*************************************
 * Constructor
 *************************************/
SHVDataStructColumn_impl::SHVDataStructColumn_impl()
{
}

SHVDataStructColumn_impl::SHVDataStructColumn_impl(const SHVDataStructColumnC* col)
{
	SetColumnName(col->GetColumnName());
	SetDataType(col->GetDataType());
	SetDataLength(col->GetDataLength());
	SetPrimaryKey(col->GetPrimaryKey());
	SetAllowNull(col->GetAllowNull());
	SetAutoInc(col->GetAutoInc());
}

SHVDataStructColumn_impl::~SHVDataStructColumn_impl()
{
}

void SHVDataStructColumn_impl::ClearOwnership()
{
}

/*************************************
 * GetColumnName
 *************************************/
const SHVString8C& SHVDataStructColumn_impl::GetColumnName() const
{
	return ColumnName;
}

/*************************************
 * SetColumnName
 *************************************/
void SHVDataStructColumn_impl::SetColumnName(const SHVString8C& name)
{
	ColumnName = name;
}

/*************************************
 * GetDataType
 *************************************/
int SHVDataStructColumn_impl::GetDataType() const
{
	return DataType;
}

/*************************************
 * SetDataType
 *************************************/
void SHVDataStructColumn_impl::SetDataType(int dataType)
{
	DataType = dataType;
}

/*************************************
 * GetDataLength
 *************************************/
int SHVDataStructColumn_impl::GetDataLength() const
{
	return DataLength;
}

/*************************************
 * SetDataLength
 *************************************/
void SHVDataStructColumn_impl::SetDataLength(int len)
{
	DataLength = len;
}

/*************************************
 * GetPrimaryKey
 *************************************/
SHVBool SHVDataStructColumn_impl::GetPrimaryKey() const
{
	return PrimaryKey;
}

/*************************************
 * SetPrimaryKey
 *************************************/
void SHVDataStructColumn_impl::SetPrimaryKey(SHVBool flag)
{
	PrimaryKey = flag;
}

/*************************************
 * GetAllowNull
 *************************************/
SHVBool SHVDataStructColumn_impl::GetAllowNull() const
{
	return AllowNull;
}

/*************************************
 * SetAllowNull
 *************************************/
void SHVDataStructColumn_impl::SetAllowNull(SHVBool flag)
{
	AllowNull = flag;
}

/*************************************
 * GetAutoInc
 *************************************/
SHVBool SHVDataStructColumn_impl::GetAutoInc() const
{
	return AutoInc;
}

/*************************************
 * SetAutoInc
 *************************************/
void SHVDataStructColumn_impl::SetAutoInc(SHVBool flag)
{
	AutoInc = flag;
}

/*************************************
 * Constructor
 *************************************/
SHVDataStruct_impl::SHVDataStruct_impl()
{
}
	
SHVDataStruct_impl::SHVDataStruct_impl(const SHVDataStructC* dataStruct)
{
	for (size_t i = 0; i < dataStruct->GetColumnCount(); i++)
	{
		Add(new SHVDataStructColumn_impl((const SHVDataStructColumnC*)(*dataStruct)[i]));
	}
	SetTableName(dataStruct->GetTableName());
	for (size_t i = 0; i < dataStruct->IndexCount(); i++)
		AddIndex((SHVDataRowKey*) dataStruct->GetIndex(i));
}

SHVDataStruct_impl::~SHVDataStruct_impl()
{
	for (size_t i = 0; i < GetColumnCount(); i++)
	{
		(*this)[i]->ClearOwnership();
	}
}

/*************************************
 * GetTableName
 *************************************/
const SHVString8C& SHVDataStruct_impl::GetTableName() const
{
	return TableName;
}

/*************************************
 * SetTableName
 *************************************/
void SHVDataStruct_impl::SetTableName(const SHVString8C& name)
{
	TableName = name;
}

/*************************************
 * ::operator[]
 *************************************/
const SHVDataStructColumnC* SHVDataStruct_impl::operator[](size_t idx) const
{
	return Columns[idx];
}

/*************************************
 * GetAt
 *************************************/
SHVDataStructColumn* SHVDataStruct_impl::operator[](size_t idx)
{
	return Columns[idx];
}

/*************************************
 * FindColumnIndex
 *************************************/
const SHVBool SHVDataStruct_impl::FindColumnIndex(size_t& index, const SHVString8C& colName) const
{
SHVBool retVal(SHVBool::False);
	for (index = Columns.CalculateCount(); index && retVal; index--)
	{
		retVal = colName == Columns[index]->GetColumnName();
	}
	return retVal;
}

/*************************************
 * CreateNew
 *************************************/
SHVDataStructColumn* SHVDataStruct_impl::CreateNew()
{
	return new SHVDataStructColumn_impl();
}

/*************************************
 * InsertAt
 *************************************/
void SHVDataStruct_impl::InsertAt(size_t idx, SHVDataStructColumn& col)
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
SHVDataStructColumn* SHVDataStruct_impl::RemoveAt(size_t idx)
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
size_t SHVDataStruct_impl::Add(SHVDataStructColumn* col)
{
	Columns.Add(col);
	return Columns.CalculateCount();
}

size_t SHVDataStruct_impl::Add(const SHVString8C& colName, int dataType, int dataLength, SHVBool PrimaryKey, SHVBool AllowNull, SHVBool AutoInc)
{
SHVDataStructColumn_impl* col = new SHVDataStructColumn_impl();
	col->SetColumnName(colName);
	col->SetDataType(dataType);
	col->SetPrimaryKey(PrimaryKey);
	col->SetAllowNull(AllowNull);
	col->SetAutoInc(AutoInc);
	return Add(col);
}


/*************************************
 * GetColumnCount
 *************************************/
size_t SHVDataStruct_impl::GetColumnCount() const
{
	return Columns.CalculateCount();
}

/*************************************
 * GetPrimaryIndex
 *************************************/
const SHVDataRowKey* SHVDataStruct_impl::GetPrimaryIndex() const
{
	return GetIndex(0);
}

/*************************************
 * SetPrimaryIndex
 *************************************/
void SHVDataStruct_impl::SetPrimaryIndex(const SHVDataRowKey* key)
{
	if (Indexes.CalculateCount() > 0)
		Indexes.Replace(0, (SHVDataRowKey*) key);
	else
		Indexes.Add((SHVDataRowKey*) key);
}

/*************************************
 * IsEqual
 *************************************/
SHVBool SHVDataStruct_impl::IsEqual(const SHVDataStructC* dataStruct) const
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

const SHVDataRowKey* SHVDataStruct_impl::GetIndex(size_t IdxID) const
{
	SHVASSERT(IdxID < Indexes.CalculateCount());
	if (IdxID >= Indexes.CalculateCount())
		return NULL;
	return (const SHVDataRowKey*) Indexes[IdxID];
}

const size_t SHVDataStruct_impl::IndexCount() const
{
	return Indexes.CalculateCount();
}

void SHVDataStruct_impl::AddIndex(SHVDataRowKey* index)
{
	Indexes.Add(index);
}
