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
#include <stdlib.h>
#include "../../../include/platformspc.h"
#include "../include/shvdatavariantimpl.h"


/*************************************
 * Destructor
 *************************************/
SHVDataVariantImpl::~SHVDataVariantImpl()
{
	if (DataType == SHVDataVariant::TypeTime && Data.timeVal)
		delete Data.timeVal;
	if (DataType == SHVDataVariant::TypeString && Data.stringVal)
		delete Data.stringVal;
}

/*************************************
 * GetDataType
 *************************************/
int SHVDataVariantImpl::GetDataType() const
{
	return DataType;
}

/*************************************
 * AsString
 *************************************/
SHVStringBuffer SHVDataVariantImpl::AsString() const
{
SHVString retVal;
	if (!isNull)
	{
		switch (DataType)
		{
		case SHVDataVariant::TypeInt:
			retVal = SHVStringC::LongToString(Data.intVal);
			break;
		case SHVDataVariant::TypeInt64:
			retVal = SHVStringC::Int64ToString(Data.int64Val);
			break;
		case SHVDataVariant::TypeBool:
			retVal = Data.boolVal ? SHVString(_T("1")) : SHVString(_T("0"));
			break;
		case SHVDataVariant::TypeDouble:
			retVal = SHVString::DoubleToString(Data.doubleVal);
			break;
		case SHVDataVariant::TypeString:
			retVal = Data.stringVal->GetBuffer();
			break;
		case SHVDataVariant::TypeTime:
			retVal = Data.timeVal->ToDateString();
			break;
		}
	}
	return retVal.ReleaseBuffer();
}

/*************************************
 * AsDBString
 *************************************/
SHVStringBuffer SHVDataVariantImpl::AsDBString() const
{
SHVString retVal;

	if (!isNull)
	{
		switch (DataType)
		{
		case SHVDataVariant::TypeString:
		case SHVDataVariant::TypeTime:
			{
			SHVString value(AsString());
				value.Replace(_T("'"), _T("''"));
				retVal.Format(_T("'%s'"), value.GetSafeBuffer());
			}
			break;
		default:
			retVal = AsString();
			break;
		}
	}
	else
	{
		switch (DataType)
		{
		case SHVDataVariant::TypeBool:
			retVal = _T("0");
			break;
		default:
			retVal = _T("null");
			break;
		}
	}

	return retVal.ReleaseBuffer();
}

/*************************************
 * SetString
 *************************************/
void SHVDataVariantImpl::SetString(const SHVStringC& val)
{
	if (DataType == SHVDataVariant::TypeUndefined)
		DataType = SHVDataVariant::TypeString;
	if (val.IsNull())
	{
		if (DataType == SHVDataVariant::TypeTime && Data.timeVal)
			delete Data.timeVal;
		if (DataType == SHVDataVariant::TypeString && Data.stringVal)
			delete Data.stringVal;
		Data.stringVal = NULL;
		isNull = true;
	}
	else
	{
		isNull = false;
		if (DataType == SHVDataVariant::TypeTime && Data.timeVal)
			delete Data.timeVal;
		if (DataType == SHVDataVariant::TypeString && Data.stringVal)
			delete Data.stringVal;
		switch (DataType)
		{
		case SHVDataVariant::TypeInt:
			Data.intVal = SHVStringC::StrToL(val.GetSafeBuffer(), NULL, 10);
			break;
		case SHVDataVariant::TypeInt64:
			Data.int64Val = SHVStringC::StrToInt64(val.GetSafeBuffer(), NULL, 10);
			break;
		case SHVDataVariant::TypeBool:
			Data.boolVal = val == _T("1");
			break;
		case SHVDataVariant::TypeDouble:
			Data.doubleVal = SHVStringC::StrToDouble(val.GetSafeBuffer(), NULL);
			break;
		case SHVDataVariant::TypeString:
			Data.stringVal = new SHVString(val);
			break;
		case SHVDataVariant::TypeTime:
			Data.timeVal = new SHVTime();
			Data.timeVal->SetFromDateString(val);
			break;
		}
	}
}

/*************************************
 * AsInt
 *************************************/
