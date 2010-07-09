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
#include "../include/shvdatarowcsqlite.h"
#include "../include/shvdatarowimpl.h"
#include "../include/shvdatarowkeyimpl.h"
#include "../include/shvdatavariantimpl.h"


/*************************************
 * AsString
 *************************************/
SHVStringBuffer SHVDataRowCSQLite::AsString(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetRowProvider();
SHVString retVal;
int len;
	if (!IsNull(colIdx))
		Statement->GetString(retVal, len, (int) colIdx);
	return retVal.ReleaseBuffer();
}

/*************************************
 * AsString
 *************************************/
SHVStringBuffer SHVDataRowCSQLite::AsDBString(size_t colIdx) const
{
SHVDataVariantPtr var = GetValue(colIdx);
	return (var.IsNull() ? SHVString(_S("null")).ReleaseBuffer() : var->AsDBString());
}

/*************************************
 * AsInt
 *************************************/
SHVInt SHVDataRowCSQLite::AsInt(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetRowProvider();
SHVInt retVal;
int val;
	if (!IsNull(colIdx))
	{
		Statement->GetInt(val, (int) colIdx);
		retVal = val;
	}		
	return retVal;
}

/*************************************
 * AsInt64
 *************************************/
SHVInt64 SHVDataRowCSQLite::AsInt64(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetRowProvider();
SHVInt64 retVal;
SHVInt64Val val;
	if (!IsNull(colIdx))
	{
		Statement->GetInt64(val, (int) colIdx);
		retVal = SHVInt64(val);
	}		
	return retVal;
}

/*************************************
 * AsDouble
 *************************************/
SHVDouble SHVDataRowCSQLite::AsDouble(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetRowProvider();
SHVDouble retVal;
double val;
	if (!IsNull(colIdx))
	{
		Statement->GetDouble(val, (int) colIdx);
		retVal = SHVDouble(val);
	}		
	return retVal;
}

/*************************************
 * AsTime
 *************************************/
SHVTime SHVDataRowCSQLite::AsTime(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetRowProvider();
int len;
SHVTime retVal;
SHVString val;
	if (!IsNull(colIdx))
	{
		Statement->GetString(val, len, (int) colIdx);
		retVal.SetFromDateString(val);
	}		
	return retVal;
}

/*************************************
 * AsBool
 *************************************/
SHVBool SHVDataRowCSQLite::AsBool(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetRowProvider();
SHVBool retVal;
int val;
	if (!IsNull(colIdx))
	{
		Statement->GetInt(val, (int) colIdx);
		retVal = val != 0;
	}		
	return retVal;
}

/*************************************
 * IsNull
 *************************************/
SHVBool SHVDataRowCSQLite::IsNull(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetRowProvider();
SHVBool retVal(SHVBool::True);
short type;
	SHVASSERT(colIdx != SIZE_T_MAX);
	if (colIdx != SIZE_T_MAX && Statement->GetColumnAffinity(type, (int) colIdx))
		retVal = type == SHVSQLiteWrapper::Affinity_Null;
	return retVal;
}

/*************************************
 * ColumnIndex
 *************************************/
size_t SHVDataRowCSQLite::ColumnIndex(const SHVString8C& columnName) const
{
size_t retVal;
	SHVASSERT(RowValid());
	if (RowValid())
	{
		if (!Select->GetStruct()->FindColumnIndex(retVal, columnName))
			retVal = SIZE_T_MAX;
	}
	return retVal;
}


/*************************************
 * GetKey
 *************************************/
SHVDataRowKey* SHVDataRowCSQLite::GetKey(size_t index) const
{
	SHVASSERT(RowValid());

SHVDataRowKeyRef ResultDef = (SHVDataRowKey*) Select->GetStruct()->GetIndex(index);
SHVDataRowKeyImpl* Result = new SHVDataRowKeyImpl();
	if (RowValid())
	{
		for (size_t i = 0; i < ResultDef->Count(); i++)
		{
			Result->AddKey((*ResultDef)[i].Key, GetValue(ColumnIndex((*ResultDef)[i].Key)), (*ResultDef)[i].Desc);
		}
	}
	return Result;
}

/*************************************
 * GetValue
 *************************************/
