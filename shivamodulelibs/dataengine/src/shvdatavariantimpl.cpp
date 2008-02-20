#include "stdafx.h"
#include <stdlib.h>
#include "../../../../include/platformspc.h"
#include "../../include/dataengineimpl/shvdatavariant_impl.h"


SHVDataVariant_impl::~SHVDataVariant_impl()
{
	if (DataType == SHVDataVariant::SHVDataType_Time && Data.timeVal)
		delete Data.timeVal;
	if (DataType == SHVDataVariant::SHVDataType_String && Data.stringVal)
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
		case SHVDataVariant::SHVDataType_Int:
			retVal = SHVStringC::LongToString(Data.intVal);
			break;
		case SHVDataVariant::SHVDataType_Bool:
			retVal = Data.boolVal ? SHVString(_T("1")) : SHVString(_T("0"));
			break;
		case SHVDataVariant::SHVDataType_Double:
			retVal.Format(_T("%g"), Data.doubleVal);
			break;
		case SHVDataVariant::SHVDataType_String:
			retVal = Data.stringVal->GetBuffer();
			break;
		case SHVDataVariant::SHVDataType_Time:
			retVal = Data.timeVal->ToDateString();
			break;
		}
	}
	return retVal.ReleaseBuffer();
}

void SHVDataVariant_impl::SetString(const SHVStringC& val)
{
	if (DataType == SHVDataVariant::SHVDataType_Undefined)
		DataType = SHVDataVariant::SHVDataType_String;
	if (val.IsNull())
	{
		if (DataType == SHVDataVariant::SHVDataType_Time && Data.timeVal)
			delete Data.timeVal;
		if (DataType == SHVDataVariant::SHVDataType_String && Data.stringVal)
			delete Data.stringVal;
		Data.stringVal = NULL;
		isNull = true;
	}
	else
	{
		if (DataType == SHVDataVariant::SHVDataType_Time && Data.timeVal)
			delete Data.timeVal;
		if (DataType == SHVDataVariant::SHVDataType_String && Data.stringVal)
			delete Data.stringVal;
		switch (DataType)
		{
		case SHVDataVariant::SHVDataType_Int:
			Data.intVal = SHVStringC::StrToL(val.GetSafeBuffer(), NULL, 10);
			break;
		case SHVDataVariant::SHVDataType_Bool:
			Data.boolVal = val == _T("1");
			break;
		case SHVDataVariant::SHVDataType_Double:
#ifdef UNICODE
			Data.doubleVal = wcstod(val.GetSafeBuffer(), NULL);
#else
			Data.doubleVal = strtod(val.GetSafeBuffer(), NULL);
#endif
			break;
		case SHVDataVariant::SHVDataType_String:
			Data.stringVal = new SHVString(val);
			break;
		case SHVDataVariant::SHVDataType_Time:
			Data.timeVal->SetFromDateString(val);
			break;
		}
	}
}

SHVInt SHVDataVariant_impl::AsInt() const
{
SHVInt retVal;
	SHVASSERT(DataType != SHVDataVariant::SHVDataType_Time);
	if (DataType != SHVDataVariant::SHVDataType_Time)
	{
		if (!isNull)
		{
		switch (DataType)
		{
			case SHVDataVariant::SHVDataType_Int:
				retVal = Data.intVal;
				break;
			case SHVDataVariant::SHVDataType_Bool:
				retVal = SHVBool(Data.boolVal).GetError();
				break;
			case SHVDataVariant::SHVDataType_Double:
				retVal = (int) Data.doubleVal;
				break;
			case SHVDataVariant::SHVDataType_String:
				retVal = SHVStringC::StrToL(Data.stringVal->GetSafeBuffer(), NULL, 10);
				break;
			case SHVDataVariant::SHVDataType_Time:
				break;
			}
		}
	}
	return retVal;
}