SHVInt SHVDataVariantImpl::AsInt() const
{
SHVInt retVal;
	SHVASSERT(DataType != SHVDataVariant::TypeTime);
	if (DataType != SHVDataVariant::TypeTime)
	{
		if (!isNull)
		{
		switch (DataType)
		{
			case SHVDataVariant::TypeInt:
				retVal = Data.intVal;
				break;
			case SHVDataVariant::TypeInt64:
				retVal = (int) Data.int64Val;
				break;
			case SHVDataVariant::TypeBool:
				retVal = SHVBool(Data.boolVal).GetError();
				break;
			case SHVDataVariant::TypeDouble:
				retVal = (int) Data.doubleVal;
				break;
			case SHVDataVariant::TypeString:
				retVal = SHVStringC::StrToL(Data.stringVal->GetSafeBuffer(), NULL, 10);
				break;
			case SHVDataVariant::TypeTime:
				break;
			}
		}
	}
	return retVal;
}

/*************************************
 * SetInt
 *************************************/
void SHVDataVariantImpl::SetInt(SHVInt val)
{
	SHVASSERT(DataType != SHVDataVariant::TypeTime);
	if (DataType == SHVDataVariant::TypeUndefined)
		DataType = SHVDataVariant::TypeInt;
	if (val.IsNull())
	{
		if (DataType == SHVDataVariant::TypeString && Data.stringVal)
			delete Data.stringVal;
		Data.stringVal = NULL;
		isNull = true;
	}
	else
	if (DataType != SHVDataVariant::TypeTime)
	{
		isNull = false;
		if (DataType == SHVDataVariant::TypeString && Data.stringVal)
			delete Data.stringVal;
		switch (DataType)
		{
		case SHVDataVariant::TypeInt:
			Data.intVal = val;
			break;
		case SHVDataVariant::TypeInt64:
			Data.int64Val = val;
			break;
		case SHVDataVariant::TypeBool:
			Data.boolVal = val != 0;
			break;
		case SHVDataVariant::TypeDouble:
			Data.doubleVal = (double) val;
			break;
		case SHVDataVariant::TypeString:
			Data.stringVal = new SHVString(SHVStringC::LongToString(val));
			break;
		}
	}
}

/*************************************
 * AsInt64
 *************************************/
SHVInt64 SHVDataVariantImpl::AsInt64() const
{
SHVInt64 retVal;
	SHVASSERT(DataType != SHVDataVariant::TypeTime);
	if (DataType != SHVDataVariant::TypeTime)
	{
		if (!isNull)
		{
		switch (DataType)
		{
			case SHVDataVariant::TypeInt:
				retVal = Data.intVal;
				break;
			case SHVDataVariant::TypeInt64:
				retVal = Data.int64Val;
				break;
			case SHVDataVariant::TypeBool:
				retVal = SHVBool(Data.boolVal).GetError();
				break;
			case SHVDataVariant::TypeDouble:
				retVal = (int) Data.doubleVal;
				break;
			case SHVDataVariant::TypeString:
				retVal = SHVStringC::StrToL(Data.stringVal->GetSafeBuffer(), NULL, 10);
				break;
			case SHVDataVariant::TypeTime:
				break;
			}
		}
	}
	return retVal;
}

/*************************************
 * SetInt
 *************************************/
void SHVDataVariantImpl::SetInt64(SHVInt64 val)
{
	SHVASSERT(DataType != SHVDataVariant::TypeTime);
	if (DataType == SHVDataVariant::TypeUndefined)
		DataType = SHVDataVariant::TypeInt;
	if (val.IsNull())
	{
		if (DataType == SHVDataVariant::TypeString && Data.stringVal)
			delete Data.stringVal;
		Data.stringVal = NULL;
		isNull = true;
	}
	else
	if (DataType != SHVDataVariant::TypeTime)
	{
		isNull = false;
		if (DataType == SHVDataVariant::TypeString && Data.stringVal)
			delete Data.stringVal;
		switch (DataType)
		{
		case SHVDataVariant::TypeInt:
			Data.intVal = (int) val;
			break;
		case SHVDataVariant::TypeInt64:
			Data.int64Val = val;
			break;
		case SHVDataVariant::TypeBool:
			Data.boolVal = val != 0;
			break;
		case SHVDataVariant::TypeDouble:
			Data.doubleVal = (double) val;
			break;
		case SHVDataVariant::TypeString:
			Data.stringVal = new SHVString(SHVStringC::Int64ToString(val));
			break;
		}
	}
}