SHVDataVariant* SHVDataRowCSQLite::GetValue(size_t colIdx) const
{
	SHVASSERT(RowValid());
SHVDataVariant* retVal = NULL;
	switch ((*Select->GetStruct())[colIdx]->GetDataType())
	{
		case SHVDataVariant::TypeInt:
			retVal = new SHVDataVariantImpl(AsInt(colIdx));
			break;
		case SHVDataVariant::TypeInt64:
			retVal = new SHVDataVariantImpl(AsInt64(colIdx));
			break;
		case SHVDataVariant::TypeBool:
			retVal = new SHVDataVariantImpl(AsBool(colIdx));
			break;
		case SHVDataVariant::TypeDouble:
			retVal = new SHVDataVariantImpl(AsDouble(colIdx));
			break;
		case SHVDataVariant::TypeString:
			retVal = new SHVDataVariantImpl(AsString(colIdx));
			break;
		case SHVDataVariant::TypeTime:
			retVal = new SHVDataVariantImpl(AsTime(colIdx));
			break;
	}
	return retVal;
}

/*************************************
 * MatchKey
 *************************************/
SHVBool SHVDataRowCSQLite::MatchKey(const SHVDataRowKey* key) const
{
SHVBool match(SHVBool::True);
	if (RowValid())
	{
	const SHVDataRowKey& Keys = *key;
	const SHVDataStructC& Struct = *Select->GetStruct();
		for (size_t i = 0; i < Keys.Count() && match; i++)
		{
		size_t colIdx;
			if (Struct.FindColumnIndex(colIdx, Keys[i].Key))
			{
				switch (Struct[colIdx]->GetDataType())
				{
				case SHVDataVariant::TypeInt:
					if (Keys[i].Value)
						match = Keys[i].Value->AsInt() == AsInt(colIdx);
					else
						match = AsInt(colIdx).IsNull();
					break;
				case SHVDataVariant::TypeInt64:
					if (Keys[i].Value)
						match = Keys[i].Value->AsInt64() == AsInt64(colIdx);
					else
						match = AsInt64(colIdx).IsNull();
					break;
				case SHVDataVariant::TypeDouble:
					if (Keys[i].Value)
						match = Keys[i].Value->AsDouble() == AsDouble(colIdx);
					else
						match = AsDouble(colIdx).IsNull();
					break;
				case SHVDataVariant::TypeString:
					if (Keys[i].Value)
						match = Keys[i].Value->AsString() == AsString(colIdx);
					else
						match = AsString(colIdx).IsNull();
					break;
				case SHVDataVariant::TypeTime:
					if (Keys[i].Value)
						match = Keys[i].Value->AsTime() == AsTime(colIdx);
					else
						match = AsTime(colIdx).IsNull();
					break;
				case SHVDataVariant::TypeBool:
					if (Keys[i].Value)
						match = Keys[i].Value->AsBool() == AsBool(colIdx);
					else
						match = AsBool(colIdx);
					break;
				}
			}
		}
	}
	return match;
}

/*************************************
 * RowValid
 *************************************/
SHVBool SHVDataRowCSQLite::RowValid() const
{
	return Select != NULL && Select->GetRowProvider() != NULL;
}

/*************************************
 * GetStruct
 *************************************/
const SHVDataStructC* SHVDataRowCSQLite::GetStruct() const
{
	if (RowValid())
		return Select->GetStruct();
	else
		return NULL;
}

/*************************************
 * GetAlias
 *************************************/
const SHVString8C SHVDataRowCSQLite::GetAlias() const
{
	if (RowValid())
		return Select->GetAlias();
	else
		return "";	
}

/*************************************
 * GetRowState
 *************************************/
int SHVDataRowCSQLite::GetRowState() const
{
	return (RowValid() ? SHVDataRow::RowStateUnchanged : SHVDataRow::RowStateInvalid);
}


/*************************************
 * GetRowListC
 *************************************/
const SHVDataRowListC* SHVDataRowCSQLite::GetRowListC()
{
	return Select;
}

/*************************************
 * Constructor
 *************************************/
SHVDataRowCSQLite::SHVDataRowCSQLite(SHVDataRowListC* select)
{
	Select = select;
}

SHVDataRowCSQLite::~SHVDataRowCSQLite()
{
}
