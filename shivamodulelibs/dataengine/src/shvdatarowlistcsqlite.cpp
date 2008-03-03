#include "stdafx.h"
#include "../../../../include/platformspc.h"
#include "../../../../include/utils/shvstringutf8.h"
#include "../../include/dataengineimpl/shvdatarowlistc_sqlite.h"
#include "../../include/dataengineimpl/shvdatarowc_sqlite.h"
#include "../../include/dataengineimpl/shvdatarow_impl.h"
#include "../../include/dataengineimpl/shvdatarowkey_impl.h"
#include "../../include/dataengineimpl/shvdatastruct_impl.h"
#include "../../include/dataengineimpl/shvdatastructc_sqlite.h"
#include "../../../../shivasqlite/include/sqlitestatement.h"


/*************************************
 * Constructor
 *************************************/
SHVDataRowListC_SQLite::SHVDataRowListC_SQLite(SHVSQLiteWrapper* sqlLite, const SHVStringC& sql, const SHVDataRowKey* sortKey)
{
SHVString rest;
SHVDataStruct_impl* st = new SHVDataStruct_impl();
	SQLite = sqlLite;
	StructCache = st;
	Statement = SQLite->Prepare(Ok, sql, rest);
	if (sortKey)
		st->AddIndex((SHVDataRowKey*) sortKey);
	SortIndex = 0;
	Eof = !Ok;
}

SHVDataRowListC_SQLite::SHVDataRowListC_SQLite(SHVSQLiteWrapper* sqlLite, const SHVDataStructC* dataStruct, const SHVStringC& condition, size_t index)
{
SHVStringSQLite rest(NULL);
	SHVASSERT(dataStruct->GetIndex(index));
	if (!dataStruct->GetIndex(index))
	{
		Ok = SHVBool::False;
		return;
	}
	SQLite = sqlLite;
	StructCache = (SHVDataStructC*) dataStruct;
	SortIndex = index;
	Statement = SQLite->PrepareUTF8(Ok, BuildQuery(condition, false), rest);
	Eof = !Ok;
}

SHVDataRowListC_SQLite::SHVDataRowListC_SQLite(SHVSQLiteWrapper* sqlLite, SHVSQLiteStatement* statement, const SHVDataStructC* dataStruct, size_t index)
{
	SHVASSERT(dataStruct->GetIndex(index));
	Ok = SHVBool::True;
	if (!dataStruct->GetIndex(index))
	{
		Ok = SHVBool::False;
		return;
	}
	SQLite = sqlLite;
	SortIndex = index;
	StructCache = (SHVDataStructC*) dataStruct;
	Statement = statement;
	Eof = !Ok;
}

SHVDataRowListC_SQLite::~SHVDataRowListC_SQLite()
{
}

/*************************************
 * GetCurrentRow
 *************************************/
const SHVDataRowC* SHVDataRowListC_SQLite::GetCurrentRow() const
{
	if (!Eof)
	{
		return CurrentRow.AsConst();
	}
	else
		return NULL;
}

/*************************************
 * NextRow
 *************************************/
SHVBool SHVDataRowListC_SQLite::NextRow()
{
	SHVASSERT(Ok);
	if (!Ok)
		return SHVBool::False;
	if (CurrentRow.IsNull())
		CurrentRow = new SHVDataRowC_SQLite(this);
	Eof = Statement->NextResult().GetError() != SHVSQLiteWrapper::SQLite_ROW;
	if (StructCache->GetColumnCount() == 0 && !Eof)
	{
	SHVStringSQLite colName(NULL);
	SHVStringSQLite type(NULL);
	int shvtype;
	int len;
		for (int col = 0; col < Statement->GetColumnCount(); col++)
		{
			if (Statement->GetColumnNameUTF8(colName, col))
			{
				Statement->GetColumnTypeUTF8(type, col);
				len = -1;
				shvtype = SHVDataVariant::TypeUndefined;
				if (type == __SQLITE_TYPE_INT)
					shvtype = SHVDataVariant::TypeInt;
				else
				if (type == __SQLITE_TYPE_DOUBLE)
					shvtype = SHVDataVariant::TypeDouble;
				else
				if (type == __SQLITE_TYPE_BOOL)
					shvtype = SHVDataVariant::TypeBool;
				else
				if (type == __SQLITE_TYPE_DATETIME)
					shvtype = SHVDataVariant::TypeTime;
				else
				if (type.Find(__SQLITE_TYPE_STRING) == 0)
				{
					shvtype = SHVDataVariant::TypeString;
					len = SHVString8C::StrToL(type.Mid(SHVString8C::StrLen(__SQLITE_TYPE_STRING) + 1).GetBufferConst(), NULL, 10);
				}
				((SHVDataStruct&) *StructCache).Add(colName.GetSafeBuffer(), shvtype, len);
			}
		}		
	}
	return !Eof;
}

