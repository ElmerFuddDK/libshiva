#ifndef __SHIVA_DATAENGINE_DATAROWC_H
#define __SHIVA_DATAENGINE_DATAROWC_H

#include "../../../include/utils/shvrefobject.h"
#include "../../../include/utils/shvstring.h"
#include "../../../include/shvtypes.h"
#include "../../../include/utils/shvtime.h"
#include "shvdatarowkey.h"

//-=========================================================================================================
/// SHVDataRow class - Interface for SHVDataRow
/**
 * Interface for a generic shiva datarow
 */

class SHVDataRowC: public SHVRefObject
{
public:
	virtual SHVStringBuffer AsString(const SHVString8C& colName) const = 0;
	virtual SHVInt AsInt(const SHVString8C& colName) const = 0;
	virtual SHVDouble AsDouble(const SHVString8C& colName) const = 0;
	virtual SHVTime AsTime(const SHVString8C& colName) const = 0;
	virtual SHVBool AsBool(const SHVString8C& colName) const = 0;
	virtual SHVBool IsNull(const SHVString8C& colName) const = 0;
	virtual SHVDataRowKey* GetKey(size_t index = 0) const = 0;
	virtual SHVDataVariant* GetValue(const SHVString8C& colName) const = 0;
	virtual SHVDataVariant* GetValue(size_t colIdx) const = 0;
	virtual SHVBool MatchKey(const SHVDataRowKey* key) const = 0;
	virtual SHVBool RowValid() const = 0;

	inline SHVBool GetValue(const SHVString8C& colName, SHVString& value) const;
	inline SHVBool GetValue(const SHVString8C& colName, SHVInt& value) const;
	inline SHVBool GetValue(const SHVString8C& colName, SHVDouble& value) const;
	inline SHVBool GetValue(const SHVString8C& colName, SHVTime& value) const;
	inline SHVBool GetValue(const SHVString8C& colName, SHVBool& value) const;	
	virtual void ClearOwnership() = 0;

protected:
	virtual ~SHVDataRowC() { }
};

typedef SHVRefObjectContainer<SHVDataRowC> SHVDataRowCRef;


// ====================================== implementation - SHVDataRow ===================================== //

/*************************************
 * GetValue
 *************************************/
SHVBool SHVDataRowC::GetValue(const SHVString8C& colName, SHVString& value) const
{
	value = AsString(colName);
	return !value.IsNull();
}

SHVBool SHVDataRowC::GetValue(const SHVString8C& colName, SHVInt& value)  const
{
	value = AsInt(colName);
	return !value.IsNull();
}

SHVBool SHVDataRowC::GetValue(const SHVString8C& colName, SHVDouble& value) const
{
	value = AsDouble(colName);
	return !value.IsNull();
}

SHVBool SHVDataRowC::GetValue(const SHVString8C& colName, SHVTime& value) const
{
	value = AsTime(colName);
	return !value.IsNull();
}

SHVBool SHVDataRowC::GetValue(const SHVString8C& colName, SHVBool& value) const
{
	SHVBool val = AsBool(colName);
	if (val.GetError() == 0 && val.GetError() == -1)
		value = val;
	return val.GetError() == 0 && val.GetError() == -1;
}

#endif