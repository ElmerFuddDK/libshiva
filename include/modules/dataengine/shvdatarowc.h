#ifndef __SHIVA_DATAENGINE_DATAROWC_H
#define __SHIVA_DATAENGINE_DATAROWC_H

#include "../../../include/utils/shvrefobject.h"
#include "../../../include/utils/shvstring.h"
#include "../../../include/shvtypes.h"
#include "../../../include/utils/shvtime.h"
#include "shvdatarowkey.h"
#include "shvdatastructc.h"

//-=========================================================================================================
/// SHVDataRow class - Interface for SHVDataRow
/**
 * Interface for a generic shiva datarow
 */
class SHVDataRowC: public SHVRefObject
{
public:
	inline SHVStringBuffer AsString(const SHVString8C& colName) const;
	virtual SHVStringBuffer AsString(size_t colIdx) const = 0;

	inline SHVInt AsInt(const SHVString8C& colName) const;
	virtual SHVInt AsInt(size_t colIdx) const = 0;

	inline SHVDouble AsDouble(const SHVString8C& colName) const;
	virtual SHVDouble AsDouble(size_t colIdx) const = 0;

	inline SHVTime AsTime(const SHVString8C& colName) const;
	virtual SHVTime AsTime(size_t colIdx) const = 0;

	inline SHVBool AsBool(const SHVString8C& colName) const;
	virtual SHVBool AsBool(size_t colIdx) const = 0;

	inline SHVBool IsNull(const SHVString8C& colName) const;
	virtual SHVBool IsNull(size_t colIdx) const = 0;

	virtual size_t ColumnIndex(const SHVString8C& colName) const = 0;

	virtual SHVDataRowKey* GetKey(size_t index = 0) const = 0;
	
	virtual SHVDataVariant* GetValue(size_t colIdx) const = 0;
	inline SHVDataVariant* GetValue(const SHVString8C& colName) const;

	virtual SHVBool MatchKey(const SHVDataRowKey* key) const = 0;
	virtual SHVBool RowValid() const = 0;
	virtual const SHVDataStructC* GetStruct() const = 0;

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


// ====================================== implementation - SHVDataRowC ==================================== //

/*************************************
 * AsString
 *************************************/
SHVStringBuffer SHVDataRowC::AsString(const SHVString8C& colName) const
{
	return AsString(ColumnIndex(colName));
}
/*************************************
 * AsInt
 *************************************/
SHVInt SHVDataRowC::AsInt(const SHVString8C& colName) const
{
	return AsInt(ColumnIndex(colName));
}

/*************************************
 * AsDouble
 *************************************/
SHVDouble SHVDataRowC::AsDouble(const SHVString8C& colName) const
{
	return AsDouble(ColumnIndex(colName));
}

/*************************************
 * AsTime
 *************************************/
SHVTime SHVDataRowC::AsTime(const SHVString8C& colName) const
{
	return AsTime(ColumnIndex(colName));
}

/*************************************
 * AsBool
 *************************************/
SHVBool SHVDataRowC::AsBool(const SHVString8C& colName) const
{
	return AsBool(ColumnIndex(colName));
}

/*************************************
 * IsNull
 *************************************/
SHVBool SHVDataRowC::IsNull(const SHVString8C& colName) const
{
	return IsNull(ColumnIndex(colName));
}

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

/*************************************
 * GetValue
 *************************************/
SHVDataVariant* SHVDataRowC::GetValue(const SHVString8C& colName) const
{
	return GetValue(ColumnIndex(colName));
}

#endif