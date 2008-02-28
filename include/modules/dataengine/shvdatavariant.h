#ifndef __SHIVA_DATAVARIANT_H
#define __SHIVA_DATAVARIANT_H

#include "../../../include/utils/shvrefobject.h"
#include "../../../include/utils/shvstring.h"
#include "../../../include/utils/shvtime.h"
#include "../../../include/shvtypes.h"

class SHVDataVariant
{
public:
	enum SHVDataType
	{
		TypeUndefined,
		TypeInt,
		TypeDouble,
		TypeBool,
		TypeString,
		TypeTime
	};

	virtual ~SHVDataVariant() { }
	virtual int GetDataType() const = 0;

	virtual SHVStringBuffer AsString() const = 0;
	virtual void SetString(const SHVStringC& val) = 0;

	virtual SHVInt AsInt() const = 0;
	virtual void SetInt(SHVInt val) = 0;

	virtual SHVDouble AsDouble() const = 0;
	virtual void SetDouble(SHVDouble val) = 0;

	virtual SHVTime AsTime() const = 0;
	virtual void SetTime(const SHVTime& val) = 0;

	virtual SHVBool AsBool() const = 0;
	virtual void SetBool(SHVBool val) = 0;

	virtual SHVBool IsNull() const = 0;
	virtual void SetNull() = 0;

	virtual SHVDataVariant& operator=(const SHVDataVariant& val) = 0;
	virtual bool operator==(const SHVDataVariant& val) const = 0;
	inline SHVDataVariant& operator=(const SHVStringC& val);
	inline SHVDataVariant& operator=(SHVInt val);
	inline SHVDataVariant& operator=(SHVDouble val);
	inline SHVDataVariant& operator=(const SHVTime& val);
	inline SHVDataVariant& operator=(SHVBool val);
};

// =================================== implementation - SHVDataVariant =================================== //

/*************************************
 * operator =
 *************************************/

SHVDataVariant& SHVDataVariant::operator=(const SHVStringC& val)
{
	SetString(val);
	return *this;
}
SHVDataVariant& SHVDataVariant::operator=(SHVInt val)
{
	SetInt(val);
	return *this;
}

SHVDataVariant& SHVDataVariant::operator=(SHVDouble val)
{
	SetDouble(val);
	return *this;
}

SHVDataVariant& SHVDataVariant::operator=(const SHVTime& val)
{
	SetTime(val);
	return *this;
}

SHVDataVariant& SHVDataVariant::operator=(SHVBool val)
{
	SetBool(val);
	return *this;
}

#endif

