#include "stdafx.h"
#include "../../../../include/platformspc.h"
#include "../../../../include/utils/shvstringutf8.h"
#include "../../include/dataengineimpl/shvdatarowlistc_sqlite.h"
#include "../../include/dataengineimpl/shvdatarowc_sqlite.h"
#include "../../include/dataengineimpl/shvdatarow_impl.h"
#include "../../include/dataengineimpl/shvdatastruct_impl.h"
#include "../../include/dataengineimpl/shvdatastructc_sqlite.h"


SHVDataRowListC_SQLite::SHVDataRowListC_SQLite(SHVSQLiteWrapper* sqlLite, const SHVStringC& sql)
{
SHVString rest;
	Statement = sqlLite->Prepare(Ok, sql, rest);
	Eof = !Ok;
}

SHVDataRowListC_SQLite::~SHVDataRowListC_SQLite()
{
}

const SHVDataRowC* SHVDataRowListC_SQLite::GetCurrentRow() const
{
	if (!Eof)
	{
		return CurrentRow.AsConst();
	}
	else
		return NULL;
}

SHVBool SHVDataRowListC_SQLite::NextRow()
{
	SHVASSERT(Ok);
	if (!Ok)
		return SHVBool::False;
	if (CurrentRow.IsNull())
		CurrentRow = new SHVDataRowC_SQLite(this);
	Eof = Statement->NextResult().GetError() != SHVSQLiteWrapper::SQLite_ROW;
	if (StructCache.IsNull() && !Eof)
	{
	SHVStringUTF8C colName(NULL);
	SHVStringUTF8C type(NULL);
	int shvtype;
	int len;
	SHVDataStruct_impl* st = new SHVDataStruct_impl();
		for (int col = 0; col < Statement->GetColumnCount(); col++)
		{
			Statement->GetColumnTypeUTF8(type, col);
			len = -1;
			shvtype = SHVDataVariant::SHVDataType_Undefined;
			if (type == __SQLITE_TYPE_INT)
				shvtype = SHVDataVariant::SHVDataType_Int;
			else
			if (type == __SQLITE_TYPE_DOUBLE)
				shvtype = SHVDataVariant::SHVDataType_Double;
			else
			if (type == __SQLITE_TYPE_BOOL)
				shvtype = SHVDataVariant::SHVDataType_Bool;
			else
			if (type == __SQLITE_TYPE_DATETIME)
				shvtype = SHVDataVariant::SHVDataType_Time;
			else
			if (type.Find(__SQLITE_TYPE_STRING) == 0)
			{
				shvtype = SHVDataVariant::SHVDataType_String;
				len = SHVString8C::StrToL(type.Mid(SHVString8C::StrLen(__SQLITE_TYPE_STRING) + 1).GetBufferConst(), NULL, 10);
			}
			st->Add(colName.ToStr8(), shvtype, len);
		}		
	}
	return !Eof;
}

SHVBool SHVDataRowListC_SQLite::Reset()
{
	SHVASSERT(Ok);
	if (!Ok)
		return SHVBool::False;
	Eof = SHVBool::False;

	return Statement->Reset();
}

const SHVDataStructC* SHVDataRowListC_SQLite::GetStruct() const
{
	return StructCache.AsConst();
}

const void* SHVDataRowListC_SQLite::GetProvider() const
{
	return Statement.AsConst();
}

SHVBool SHVDataRowListC_SQLite::IsOk() const
{
	return Ok;
}
