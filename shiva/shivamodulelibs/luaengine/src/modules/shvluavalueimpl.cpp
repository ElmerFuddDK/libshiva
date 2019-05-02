#include "stdafx.h"
#include "shiva/include/platformspc.h"
#include "shiva/include/utils/shvmath.h"

#include "shvluavalueimpl.h"


//=========================================================================================================
// SHVLuaValueImpl class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVLuaValueImpl::SHVLuaValueImpl()
{
}

/*************************************
 * Destructor
 *************************************/
SHVLuaValueImpl::~SHVLuaValueImpl()
{
	switch (Type)
	{
	case TypeRefOject:
		Data.ObjVal->DestroyRef();
		break;
	case TypeString:
		delete Data.StringVal;
		break;
	default:
		break;
	}
}

/*************************************
 * GetType
 *************************************/
SHVLuaValue::Types SHVLuaValueImpl::GetType() const
{
	return Type;
}

/*************************************
 * GetRefType
 *************************************/
const char* SHVLuaValueImpl::GetRefType() const
{
	return (Type == TypeRefOject ? RefTypeID : NULL);
}

/*************************************
 * AsInt
 *************************************/
SHVInt SHVLuaValueImpl::AsInt() const
{
	switch (Type)
	{
	case TypeInt:
		return Data.IntVal;
	case TypeDouble:
		return (int)Data.DoubleVal;
	case TypeBool:
		return Data.BoolVal;
	case TypeString:
		return (int)SHVMath::Eval(*Data.StringVal);
	default:
	case TypeNull:
		break;
	}
	return SHVInt();
}

/*************************************
 * AsDouble
 *************************************/
SHVDouble SHVLuaValueImpl::AsDouble() const
{
	switch (Type)
	{
	case TypeInt:
		return (double)Data.IntVal;
	case TypeDouble:
		return Data.DoubleVal;
	case TypeBool:
		return (double)Data.BoolVal;
	case TypeString:
		return SHVMath::Eval(*Data.StringVal);
	default:
	case TypeNull:
		break;
	}
	return SHVDouble();
}

/*************************************
 * AsString
 *************************************/
SHVStringBuffer SHVLuaValueImpl::AsString() const
{
SHVString retVal;
	switch (Type)
	{
	case TypeInt:
		retVal = SHVStringC::LongToString(Data.IntVal);
		break;
	case TypeDouble:
		retVal = SHVStringC::DoubleToString(Data.DoubleVal);
		break;
	case TypeBool:
		retVal = SHVStringC::LongToString(Data.BoolVal);
		break;
	case TypeString:
		retVal = *Data.StringVal;
		break;
	default:
	case TypeNull:
		break;
	}
	return retVal.ReleaseBuffer();
}

/*************************************
 * AsBool
 *************************************/
SHVBool SHVLuaValueImpl::AsBool() const
{
	switch (Type)
	{
	case TypeInt:
		return Data.IntVal;
	case TypeDouble:
		return (int)Data.DoubleVal;
	case TypeBool:
		return Data.BoolVal;
	case TypeString:
		return (int)SHVMath::Eval(*Data.StringVal);
	default:
	case TypeNull:
		break;
	}
	return SHVBool();
}

/*************************************
 * AsObject
 *************************************/
SHVRefObject* SHVLuaValueImpl::AsRef(const char* typeID) const
{
	switch (Type)
	{
	case TypeRefOject:
		return (SHVStringUTF8C(RefTypeID) == SHVStringUTF8C(typeID) ? Data.ObjVal : NULL);
	default:
	case TypeNull:
		break;
	}
	return NULL;
}

/*************************************
 * NewNull
 *************************************/
SHVLuaValueImpl* SHVLuaValueImpl::NewNull()
{
SHVLuaValueImpl* retVal = new SHVLuaValueImpl();
	retVal->Type = TypeNull;
	return retVal;
}

/*************************************
 * NewInt
 *************************************/
