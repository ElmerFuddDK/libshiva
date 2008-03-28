#ifndef __SHIVA_DATAENGINE_DATAVARIANT_IMPL_H
#define __SHIVA_DATAENGINE_DATAVARIANT_IMPL_H

#include "../../../include/modules/dataengine/shvdatavariant.h"

//-=========================================================================================================
///  SHVDataVariantImpl class - Implements the shiva variant data type.
/**
 * I know this is evil.. But this class makes it easier to create dynamic datastructure as a datarow or
 * a datakey.
 */
class SHVDataVariantImpl: public SHVDataVariant
{
public:
	virtual ~SHVDataVariantImpl();

	// from SHVDataVariant
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

	// inlines
	inline SHVDataVariantImpl();
	inline SHVDataVariantImpl(SHVInt val);
	inline SHVDataVariantImpl(SHVDouble val);
	inline SHVDataVariantImpl(SHVBool val);
	inline SHVDataVariantImpl(const SHVString& val);
	inline SHVDataVariantImpl(const SHVTime& val);
	inline SHVDataVariantImpl(const SHVDataVariant* copy);
	inline SHVDataVariant& operator=(const SHVDataVariantImpl& val);


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

// ================================ implementation - SHVDataVariantImpl ================================ //

/*************************************
 * Inline constructors
 *************************************/
SHVDataVariantImpl::SHVDataVariantImpl()
{
	DataType = SHVDataVariant::TypeUndefined;
	Data.stringVal = NULL;
}

SHVDataVariantImpl::SHVDataVariantImpl(SHVInt val)
{
	DataType = SHVDataVariant::TypeInt;
	SetInt(val);
}
SHVDataVariantImpl::SHVDataVariantImpl(SHVDouble val)
{
	DataType = SHVDataVariant::TypeDouble;
	SetDouble(val);
}

SHVDataVariantImpl::SHVDataVariantImpl(SHVBool val)
{
	DataType = SHVDataVariant::TypeBool;
	SetBool(val);
}

SHVDataVariantImpl::SHVDataVariantImpl(const SHVString& val)
{
	DataType = SHVDataVariant::TypeString;
	Data.stringVal = NULL;
	SetString(val);
}

SHVDataVariantImpl::SHVDataVariantImpl(const SHVTime& val)
{
	DataType = SHVDataVariant::TypeTime;
	Data.stringVal = NULL;
	SetTime(val);
}
SHVDataVariantImpl::SHVDataVariantImpl(const SHVDataVariant* copy)
{
	DataType = SHVDataVariant::TypeUndefined;
	Data.stringVal = NULL;
	*this = *copy;
}

/*************************************
 * operator =
 *************************************/
SHVDataVariant& SHVDataVariantImpl::operator=(const SHVDataVariantImpl& val)
{
	*this = (const SHVDataVariant&) val;
	return *this;
}

#endif