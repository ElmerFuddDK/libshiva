#include "stdafx.h"
#include "../../../../include/platformspc.h"
#include "../../../../include/utils/shvstringutf8.h"
#include "../../include/dataengineimpl/shvdatastructc_sqlite.h"
#include "../../include/dataengineimpl/shvdatarowkey_impl.h"

/*************************************
 * Constructor
 *************************************/
SHVDataStructC_SQLite::SHVDataStructC_SQLite(SHVSQLiteWrapper* psqlite, const SHVString8C& tableName)
{
SHVStringUTF8 query;
SHVStringUTF8 notparsed;
SHVStringUTF8C valUTF8("");
SHVSQLiteStatementRef statement;
SHVSQLiteWrapperRef sqlite = psqlite;
int len;
SHVBool ok;
	Struct->SetTableName(tableName);

	query.Format("PRAGMA table_info(%s)", tableName.GetSafeBuffer());

	statement = sqlite->PrepareUTF8(ok, query, notparsed);
	SHVASSERT(ok);

	if (ok)
	{
		// Lets get the table structure
		while (statement->NextResult().GetError() == SHVSQLiteWrapper::SQLite_ROW)
		{
		SHVDataStructColumn_implRef col = new SHVDataStructColumn_impl();
		SHVString val;
		SHVStringUTF8C type("");

			if (statement->GetValueUTF8(valUTF8, len, 1))
				col->SetColumnName(SHVString8C(valUTF8.GetSafeBuffer()));
			if (statement->GetValueUTF8(valUTF8, len, 2))
			{
				col->SetDataLength(-1);
				if (SHVStringUTF8::StrCaseCmp(valUTF8.GetSafeBuffer(), __SQLITE_TYPE_INT))
					col->SetDataType(SHVDataVariant::SHVDataType_Int);
				if (SHVStringUTF8::StrCaseCmp(valUTF8.GetSafeBuffer(), __SQLITE_TYPE_DOUBLE))
					col->SetDataType(SHVDataVariant::SHVDataType_Double);
				if (SHVStringUTF8::StrCaseCmp(valUTF8.GetSafeBuffer(), __SQLITE_TYPE_DATETIME))
					col->SetDataType(SHVDataVariant::SHVDataType_Time);
				if (SHVStringUTF8::StrCaseCmp(valUTF8.GetSafeBuffer(), __SQLITE_TYPE_BOOL))
					col->SetDataType(SHVDataVariant::SHVDataType_Bool);
				if (SHVStringUTF8::StrCaseCmp(valUTF8.Left(SHVString8::StrLen(__SQLITE_TYPE_STRING)).GetSafeBuffer(), __SQLITE_TYPE_STRING))
				{
					col->SetDataType(SHVDataVariant::SHVDataType_Time);
					col->SetDataLength(SHVString8C::StrToL(valUTF8.GetSafeBuffer() + SHVString8::StrLen(__SQLITE_TYPE_STRING) + 1, NULL, 10));
				}				
			}
			if (statement->GetValueUTF8(valUTF8, len, 3))
				col->SetAllowNull(valUTF8 == "1");
			else
				col->SetAllowNull(SHVBool::True);
			if (statement->GetValueUTF8(valUTF8, len, 5))
				col->SetPrimaryKey(valUTF8 == "1");
			col->SetAutoInc(SHVBool::False);
			Struct->Add(col);
		}
		// now lets find the primary index
		query.Format("select name from sqlite_master where tbl_name='%s' and type='index'", tableName.GetSafeBuffer());
		statement = sqlite->PrepareUTF8(ok, query, notparsed);
		SHVASSERT(statement);
		if (ok)
		{
			while (statement->NextResult().GetError() == SHVSQLiteWrapper::SQLite_ROW)
			{
			SHVStringUTF8 name;
			SHVStringUTF8 sql;
				if (statement->GetValueUTF8(name, len, 0))
				{
				SHVSQLiteStatementRef indexInfo;
				SHVStringUTF8 nameDesc;
					nameDesc.Format(" %s desc");
					query.Format("PRAGMA index_info(%s)");
					indexInfo = sqlite->PrepareUTF8(ok, query, notparsed);
					SHVASSERT(indexInfo);
					if (ok)
					{
					SHVDataRowKey_implRef index = new SHVDataRowKey_impl();
						while (statement->NextResult().GetError() == SHVSQLiteWrapper::SQLite_ROW)
						{
							if (statement->GetValueUTF8(valUTF8, len, 2))
							{
								if (statement->GetValueUTF8(sql, len, 4) && sql.Find(nameDesc) >= 0)
									index->AddKey(valUTF8.ToStr8(), NULL, true);
								else
									index->AddKey(valUTF8.ToStr8(), NULL, false);
							}
						}
					}
				}
			}
		}
	}
}

SHVDataStructC_SQLite::~SHVDataStructC_SQLite()
{
}

/*************************************
 * GetTableName
 *************************************/
const SHVString8C& SHVDataStructC_SQLite::GetTableName() const
{
	return Struct->GetTableName();
}

/*************************************
 * operator[]
 *************************************/
const SHVDataStructColumnC* SHVDataStructC_SQLite::operator[](size_t idx) const
{
	return (*Struct.AsConst())[idx];
}

const SHVBool SHVDataStructC_SQLite::FindColumnIndex(size_t& index, const SHVString8C& colName) const
{
	return Struct->FindColumnIndex(index, colName);
}

/*************************************
 * GetColumnCount
 *************************************/
size_t SHVDataStructC_SQLite::GetColumnCount() const
{
	return Struct->GetColumnCount();
}

/*************************************
 * GetPrimaryIndex
 *************************************/
const SHVDataRowKey* SHVDataStructC_SQLite::GetPrimaryIndex() const
{
	return Struct->GetPrimaryIndex();
}

const SHVDataRowKey* SHVDataStructC_SQLite::GetIndex(size_t IdxID) const
{
	return Struct->GetIndex(IdxID);
}

const size_t SHVDataStructC_SQLite::IndexCount() const
{
	return Struct->IndexCount();
}

SHVBool SHVDataStructC_SQLite::IsEqual(const SHVDataStructC* dataStruct) const
{
	return Struct->IsEqual(dataStruct);
}
