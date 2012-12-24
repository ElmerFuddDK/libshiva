#ifndef __SHIVA_DATAENGINE_DATAROWC_H
#define __SHIVA_DATAENGINE_DATAROWC_H

#include "../../shvtypes.h"
#include "../../utils/shvrefobject.h"
#include "../../utils/shvstring.h"
#include "../../utils/shvtime.h"
#include "shvdatavariant.h"

// forward declares
class SHVDataRowKey;
class SHVDataStructC;
class SHVDataRowListC;

//-=========================================================================================================
/// SHVDataRow class - Interface for SHVDataRow
/**
 * Interface for a generic shiva datarow
 */
class SHVDataRowC: public SHVRefObject
{
public:
	virtual SHVStringBuffer AsString(size_t colIdx) const = 0;
	virtual SHVStringBuffer AsDBString(size_t colIdx) const = 0;
	virtual SHVInt AsInt(size_t colIdx) const = 0;
	virtual SHVInt64 AsInt64(size_t colIdx) const = 0;
	virtual SHVDouble AsDouble(size_t colIdx) const = 0;
	virtual SHVTime AsTime(size_t colIdx) const = 0;
	virtual SHVBool AsBool(size_t colIdx) const = 0;
	virtual SHVBool IsNull(size_t colIdx) const = 0;

	virtual size_t ColumnIndex(const SHVString8C& colName) const = 0;

	virtual SHVDataRowKey* GetKey(size_t index = 0) const = 0;
	
	virtual SHVDataVariant* GetValue(size_t colIdx) const = 0;

	virtual SHVBool MatchKey(const SHVDataRowKey* key) const = 0;
	virtual SHVBool RowValid() const = 0;
	virtual const SHVDataStructC* GetStruct() const = 0;
	virtual const SHVString8C GetAlias() const = 0;

	virtual int GetRowState() const = 0;
	virtual const SHVDataRowListC* GetRowListC() = 0;

	inline SHVStringBuffer AsString(const SHVString8C& colName) const;
	inline SHVStringBuffer AsDBString(const SHVString8C& colName) const;
	inline SHVInt AsInt(const SHVString8C& colName) const;
	inline SHVInt64 AsInt64(const SHVString8C& colName) const;
	inline SHVDouble AsDouble(const SHVString8C& colName) const;
	inline SHVTime AsTime(const SHVString8C& colName) const;
	inline SHVBool AsBool(const SHVString8C& colName) const;
	inline SHVBool IsNull(const SHVString8C& colName) const;
	inline SHVDataVariant* GetValue(const SHVString8C& colName) const;

	inline SHVBool GetValue(const SHVString8C& colName, SHVString& value) const;
	inline SHVBool GetValue(const SHVString8C& colName, SHVInt& value) const;
	inline SHVBool GetValue(const SHVString8C& colName, SHVInt64& value) const;
	inline SHVBool GetValue(const SHVString8C& colName, SHVDouble& value) const;
	inline SHVBool GetValue(const SHVString8C& colName, SHVTime& value) const;
	inline SHVBool GetValue(const SHVString8C& colName, SHVBool& value) const;	

protected:
	virtual ~SHVDataRowC() { }
};

typedef SHVRefObjectContainer<SHVDataRowC> SHVDataRowCRef;

#endif

// ====================================== implementation - SHVDataRowC ==================================== //
#ifndef __SHIVA_DATAENGINE_DATAROWC_INL
#define __SHIVA_DATAENGINE_DATAROWC_INL

/*************************************
 * AsString
 *************************************/
SHVStringBuffer SHVDataRowC::AsString(const SHVString8C& colName) const
{
	return AsString(ColumnIndex(colName));
}

/*************************************
 * AsDBString
 *************************************/
SHVStringBuffer SHVDataRowC::AsDBString(const SHVString8C &colName) const
{
	return AsDBString(ColumnIndex(colName));
}

/*************************************
 * AsInt
 *************************************/
SHVInt SHVDataRowC::AsInt(const SHVString8C& colName) const
{
	return AsInt(ColumnIndex(colName));
}

/*************************************
 * AsInt64
 *************************************/
SHVInt64 SHVDataRowC::AsInt64(const SHVString8C& colName) const
{
	return AsInt64(ColumnIndex(colName));
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

SHVBool SHVDataRowC::GetValue(const SHVString8C& colName, SHVInt64& value)  const
{
	value = AsInt64(colName);
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