SHVLuaValueImpl* SHVLuaValueImpl::NewInt(SHVInt val)
{
SHVLuaValueImpl* retVal = new SHVLuaValueImpl();
	if (val.IsNull())
	{
		retVal->Type = TypeNull;
	}
	else
	{
		retVal->Type = TypeInt;
		retVal->Data.IntVal = val;
	}
	return retVal;
}

/*************************************
 * NewDouble
 *************************************/
SHVLuaValueImpl* SHVLuaValueImpl::NewDouble(SHVDouble val)
{
SHVLuaValueImpl* retVal = new SHVLuaValueImpl();
	if (val.IsNull())
	{
		retVal->Type = TypeNull;
	}
	else
	{
		retVal->Type = TypeDouble;
		retVal->Data.DoubleVal = val;
	}
	return retVal;
}

/*************************************
 * NewString
 *************************************/
SHVLuaValueImpl* SHVLuaValueImpl::NewString(const SHVStringC val)
{
SHVLuaValueImpl* retVal = new SHVLuaValueImpl();
	if (val.IsNull())
	{
		retVal->Type = TypeNull;
	}
	else
	{
		retVal->Type = TypeString;
		retVal->Data.StringVal = new SHVString(val);
	}
	return retVal;
}

/*************************************
 * NewBool
 *************************************/
SHVLuaValueImpl* SHVLuaValueImpl::NewBool(SHVBool val)
{
SHVLuaValueImpl* retVal = new SHVLuaValueImpl();
	retVal->Type = TypeBool;
	retVal->Data.BoolVal = val.GetError();
	return retVal;
}

/*************************************
 * NewObject
 *************************************/
SHVLuaValueImpl* SHVLuaValueImpl::NewRef(SHVRefObject* val, const char* typeID)
{
SHVLuaValueImpl* retVal = new SHVLuaValueImpl();
	if (!val)
	{
		retVal->Type = TypeNull;
	}
	else
	{
		retVal->Type = TypeRefOject;
		retVal->Data.ObjVal = val->CreateRef();
		retVal->RefTypeID = typeID;
	}
	return retVal;
}

//=========================================================================================================
// SHVLuaValuesImpl class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVLuaValuesImpl::SHVLuaValuesImpl()
{
}

/*************************************
 * GetCount
 *************************************/
int SHVLuaValuesImpl::GetCount()
{
	return (int)Values.CalculateCount();
}

/*************************************
 * Get
 *************************************/
SHVLuaValue* SHVLuaValuesImpl::Get(int idx)
{
	return Values[idx];
}

/*************************************
 * AddInt
 *************************************/
SHVLuaValues* SHVLuaValuesImpl::AddInt(SHVInt val)
{
	Values.Add(SHVLuaValueImpl::NewInt(val));
	return this;
}

/*************************************
 * AddDouble
 *************************************/
SHVLuaValues* SHVLuaValuesImpl::AddDouble(SHVDouble val)
{
	Values.Add(SHVLuaValueImpl::NewDouble(val));
	return this;
}

/*************************************
 * AddString
 *************************************/
SHVLuaValues* SHVLuaValuesImpl::AddString(const SHVStringC val)
{
	Values.Add(SHVLuaValueImpl::NewString(val));
	return this;
}

/*************************************
 * AddBool
 *************************************/
SHVLuaValues* SHVLuaValuesImpl::AddBool(SHVBool val)
{
	Values.Add(SHVLuaValueImpl::NewBool(val));
	return this;
}

/*************************************
 * AddRefObject
 *************************************/
SHVLuaValues* SHVLuaValuesImpl::AddRef(SHVRefObject* val, const char* typeID)
{
	Values.Add(SHVLuaValueImpl::NewRef(val,typeID));
	return this;
}
SHVLuaValues* SHVLuaValuesImpl::AddRef(SHVLuaValues::RefStruct val)
{
	Values.Add(SHVLuaValueImpl::NewRef(val.Obj,val.TypeID));
	return this;
}

/*************************************
 * AddValue
 *************************************/
void SHVLuaValuesImpl::AddValue(SHVLuaValue* value)
{
	Values.Add(value);
}
