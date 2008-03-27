#include "stdafx.h"

#include "../../../../include/platformspc.h"
#include "../../include/shvdatarowcsqlite.h"
#include "../../include/shvdatarowimpl.h"
#include "../../include/shvdatarowkeyimpl.h"
#include "../../include/shvdatavariantimpl.h"


/*************************************
 * Constructor
 *************************************/
SHVDataRowC_SQLite::SHVDataRowC_SQLite(SHVDataRowListC* select)
{
	Select = select;
}

SHVDataRowC_SQLite::~SHVDataRowC_SQLite()
{
}

/*************************************
 * AsString
 *************************************/
SHVStringBuffer SHVDataRowC_SQLite::AsString(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetRowProvider();
SHVString retVal;
int len;
	if (!IsNull(colIdx))
		Statement->GetString(retVal, len, (int) colIdx);
	return retVal.ReleaseBuffer();
}

/*************************************
 * AsInt
 *************************************/
SHVInt SHVDataRowC_SQLite::AsInt(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetRowProvider();
SHVInt retVal;
long val;
	if (!IsNull(colIdx))
	{
		Statement->GetLong(val, (int) colIdx);
		retVal = SHVInt(val);
	}		
	return retVal;
}

/*************************************
 * AsDouble
 *************************************/
SHVDouble SHVDataRowC_SQLite::AsDouble(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetRowProvider();
SHVDouble retVal;
double val;
	if (!IsNull(colIdx))
	{
		Statement->GetDouble(val, (int) colIdx);
		retVal = SHVDouble(val);
	}		
	return retVal;
}

/*************************************
 * AsTime
 *************************************/
SHVTime SHVDataRowC_SQLite::AsTime(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetRowProvider();
int len;
SHVTime retVal;
SHVString val;
	if (!IsNull(colIdx))
	{
		Statement->GetString(val, len, (int) colIdx);
		retVal.SetFromDateString(val);
	}		
	return retVal;
}

/*************************************
 * AsBool
 *************************************/
SHVBool SHVDataRowC_SQLite::AsBool(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetRowProvider();
SHVBool retVal;
long val;
	if (!IsNull(colIdx))
	{
		Statement->GetLong(val, (int) colIdx);
		retVal = val != 0;
	}		
	return retVal;
}

/*************************************
 * IsNull
 *************************************/
SHVBool SHVDataRowC_SQLite::IsNull(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetRowProvider();
SHVBool retVal(SHVBool::True);
short type;
	SHVASSERT(colIdx != SIZE_T_MAX);
	if (colIdx != SIZE_T_MAX && Statement->GetColumnAffinity(type, (int) colIdx))
		retVal = type == SHVSQLiteWrapper::Affinity_Null;
	return retVal;
}

/*************************************
 * ColumnIndex
 *************************************/
size_t SHVDataRowC_SQLite::ColumnIndex(const SHVString8C& columnName) const
{
size_t retVal;
	SHVASSERT(RowValid());
	if (RowValid())
	{
		if (!Select->GetStruct()->FindColumnIndex(retVal, columnName))
			retVal = SIZE_T_MAX;
	}
	return retVal;
}


/*************************************
 * GetKey
 *************************************/
SHVDataRowKey* SHVDataRowC_SQLite::GetKey(size_t index) const
{
	SHVASSERT(RowValid());

SHVDataRowKeyRef ResultDef = (SHVDataRowKey*) Select->GetStruct()->GetIndex(index);
SHVDataRowKey_impl* Result = new SHVDataRowKey_impl();
	if (RowValid())
	{
		for (size_t i = 0; i < ResultDef->Count(); i++)
		{
			Result->AddKey((*ResultDef)[i].Key, GetValue(ColumnIndex((*ResultDef)[i].Key)), (*ResultDef)[i].Desc);
		}
	}
	return Result;
}

/*************************************
 * GetValue
 *************************************/
SHVDataVariant* SHVDataRowC_SQLite::GetValue(size_t colIdx) const
{
	SHVASSERT(RowValid());
SHVDataVariant* retVal = NULL;
	switch ((*Select->GetStruct())[colIdx]->GetDataType())
	{
		case SHVDataVariant::TypeInt:
			retVal = new SHVDataVariant_impl(AsInt(colIdx));
			break;
		case SHVDataVariant::TypeBool:
			retVal = new SHVDataVariant_impl(AsBool(colIdx));
			break;
		case SHVDataVariant::TypeDouble:
			retVal = new SHVDataVariant_impl(AsDouble(colIdx));
			break;
		case SHVDataVariant::TypeString:
			retVal = new SHVDataVariant_impl(AsString(colIdx));
			break;
		case SHVDataVariant::TypeTime:
			retVal = new SHVDataVariant_impl(AsTime(colIdx));
			break;
	}
	return retVal;
}

/*************************************
 * MatchKey
 *************************************/
SHVBool SHVDataRowC_SQLite::MatchKey(const SHVDataRowKey* key) const
{
SHVBool match(SHVBool::True);
	if (RowValid())
	{
	const SHVDataRowKey& Keys = *key;
	const SHVDataStructC& Struct = *Select->GetStruct();
		for (size_t i = 0; i < Keys.Count() && match; i++)
		{
		size_t colIdx;
			if (Struct.FindColumnIndex(colIdx, Keys[i].Key))
			{
				switch (Struct[colIdx]->GetDataType())
				{
				case SHVDataVariant::TypeInt:
					if (Keys[i].Value)
						match = Keys[i].Value->AsInt() == AsInt(colIdx);
					else
						match = AsInt(colIdx).IsNull();
					break;
				case SHVDataVariant::TypeDouble:
					if (Keys[i].Value)
						match = Keys[i].Value->AsDouble() == AsDouble(colIdx);
					else
						match = AsDouble(colIdx).IsNull();
					break;
				case SHVDataVariant::TypeString:
					if (Keys[i].Value)
						match = Keys[i].Value->AsString() == AsString(colIdx);
					else
						match = AsString(colIdx).IsNull();
					break;
				case SHVDataVariant::TypeTime:
					if (Keys[i].Value)
						match = Keys[i].Value->AsTime() == AsTime(colIdx);
					else
						match = AsTime(colIdx).IsNull();
					break;
				case SHVDataVariant::TypeBool:
					if (Keys[i].Value)
						match = Keys[i].Value->AsBool() == AsBool(colIdx);
					else
						match = AsBool(colIdx);
					break;
				}
			}
		}
	}
	return match;
}

/*************************************
 * RowValid
 *************************************/
SHVBool SHVDataRowC_SQLite::RowValid() const
{
	return Select != NULL && Select->GetRowProvider() != NULL;
}

/*************************************
 * GetStruct
 *************************************/
const SHVDataStructC* SHVDataRowC_SQLite::GetStruct() const
{
	if (RowValid())
		return Select->GetStruct();
	else
		return NULL;
}

/*************************************
 * GetAlias
 *************************************/
const SHVString8C SHVDataRowC_SQLite::GetAlias() const
{
	if (RowValid())
		return Select->GetAlias();
	else
		return "";	
}


/*************************************
 * GetRowListC
 *************************************/
const SHVDataRowListC* SHVDataRowC_SQLite::GetRowListC()
{
	return Select;
}

/*************************************
 * GetRowState
 *************************************/
int SHVDataRowC_SQLite::GetRowState() const
{
	return (RowValid() ? SHVDataRow::RowStateUnchanged : SHVDataRow::RowStateInvalid);
}

