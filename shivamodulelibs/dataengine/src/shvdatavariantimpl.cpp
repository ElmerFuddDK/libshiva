#include "stdafx.h"
#include <stdlib.h>
#include "../../../../include/platformspc.h"
#include "../../include/shvdatavariantimpl.h"


SHVDataVariant_impl::~SHVDataVariant_impl()
{
	if (DataType == SHVDataVariant::TypeTime && Data.timeVal)
		delete Data.timeVal;
	if (DataType == SHVDataVariant::TypeString && Data.stringVal)
		delete Data.stringVal;
}

int SHVDataVariant_impl::GetDataType() const
{
	return DataType;
}

SHVStringBuffer SHVDataVariant_impl::AsString() const
{
SHVString retVal;
	if (!isNull)
	{
	switch (DataType)
	{
		case SHVDataVariant::TypeInt:
			retVal = SHVStringC::LongToString(Data.intVal);
			break;
		case SHVDataVariant::TypeBool:
			retVal = Data.boolVal ? SHVString(_T("1")) : SHVString(_T("0"));
			break;
		case SHVDataVariant::TypeDouble:
			retVal.Format(_T("%g"), Data.doubleVal);
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

void SHVDataVariant_impl::SetString(const SHVStringC& val)
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
		case SHVDataVariant::TypeBool:
			Data.boolVal = val == _T("1");
			break;
		case SHVDataVariant::TypeDouble:
#ifdef UNICODE
			Data.doubleVal = wcstod(val.GetSafeBuffer(), NULL);
#else
			Data.doubleVal = strtod(val.GetSafeBuffer(), NULL);
#endif
			break;
		case SHVDataVariant::TypeString:
			Data.stringVal = new SHVString(val);
			break;
		case SHVDataVariant::TypeTime:
			Data.timeVal->SetFromDateString(val);
			break;
		}
	}
}

SHVInt SHVDataVariant_impl::AsInt() const
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

void SHVDataVariant_impl::SetInt(SHVInt val)
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

SHVDouble SHVDataVariant_impl::AsDouble() const
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
			case SHVDataVariant::TypeBool:
				retVal = (double) SHVBool(Data.boolVal).GetError();
				break;
			case SHVDataVariant::TypeDouble:
				retVal = Data.doubleVal;
				break;
			case SHVDataVariant::TypeString:
#ifdef UNICODE
				retVal = wcstod(Data.stringVal->GetSafeBuffer(), NULL);
#else
				retVal = strtod(Data.stringVal->GetSafeBuffer(), NULL);
#endif
				break;
			case SHVDataVariant::TypeTime:
				break;
			}
		}
	}
	return retVal;
}

void SHVDataVariant_impl::SetDouble(SHVDouble val)
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
		case SHVDataVariant::TypeBool:
			Data.boolVal = val != 0;
			break;
		case SHVDataVariant::TypeDouble:
			Data.doubleVal = val;
			break;
		case SHVDataVariant::TypeString:
			Data.stringVal = new SHVString();
			Data.stringVal->Format(_T("%g"), val);
			break;
		}
	}
}

SHVTime SHVDataVariant_impl::AsTime() const
{
SHVTime retVal;
	SHVASSERT(DataType == SHVDataVariant::TypeTime);
	if (DataType == SHVDataVariant::TypeTime)
	{
		retVal = *Data.timeVal;
	}
	return retVal;
}
void SHVDataVariant_impl::SetTime(const SHVTime& val)
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

SHVBool SHVDataVariant_impl::AsBool() const
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

void SHVDataVariant_impl::SetBool(SHVBool val)
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

SHVBool SHVDataVariant_impl::IsNull() const
{
	return isNull;
}

void SHVDataVariant_impl::SetNull()
{
	if (DataType == SHVDataVariant::TypeTime && Data.timeVal)
		delete Data.timeVal;
	if (DataType == SHVDataVariant::TypeString && Data.stringVal)
		delete Data.stringVal;
	isNull = true;
}

SHVDataVariant& SHVDataVariant_impl::operator=(const SHVDataVariant& val)
{
	switch (val.GetDataType())
	{
		case SHVDataVariant::TypeInt:
			SetInt(val.AsInt());
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

bool SHVDataVariant_impl::operator==(const SHVDataVariant& val) const
{
	if (DataType == val.GetDataType())
	{
		switch (DataType)
		{
		case SHVDataVariant::TypeInt:
			return AsInt() == val.AsInt();
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