/*************************************
 * Reset
 *************************************/
SHVBool SHVDataRowListC_SQLite::Reset()
{
	SHVASSERT(Ok);
	if (!Ok)
		return SHVBool::False;
	Eof = SHVBool::False;

	return Statement->Reset();
}


/*************************************
 * Find
 *************************************/
SHVDataRowC* SHVDataRowListC_SQLite::Find(const SHVDataRowKey* key)
{
const SHVDataRowKey& k = *key;
	Reset();
	for (size_t i = key->Count(); i;)
	{
	SHVStringUTF8 keyParm;
		keyParm.Format("@%s", k[--i].Key.GetSafeBuffer());
		if (k[i].Value)
		{
			if (k[i].Value->GetDataType() == SHVDataVariant::TypeInt)
				Statement->SetParameterLongUTF8(keyParm, k[i].Value->AsInt());
			else
			if (k[i].Value->GetDataType() == SHVDataVariant::TypeBool)
				Statement->SetParameterLongUTF8(keyParm, k[i].Value->AsInt());
			else
			if (k[i].Value->GetDataType() == SHVDataVariant::TypeDouble)
				Statement->SetParameterDoubleUTF8(keyParm, k[i].Value->AsDouble());
			else
			if (k[i].Value->GetDataType() == SHVDataVariant::TypeString)
				Statement->SetParameterStringUTF8(keyParm, k[i].Value->AsString().ToStrUTF8());
			else
			if (k[i].Value->GetDataType() == SHVDataVariant::TypeTime)
				Statement->SetParameterStringUTF8(keyParm, k[i].Value->AsString().ToStrUTF8());
			else
				Statement->SetParameterNullUTF8(keyParm);
		}
		else
			Statement->SetParameterNullUTF8(keyParm);
	}
	if (NextRow())
		return CurrentRow;
	else
		return NULL;
}

/*************************************
 * Reverse
 *************************************/
SHVDataRowListC* SHVDataRowListC_SQLite::Reverse(const SHVStringC& condition)
{
SHVBool ok;
SHVStringSQLite rest(NULL);
	SHVSQLiteStatement* statement = SQLite->PrepareUTF8(ok, BuildQuery(condition,true), rest);
	return new SHVDataRowListC_SQLite(SQLite, statement, StructCache, SortIndex);
}

/*************************************
 * GetStruct
 *************************************/
const SHVDataStructC* SHVDataRowListC_SQLite::GetStruct() const
{
	return StructCache.AsConst();
}

/*************************************
 * GetRowCount
 *************************************/
int SHVDataRowListC_SQLite::GetRowCount() const
{
	return Statement->GetRowCount();
}

/*************************************
 * GetRowProvider
 *************************************/
const void* SHVDataRowListC_SQLite::GetRowProvider() const
{
	return Statement.AsConst();
}

/*************************************
 * IsOk
 *************************************/
SHVBool SHVDataRowListC_SQLite::IsOk() const
{
	return Ok;
}

/*************************************
 * BuildQuery
 *************************************/
SHVStringBufferUTF8 SHVDataRowListC_SQLite::BuildQuery(const SHVStringC& condition, bool reverse)
{
SHVStringUTF8 queryUTF8;
SHVStringUTF8 conditionUTF8;
SHVStringUTF8 colConditionUTF8;
SHVStringUTF8 orderbyUTF8;
const SHVDataRowKey& Key = *StructCache->GetIndex(SortIndex);

	if (!condition.IsNull() && condition != _T(""))
		conditionUTF8.Format("(%s) and ", condition.ToStrUTF8().GetSafeBuffer());
	for (size_t i = 0; i < Key.Count(); i++)
	{
		if (i)
		{
			conditionUTF8 += " and ";
			orderbyUTF8 += ", ";
		}
		if (Key[i].Desc != reverse)
			colConditionUTF8.Format("(@%s is null or %s <= @%s)", Key[i].Key.GetSafeBuffer(), Key[i].Key.GetSafeBuffer(), Key[i].Key.GetSafeBuffer());
		else
			colConditionUTF8.Format("(@%s is null or %s >= @%s)", Key[i].Key.GetSafeBuffer(), Key[i].Key.GetSafeBuffer(), Key[i].Key.GetSafeBuffer());
		conditionUTF8 += colConditionUTF8;
		orderbyUTF8 += Key[i].Key.GetSafeBuffer();
		if (Key[i].Desc != reverse)
			orderbyUTF8 += " desc";
	}
	queryUTF8.Format("select * from %s where %s order by %s", 
		StructCache->GetTableName().GetSafeBuffer(),
		conditionUTF8.GetSafeBuffer(),
		orderbyUTF8.GetSafeBuffer());
	return queryUTF8.ReleaseBuffer();
}
