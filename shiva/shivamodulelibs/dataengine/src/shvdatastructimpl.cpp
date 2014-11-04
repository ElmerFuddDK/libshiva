/*
 *   Copyright (C) 2008 by Mogens Bak Nielsen, Lars Eriksen
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of the
 *   License, or (at your option) any later version with the following
 *   exeptions:
 *
 *   1) Static linking to the library does not constitute derivative work
 *      and does not require the author to provide source code for the
 *      application.
 *      Compiling applications with the source code directly linked in is
 *      Considered static linking as well.
 *
 *   2) You do not have to provide a copy of the license with programs
 *      that are linked against this code.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../include/shvdatastructimpl.h"
#include "../include/shvdatarowkeyimpl.h"

// =============================== implementation SHVDataStructColumnImpl ====================================
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

// ================================== implementation SHVDataStructImpl =======================================
/*************************************
 * Constructor
 *************************************/
SHVDataStructImpl::SHVDataStructImpl()
{
	IsMultiInstance = false;
}
	
SHVDataStructImpl::SHVDataStructImpl(const SHVDataStructC* dataStruct)
{
	for (size_t i = 0; i < dataStruct->GetColumnCount(); i++)
	{
		Add(new SHVDataStructColumnImpl((*dataStruct)[i]));
	}
	SetTableName(dataStruct->GetTableName());
	for (size_t i = 0; i < dataStruct->IndexCount(); i++)
		AddIndex((SHVDataRowKey*) dataStruct->GetIndex(i));
}

SHVDataStructImpl::~SHVDataStructImpl()
{
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
 * GetIsMultiInstance
 *************************************/
bool SHVDataStructImpl::GetIsMultiInstance() const
{
	return IsMultiInstance;
}

/*************************************
 * SetIsMultiInstance
 *************************************/
void SHVDataStructImpl::SetIsMultiInstance(bool isMultiInstance)
{
	IsMultiInstance = isMultiInstance;
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
SHVBool SHVDataStructImpl::FindColumnIndex(size_t& index, const SHVString8C& colName) const
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
void SHVDataStructImpl::RemoveAt(size_t idx)
{
size_t size = Columns.CalculateCount()-1;
	for (size_t i = idx; i < size; i++)
	{
		Columns.Replace(i, Columns[i+1]);
	}
	Columns.Pop();
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
	col->SetDataLength(dataType == SHVDataVariant::TypeString ? dataLength : -1);
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
SHVBool SHVDataStructImpl::IsEqual(const SHVDataStructC* dataStruct, bool strict) const
{
SHVBool retVal = SHVBool::False;
const SHVDataStructC& This = *this;
const SHVDataStructC& Struct = *dataStruct;

	if (Struct.GetColumnCount() == GetColumnCount() && Struct.GetIsMultiInstance() == GetIsMultiInstance())
	{
		retVal = SHVBool::True;
		for (size_t i = 0; i < GetColumnCount() && retVal; i++)
		{
		size_t j;
			if (strict)
			{
				j=i;
			}
			else
			{
				for (j = 0; j < Struct.GetColumnCount() && This[i]->GetColumnName() != Struct[j]->GetColumnName(); j++);
				if (j == dataStruct->GetColumnCount())
					retVal = SHVBool::False;
			}
			retVal = retVal &&
				This[i]->GetColumnName() == Struct[j]->GetColumnName() &&
				This[i]->GetDataType() == Struct[j]->GetDataType() &&
				This[i]->GetDataLength() == Struct[j]->GetDataLength();
		}
		if (retVal && Struct.IndexCount() > 0 && IndexCount() > 0)
			retVal = Struct.GetPrimaryIndex()->KeyDefEquals(GetPrimaryIndex());
		else
			retVal = false;
	}
	return retVal;
}
/*************************************
 * IsIndexesEqual
 *************************************/
SHVBool SHVDataStructImpl::IsIndexesEqual(const SHVDataStructC* dataStruct) const
{
SHVBool retVal = SHVBool::True;
const SHVDataStructC& Struct = *dataStruct;
	retVal = Struct.IndexCount() != IndexCount();
	for (size_t i = 0; i < Struct.IndexCount() && retVal; i++)
	{
		retVal = Struct.GetIndex(i)->KeyDefEquals(GetIndex(i));			
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
 * FindIndex
 *************************************/
size_t SHVDataStructImpl::FindIndex(const SHVDataRowKey* key) const
{
bool Exists = false;
size_t retVal;
	for (retVal = 0; retVal < IndexCount() && !Exists; retVal++)
	{
		Exists = GetIndex(retVal)->KeyDefEquals(key);
		if (Exists)
			break;
	}
	return Exists ? retVal : SIZE_T_MAX;
}

/*************************************
 * IndexCount
 *************************************/
size_t SHVDataStructImpl::IndexCount() const
{
	return Indexes.CalculateCount();
}

/*************************************
 * AddIndex
 *************************************/
size_t SHVDataStructImpl::AddIndex(SHVDataRowKey* index)
{
	return Indexes.Add(index);
}
/*************************************
 * RemoveLastIndex
 *************************************/
size_t SHVDataStructImpl::RemoveLastIndex()
{
	if (Indexes.CalculateCount())
		Indexes.Pop();
	return IndexCount();
}

/*************************************
 * CreateIndexKey
 *************************************/
SHVDataRowKey* SHVDataStructImpl::CreateIndexKey() const
{
	return new SHVDataRowKeyImpl();
}

/*************************************
 * GetInternalStruct
 *************************************/
SHVDataStruct* SHVDataStructImpl::GetInternalStruct()
{
	return this;
}