void SHVDataVariant_impl::SetInt(SHVInt val)
{
	SHVASSERT(DataType != SHVDataVariant::SHVDataType_Time);
	if (DataType == SHVDataVariant::SHVDataType_Undefined)
		DataType = SHVDataVariant::SHVDataType_Int;
	if (val.IsNull())
	{
		if (DataType == SHVDataVariant::SHVDataType_String && Data.stringVal)
			delete Data.stringVal;
		Data.stringVal = NULL;
		isNull = true;
	}
	else
	if (DataType != SHVDataVariant::SHVDataType_Time)
	{
		if (DataType == SHVDataVariant::SHVDataType_String && Data.stringVal)
			delete Data.stringVal;
		switch (DataType)
		{
		case SHVDataVariant::SHVDataType_Int:
			Data.intVal = val;
			break;
		case SHVDataVariant::SHVDataType_Bool:
			Data.boolVal = val != 0;
			break;
		case SHVDataVariant::SHVDataType_Double:
			Data.doubleVal = (double) val;
			break;
		case SHVDataVariant::SHVDataType_String:
			Data.stringVal = new SHVString(SHVStringC::LongToString(val));
			break;
		}
	}
}

SHVDouble SHVDataVariant_impl::AsDouble() const
{
SHVDouble retVal;
	SHVASSERT(DataType != SHVDataVariant::SHVDataType_Time);
	if (DataType != SHVDataVariant::SHVDataType_Time)
	{
		if (!isNull)
		{
		switch (DataType)
		{
			case SHVDataVariant::SHVDataType_Int:
				retVal = (double) Data.intVal;
				break;
			case SHVDataVariant::SHVDataType_Bool:
				retVal = (double) SHVBool(Data.boolVal).GetError();
				break;
			case SHVDataVariant::SHVDataType_Double:
				retVal = Data.doubleVal;
				break;
			case SHVDataVariant::SHVDataType_String:
#ifdef UNICODE
				retVal = wcstod(Data.stringVal->GetSafeBuffer(), NULL);
#else
				retVal = strtod(Data.stringVal->GetSafeBuffer(), NULL);
#endif
				break;
			case SHVDataVariant::SHVDataType_Time:
				break;
			}
		}
	}
	return retVal;
}

void SHVDataVariant_impl::SetDouble(SHVDouble val)
{
	SHVASSERT(DataType != SHVDataVariant::SHVDataType_Time);
	if (DataType == SHVDataVariant::SHVDataType_Undefined)
		DataType = SHVDataVariant::SHVDataType_Double;
	if (val.IsNull())
	{
		if (DataType == SHVDataVariant::SHVDataType_Time && Data.timeVal)
			delete Data.timeVal;
		if (DataType == SHVDataVariant::SHVDataType_String && Data.stringVal)
			delete Data.stringVal;
		Data.stringVal = NULL;
		isNull = true;
	}
	else
	if (DataType != SHVDataVariant::SHVDataType_Time)
	{
		if (DataType == SHVDataVariant::SHVDataType_String && Data.stringVal)
			delete Data.stringVal;
		switch (DataType)
		{
		case SHVDataVariant::SHVDataType_Int:
			Data.intVal = (int) val;
			break;
		case SHVDataVariant::SHVDataType_Bool:
			Data.boolVal = val != 0;
			break;
		case SHVDataVariant::SHVDataType_Double:
			Data.doubleVal = val;
			break;
		case SHVDataVariant::SHVDataType_String:
			Data.stringVal = new SHVString();
			Data.stringVal->Format(_T("%g"), val);
			break;
		}
	}
}