/*************************************
 * AsDouble
 *************************************/
SHVDouble SHVDataVariantImpl::AsDouble() const
{
SHVDouble retVal;
	SHVASSERT(DataType != SHVDataVariant::TypeTime);
	if (DataType != SHVDataVariant::TypeTime)
	{
		if (!isNull)
		{
		switch (DataType)
		{
			case SHVDataVariant::TypeInt:
				retVal = (double) Data.intVal;
				break;
			case SHVDataVariant::TypeInt64:
				retVal = (double) Data.int64Val;
				break;
			case SHVDataVariant::TypeBool:
				retVal = (double) SHVBool(Data.boolVal).GetError();
				break;
			case SHVDataVariant::TypeDouble:
				retVal = Data.doubleVal;
				break;
			case SHVDataVariant::TypeString:
				retVal = SHVStringC::StrToDouble(Data.stringVal->GetSafeBuffer(), NULL);
				break;
			case SHVDataVariant::TypeTime:
				break;
			}
		}
	}
	return retVal;
}

/*************************************
 * SetDouble
 *************************************/
void SHVDataVariantImpl::SetDouble(SHVDouble val)
{
	SHVASSERT(DataType != SHVDataVariant::TypeTime);
	if (DataType == SHVDataVariant::TypeUndefined)
		DataType = SHVDataVariant::TypeDouble;
	if (val.IsNull())
	{
		if (DataType == SHVDataVariant::TypeTime && Data.timeVal)
			delete Data.timeVal;
		if (DataType == SHVDataVariant::TypeString && Data.stringVal)
			delete Data.stringVal;
		Data.stringVal = NULL;
		isNull = true;
	}
	else
	if (DataType != SHVDataVariant::TypeTime)
	{
		isNull = false;
		if (DataType == SHVDataVariant::TypeString && Data.stringVal)
			delete Data.stringVal;
		switch (DataType)
		{
		case SHVDataVariant::TypeInt:
			Data.intVal = (int) val;
			break;
		case SHVDataVariant::TypeInt64:
			Data.int64Val = (SHVInt64Val) val;
			break;
		case SHVDataVariant::TypeBool:
			Data.boolVal = val != 0;
			break;
		case SHVDataVariant::TypeDouble:
			Data.doubleVal = val;
			break;
		case SHVDataVariant::TypeString:
			Data.stringVal = new SHVString();
			Data.stringVal->Format(_T("%g"), (double)val);
			break;
		}
	}
}

/*************************************
 * AsTime
 *************************************/
SHVTime SHVDataVariantImpl::AsTime() const
{
SHVTime retVal;
	SHVASSERT(DataType == SHVDataVariant::TypeTime);
	if (DataType == SHVDataVariant::TypeTime)
	{
		retVal = *Data.timeVal;
	}
	return retVal;
}

/*************************************
 * SetTime
 *************************************/
void SHVDataVariantImpl::SetTime(const SHVTime& val)
{
	if (DataType == SHVDataVariant::TypeUndefined)
		DataType = SHVDataVariant::TypeTime;
	SHVASSERT(DataType == SHVDataVariant::TypeTime || DataType == SHVDataVariant::TypeString);
	if (val.IsNull())
	{
		if (DataType == SHVDataVariant::TypeTime && Data.timeVal)
			delete Data.timeVal;
		if (DataType == SHVDataVariant::TypeString && Data.stringVal)
			delete Data.stringVal;
		Data.stringVal = NULL;
		isNull = true;
	}
	else
	if (DataType == SHVDataVariant::TypeTime || DataType == SHVDataVariant::TypeString)
	{
		isNull = false;
		if (DataType == SHVDataVariant::TypeTime && Data.timeVal)
			delete Data.timeVal;
		if (DataType == SHVDataVariant::TypeString && Data.stringVal)
			delete Data.stringVal;
		switch (DataType)
		{
		case SHVDataVariant::TypeString:
			Data.stringVal = new SHVString(val.ToDateString());
			break;
		case SHVDataVariant::TypeTime:
			Data.timeVal = new SHVTime(val);
			break;
		}
	}
}

