#include "stdafx.h"

#include "../../../../include/platformspc.h"
#include "../../include/dataengineimpl/shvdatarowc_sqlite.h"
#include "../../include/dataengineimpl/shvdatarowkey_impl.h"
#include "../../include/dataengineimpl/shvdatavariant_impl.h"


SHVDataRowC_SQLite::SHVDataRowC_SQLite(SHVDataRowListC* select)
{
	Select = select;
}

SHVDataRowC_SQLite::~SHVDataRowC_SQLite()
{
}

SHVStringBuffer SHVDataRowC_SQLite::AsString(const SHVString8C& colName) const
{
bool found;
size_t colIdx = ColIndex(colName, found);
SHVString retVal;

	if (found) 
		retVal = AsString(colIdx);
	return retVal.ReleaseBuffer();
}

SHVInt SHVDataRowC_SQLite::AsInt(const SHVString8C& colName) const
{
bool found;
size_t colIdx = ColIndex(colName, found);
SHVInt retVal;

	if (found) 
		retVal = AsInt(colIdx);
	return retVal;
}

SHVDouble SHVDataRowC_SQLite::AsDouble(const SHVString8C& colName) const
{
bool found;
size_t colIdx = ColIndex(colName, found);
SHVDouble retVal;

	if (found) 
		retVal = AsDouble(colIdx);
	return retVal;
}

SHVTime SHVDataRowC_SQLite::AsTime(const SHVString8C& colName) const
{
bool found;
size_t colIdx = ColIndex(colName, found);
SHVTime retVal;

	if (found) 
		retVal = AsTime(colIdx);
	return retVal;
}

SHVBool SHVDataRowC_SQLite::AsBool(const SHVString8C& colName) const
{
bool found;
size_t colIdx = ColIndex(colName, found);
SHVBool retVal;

	if (found) 
		retVal = AsBool(colIdx);
	return retVal;
}

SHVBool SHVDataRowC_SQLite::IsNull(const SHVString8C& colName) const
{
bool found;
SHVBool retVal(SHVBool::True);
size_t colIdx = ColIndex(colName, found);

	if (found)
	{
		retVal = IsNull(colIdx);
	}
	return retVal;
}

SHVStringBuffer SHVDataRowC_SQLite::AsString(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetProvider();
SHVString retVal;
int len;
	if (!IsNull(colIdx))
		Statement->GetValue(retVal, len, (int) colIdx);
	return retVal.ReleaseBuffer();
}

SHVInt SHVDataRowC_SQLite::AsInt(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetProvider();
SHVInt retVal;
long val;
	if (!IsNull(colIdx))
	{
		Statement->GetValue(val, (int) colIdx);
		retVal = SHVInt(val);
	}		
	return retVal;
}

SHVDouble SHVDataRowC_SQLite::AsDouble(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetProvider();
SHVDouble retVal;
double val;
	if (!IsNull(colIdx))
	{
		Statement->GetValue(val, (int) colIdx);
		retVal = SHVDouble(val);
	}		
	return retVal;
}

SHVTime SHVDataRowC_SQLite::AsTime(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetProvider();
int len;
SHVTime retVal;
SHVString val;
	if (!IsNull(colIdx))
	{
		Statement->GetValue(val, len, (int) colIdx);
		retVal.SetFromDateString(val);
	}		
	return retVal;
}

SHVBool SHVDataRowC_SQLite::AsBool(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetProvider();
SHVBool retVal;
long val;
	if (!IsNull(colIdx))
	{
		Statement->GetValue(val, (int) colIdx);
		retVal = val != 0;
	}		
	return retVal;
}

SHVBool SHVDataRowC_SQLite::IsNull(size_t colIdx) const
{
const SHVSQLiteStatement* Statement = (const SHVSQLiteStatement*) Select->GetProvider();
SHVBool retVal(SHVBool::True);
short type;
	if (Statement->GetColumnType(type, (int) colIdx))
		retVal = type == SHVSQLiteWrapper::SQLite_Null;
	return retVal;
}


SHVDataRowKey* SHVDataRowC_SQLite::GetKey(size_t index) const
{
	SHVASSERT(RowValid());

SHVDataRowKeyRef ResultDef = (SHVDataRowKey*) Select->GetStruct()->GetIndex(index);
SHVDataRowKey_impl* Result = new SHVDataRowKey_impl();
	if (RowValid())
	{
		for (size_t i = 0; i < ResultDef->Count(); i++)
		{
			Result->AddKey((*ResultDef)[i].Key, GetValue((*ResultDef)[i].Key), (*ResultDef)[i].Desc);
		}
	}
	return Result;
}
SHVDataVariant* SHVDataRowC_SQLite::GetValue(size_t colIdx) const
{
	SHVASSERT(RowValid());
SHVDataVariant* retVal = NULL;
	switch ((*Select->GetStruct())[colIdx]->GetDataType())
	{
		case SHVDataVariant::SHVDataType_Int:
			retVal = new SHVDataVariant_impl(AsInt(colIdx));
			break;
		case SHVDataVariant::SHVDataType_Bool:
			retVal = new SHVDataVariant_impl(AsBool(colIdx));
			break;
		case SHVDataVariant::SHVDataType_Double:
			retVal = new SHVDataVariant_impl(AsDouble(colIdx));
			break;
		case SHVDataVariant::SHVDataType_String:
			retVal = new SHVDataVariant_impl(AsString(colIdx));
			break;
		case SHVDataVariant::SHVDataType_Time:
			retVal = new SHVDataVariant_impl(AsTime(colIdx));
			break;
	}
	return retVal;
}
SHVDataVariant* SHVDataRowC_SQLite::GetValue(const SHVString8C& colName) const
{
	SHVASSERT(RowValid());

bool found;
SHVDataVariant* retVal = NULL;
size_t colIdx = ColIndex(colName, found);
	if (found)
		retVal = GetValue(colIdx);
	return retVal;	
}


SHVBool SHVDataRowC_SQLite::MatchKey(const SHVDataRowKey* key) const
{
SHVBool match(SHVBool::True);
	for (size_t i = 0; i < key->Count() && match; i++)
	{
		match = (*(*key)[i].Value == *GetValue((*key)[i].Key));
	}
	return match;
}

SHVBool SHVDataRowC_SQLite::RowValid() const
{
	return Select != NULL && Select->GetProvider() != NULL;
}

void SHVDataRowC_SQLite::ClearOwnership()
{
	Select = NULL;
}

size_t SHVDataRowC_SQLite::ColIndex(const SHVString8C& colName, bool& found) const
{
size_t colIdx;
	found = Select->GetStruct()->FindColumnIndex(colIdx, colName) == SHVBool::True;
	return colIdx;
}