SHVTime SHVDataVariant_impl::AsTime() const
{
SHVTime retVal;
	SHVASSERT(DataType == SHVDataVariant::SHVDataType_Time);
	if (DataType == SHVDataVariant::SHVDataType_Time)
	{
		retVal = *Data.timeVal;
	}
	return retVal;
}
void SHVDataVariant_impl::SetTime(const SHVTime& val)
{
	SHVASSERT(DataType == SHVDataVariant::SHVDataType_Time || DataType == SHVDataVariant::SHVDataType_String);
	if (DataType == SHVDataVariant::SHVDataType_Undefined)
		DataType = SHVDataVariant::SHVDataType_Time;
	if (val.IsNull())
	{
		if (DataType == SHVDataVariant::SHVDataType_Time && Data.timeVal)
			delete Data.timeVal;
		if (DataType == SHVDataVariant::SHVDataType_String && Data.stringVal)
			delete Data.stringVal;
		Data.stringVal = NULL;
		isNull = true;
	}
	else
	if (DataType == SHVDataVariant::SHVDataType_Time || DataType == SHVDataVariant::SHVDataType_String)
	{
		if (DataType == SHVDataVariant::SHVDataType_Time && Data.timeVal)
			delete Data.timeVal;
		if (DataType == SHVDataVariant::SHVDataType_String && Data.stringVal)
			delete Data.stringVal;
		switch (DataType)
		{
		case SHVDataVariant::SHVDataType_String:
			Data.stringVal = new SHVString(val.ToDateString());
			break;
		case SHVDataVariant::SHVDataType_Time:
			Data.timeVal = new SHVTime(val);
			break;
		}
	}
}

SHVBool SHVDataVariant_impl::AsBool() const
{
SHVBool retVal;
	SHVASSERT(DataType != SHVDataVariant::SHVDataType_Time);
	if (DataType != SHVDataVariant::SHVDataType_Time)
	{
		if (!isNull)
		{
		switch (DataType)
		{
			case SHVDataVariant::SHVDataType_Int:
				retVal = SHVBool(Data.intVal != 0);
				break;
			case SHVDataVariant::SHVDataType_Bool:
				retVal = SHVBool(Data.boolVal);
				break;
			case SHVDataVariant::SHVDataType_Double:
				retVal = SHVBool(Data.intVal != 0);
				break;
			case SHVDataVariant::SHVDataType_String:
				return *Data.stringVal == _T("1");
				break;
			case SHVDataVariant::SHVDataType_Time:
				break;
			}
		}
	}
	return retVal;
}

void SHVDataVariant_impl::SetBool(SHVBool val)
{
	SHVASSERT(DataType != SHVDataVariant::SHVDataType_Time);
	if (DataType == SHVDataVariant::SHVDataType_Undefined)
		DataType = SHVDataVariant::SHVDataType_Bool;
	if (DataType != SHVDataVariant::SHVDataType_Time)
	{
		if (DataType == SHVDataVariant::SHVDataType_String && Data.stringVal)
			delete Data.stringVal;
		switch (DataType)
		{
		case SHVDataVariant::SHVDataType_Int:
			Data.intVal = val.GetError();
			break;
		case SHVDataVariant::SHVDataType_Bool:
			Data.boolVal = val;
			break;
		case SHVDataVariant::SHVDataType_Double:
			Data.doubleVal = (double) val.GetError();
			break;
		case SHVDataVariant::SHVDataType_String:
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
	if (DataType == SHVDataVariant::SHVDataType_Time && Data.timeVal)
		delete Data.timeVal;
	if (DataType == SHVDataVariant::SHVDataType_String && Data.stringVal)
		delete Data.stringVal;
	isNull = true;
}

SHVDataVariant& SHVDataVariant_impl::operator=(const SHVDataVariant& val)
{
	switch (val.GetDataType())
	{
		case SHVDataVariant::SHVDataType_Int:
			SetInt(val.AsInt());
			break;
		case SHVDataVariant::SHVDataType_Bool:
			SetBool(val.AsBool());
			break;
		case SHVDataVariant::SHVDataType_Double:
			SetDouble(val.AsDouble());
			break;
		case SHVDataVariant::SHVDataType_String:
			SetString(val.AsString());
			break;
		case SHVDataVariant::SHVDataType_Time:
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
		case SHVDataVariant::SHVDataType_Int:
			return AsInt() == val.AsInt();
		case SHVDataVariant::SHVDataType_Bool:
			return AsBool() == val.AsBool();
		case SHVDataVariant::SHVDataType_Double:
			return AsDouble() == val.AsDouble();
		case SHVDataVariant::SHVDataType_String:
			return AsString() == val.AsString();
		case SHVDataVariant::SHVDataType_Time:
			return (AsTime() == val.AsTime() ? true : false);
			break;
		}
	}
	return false;
}