/*************************************
 * AsBool
 *************************************/
SHVBool SHVDataVariantImpl::AsBool() const
{
SHVBool retVal;
	SHVASSERT(DataType != SHVDataVariant::TypeTime);
	if (DataType != SHVDataVariant::TypeTime)
	{
		if (!isNull)
		{
		switch (DataType)
		{
			case SHVDataVariant::TypeInt:
				retVal = SHVBool(Data.intVal != 0);
				break;
			case SHVDataVariant::TypeInt64:
				retVal = SHVBool(Data.int64Val != 0);
				break;
			case SHVDataVariant::TypeBool:
				retVal = SHVBool(Data.boolVal);
				break;
			case SHVDataVariant::TypeDouble:
				retVal = SHVBool(Data.intVal != 0);
				break;
			case SHVDataVariant::TypeString:
				return *Data.stringVal == _T("1");
				break;
			case SHVDataVariant::TypeTime:
				break;
			}
		}
	}
	return retVal;
}

/*************************************
 * SetBool
 *************************************/
void SHVDataVariantImpl::SetBool(SHVBool val)
{
	if (DataType == SHVDataVariant::TypeUndefined)
		DataType = SHVDataVariant::TypeBool;
	SHVASSERT(DataType != SHVDataVariant::TypeTime);
	if (DataType != SHVDataVariant::TypeTime)
	{
		isNull = false;
		if (DataType == SHVDataVariant::TypeString && Data.stringVal)
			delete Data.stringVal;
		switch (DataType)
		{
		case SHVDataVariant::TypeInt:
			Data.intVal = val.GetError();
			break;
		case SHVDataVariant::TypeInt64:
			Data.int64Val = val.GetError();
			break;
		case SHVDataVariant::TypeBool:
			Data.boolVal = val;
			break;
		case SHVDataVariant::TypeDouble:
			Data.doubleVal = (double) val.GetError();
			break;
		case SHVDataVariant::TypeString:
			Data.stringVal = new SHVString(val ? _T("1") : _T("0"));
			break;
		}
	}
}

/*************************************
 * IsNull
 *************************************/
SHVBool SHVDataVariantImpl::IsNull() const
{
	return isNull;
}

/*************************************
 * SetNull
 *************************************/
void SHVDataVariantImpl::SetNull()
{
	if (DataType == SHVDataVariant::TypeTime && Data.timeVal)
		delete Data.timeVal;
	if (DataType == SHVDataVariant::TypeString && Data.stringVal)
		delete Data.stringVal;
	isNull = true;
}

/*************************************
 * operator =
 *************************************/
SHVDataVariant& SHVDataVariantImpl::operator=(const SHVDataVariant& val)
{
	switch (val.GetDataType())
	{
		case SHVDataVariant::TypeInt:
			SetInt(val.AsInt());
			break;
		case SHVDataVariant::TypeInt64:
			SetInt64(val.AsInt64());
			break;
		case SHVDataVariant::TypeBool:
			SetBool(val.AsBool());
			break;
		case SHVDataVariant::TypeDouble:
			SetDouble(val.AsDouble());
			break;
		case SHVDataVariant::TypeString:
			SetString(val.AsString());
			break;
		case SHVDataVariant::TypeTime:
			SetTime(val.AsTime());
			break;
	}
	return *this;
}

/*************************************
 * operator ==
 *************************************/
bool SHVDataVariantImpl::operator==(const SHVDataVariant& val) const
{
	if (DataType == val.GetDataType())
	{
		switch (DataType)
		{
		case SHVDataVariant::TypeInt:
			return AsInt() == val.AsInt();
		case SHVDataVariant::TypeInt64:
			return AsInt64() == val.AsInt64();
		case SHVDataVariant::TypeBool:
			return AsBool() == val.AsBool();
		case SHVDataVariant::TypeDouble:
			return AsDouble() == val.AsDouble();
		case SHVDataVariant::TypeString:
			return AsString() == val.AsString();
		case SHVDataVariant::TypeTime:
			return (AsTime() == val.AsTime() ? true : false);
			break;
		}
	}
	return false;
}
