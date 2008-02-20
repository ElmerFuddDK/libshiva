#ifndef __SHIVA_DATAENGINE_DATAVARIANT_IMPL_H
#define __SHIVA_DATAENGINE_DATAVARIANT_IMPL_H

#include "../shvdatavariant.h"

class SHVDataVariant_impl: public SHVDataVariant
{
public:
	inline SHVDataVariant_impl();
	inline SHVDataVariant_impl(SHVInt val);
	inline SHVDataVariant_impl(SHVDouble val);
	inline SHVDataVariant_impl(SHVBool val);
	inline SHVDataVariant_impl(const SHVString& val);
	inline SHVDataVariant_impl(const SHVTime& val);
	inline SHVDataVariant_impl(const SHVDataVariant* copy);
	virtual ~SHVDataVariant_impl();

	virtual int GetDataType() const;
	virtual SHVStringBuffer AsString() const;
	virtual void SetString(const SHVStringC& val);

	virtual SHVInt AsInt() const;
	virtual void SetInt(SHVInt val);

	virtual SHVDouble AsDouble() const;
	virtual void SetDouble(SHVDouble val);

	virtual SHVTime AsTime() const;
	virtual void SetTime(const SHVTime& val);

	virtual SHVBool AsBool() const;
	virtual void SetBool(SHVBool val);

	virtual SHVBool IsNull() const;
	virtual void SetNull();

	virtual SHVDataVariant& operator=(const SHVDataVariant& val);
	virtual bool operator==(const SHVDataVariant& val) const;

private:
	int DataType;
	union {
		int intVal;
		double doubleVal;
		bool boolVal;
		SHVTime* timeVal;
		SHVString* stringVal;
	} Data;	
	bool isNull;
};

SHVDataVariant_impl::SHVDataVariant_impl()
{
	DataType = SHVDataVariant::SHVDataType_Undefined;
	Data.stringVal = NULL;
}

SHVDataVariant_impl::SHVDataVariant_impl(SHVInt val)
{
	DataType = SHVDataVariant::SHVDataType_Int;
	SetInt(val);
}
SHVDataVariant_impl::SHVDataVariant_impl(SHVDouble val)
{
	DataType = SHVDataVariant::SHVDataType_Double;
	SetDouble(val);
}

SHVDataVariant_impl::SHVDataVariant_impl(SHVBool val)
{
	DataType = SHVDataVariant::SHVDataType_Bool;
	SetBool(val);
}

SHVDataVariant_impl::SHVDataVariant_impl(const SHVString& val)
{
	DataType = SHVDataVariant::SHVDataType_String;
	SetString(val);
}

SHVDataVariant_impl::SHVDataVariant_impl(const SHVTime& val)
{
	DataType = SHVDataVariant::SHVDataType_Time;
	SetTime(val);
}
SHVDataVariant_impl::SHVDataVariant_impl(const SHVDataVariant* copy)
{
	*this = *copy;
}

#endif