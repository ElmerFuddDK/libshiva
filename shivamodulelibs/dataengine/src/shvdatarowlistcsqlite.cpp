#include "stdafx.h"
#include "../../../../include/platformspc.h"
#include "../../../../include/utils/shvstringutf8.h"
#include "../../include/dataengineimpl/shvdatarowlistc_sqlite.h"
#include "../../include/dataengineimpl/shvdatarowc_sqlite.h"
#include "../../include/dataengineimpl/shvdatarow_impl.h"
#include "../../include/dataengineimpl/shvdatarowkey_impl.h"
#include "../../include/dataengineimpl/shvdatastruct_impl.h"
#include "../../include/dataengineimpl/shvdatastructc_sqlite.h"
#include "../../include/shvdataengine.h"
#include "../../../../shivasqlite/include/sqlitestatement.h"


/*************************************
 * Constructor
 *************************************/
SHVDataRowListC_SQLite::SHVDataRowListC_SQLite(SHVDataSession* session, const SHVDataStructC* dataStruct): DataSession(session), StructCache((SHVDataStructC*)dataStruct), RowCount(0)
{
}

SHVDataRowListC_SQLite::SHVDataRowListC_SQLite(SHVDataSession* session, const SHVStringC& sql, const SHVDataRowKey* sortKey): DataSession(session), RowCount(0)
{
SHVString rest;
SHVDataStruct_impl* st = new SHVDataStruct_impl();
SHVSQLiteWrapperRef SQLite = (SHVSQLiteWrapper*) session->GetProvider();
	StructCache = st;
	Statement = SQLite->Prepare(Ok, sql, rest);
	if (sortKey)
		st->AddIndex((SHVDataRowKey*) sortKey);
	SortIndex = 0;
	Eof = !Ok;
}

SHVDataRowListC_SQLite::SHVDataRowListC_SQLite(SHVDataSession* session, const SHVDataStructC* dataStruct, const SHVStringC& condition, size_t index): DataSession(session), StructCache((SHVDataStructC*)dataStruct), RowCount(0)
{
SHVStringSQLite rest(NULL);
SHVSQLiteWrapperRef SQLite = (SHVSQLiteWrapper*) session->GetProvider();

	SHVASSERT(dataStruct->GetIndex(index));
	if (!dataStruct->GetIndex(index))
	{
		Ok = SHVBool::False;
		return;
	}
	SortIndex = index;
	Statement = SQLite->PrepareUTF8(Ok, BuildQuery(condition, false), rest);
	Eof = !Ok;
}

SHVDataRowListC_SQLite::SHVDataRowListC_SQLite(SHVDataSession* session, SHVSQLiteStatement* statement, const SHVDataStructC* dataStruct, size_t index): DataSession(session), StructCache((SHVDataStructC*)dataStruct), RowCount(0)
{
	SHVASSERT(dataStruct->GetIndex(index));
	Ok = SHVBool::True;
	if (!dataStruct->GetIndex(index))
	{
		Ok = SHVBool::False;
		return;
	}
	SortIndex = index;
	Statement = statement;
	Eof = !Ok;
}

/*************************************
 * Destructor
 *************************************/
SHVDataRowListC_SQLite::~SHVDataRowListC_SQLite()
{
	UnregisterDataList();
	DataSession = NULL;
}

/*************************************
 * GetCurrentRow
 *************************************/
const SHVDataRowC* SHVDataRowListC_SQLite::GetCurrentRow() const
{
	SHVASSERT(IsOk());
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
SHVBool retVal = IsOk();
	SHVASSERT(retVal);
	if (retVal)
	{
		if (RowCount < 0)
		{
			RowCount = 0;
			while (NextRow()) RowCount++;
			Reset();
		}
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
		retVal = !Eof;
	}
	return retVal;
}

/*************************************
 * Reset
 *************************************/
SHVBool SHVDataRowListC_SQLite::Reset()
{
SHVBool retVal = IsOk();
	SHVASSERT(retVal);
	if (retVal)
	{
	const SHVDataRowKey& key = *GetStruct()->GetIndex(0);
		Eof = SHVBool::False;
		for (size_t i = 0; i < key.Count(); i++)
		{
			Statement->SetParameterNullUTF8(key[i].Key.GetSafeBuffer());
		}
		retVal = Statement->Reset();
		if (!retVal)
			Eof = SHVBool::True;
	}
	return retVal;
}


/*************************************
 * Find
 *************************************/
SHVDataRowC* SHVDataRowListC_SQLite::Find(const SHVDataRowKey* key)
{
const SHVDataRowKey& k = *key;
SHVDataRowC* retVal = NULL;
	SHVASSERT(IsOk());
	if (IsOk())
	{
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
			retVal = CurrentRow;
	}
	return retVal;
}

/*************************************
 * Reverse
 *************************************/
SHVDataRowListC* SHVDataRowListC_SQLite::Reverse(const SHVStringC& condition)
{
SHVBool ok;
SHVStringSQLite rest(NULL);
SHVSQLiteWrapperRef SQLite;
SHVSQLiteStatement* statement;
SHVDataRowListC* retVal = NULL;

	if (IsOk())
	{
		SQLite = (SHVSQLiteWrapper*) DataSession->GetProvider();
		statement = SQLite->PrepareUTF8(ok, BuildQuery(condition,true), rest);
		retVal = new SHVDataRowListC_SQLite(DataSession, statement, StructCache, SortIndex);
	}
	return retVal;
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
	return RowCount;
}

/*************************************
 * GetRowProvider
 *************************************/
const void* SHVDataRowListC_SQLite::GetRowProvider() const
{
	return Statement.AsConst();
}

/*************************************
 * GetDataSession
 *************************************/
SHVDataSession* SHVDataRowListC_SQLite::GetDataSession()
{
	return DataSession;
}
/*************************************
 * RowListValid
 *************************************/
SHVBool SHVDataRowListC_SQLite::RowListValid() const
{
///\todo Missing thread safety
	return !DataSession.IsNull() && DataSession->SessionValid();
}

/*************************************
 * IsOk
 *************************************/
SHVBool SHVDataRowListC_SQLite::IsOk() const
{
	return RowListValid() && Ok;
}

/*************************************
 * BuildQuery
 *************************************/
SHVStringBufferUTF8 SHVDataRowListC_SQLite::BuildQuery(const SHVStringC& condition, bool reverse)
{
SHVStringUTF8 queryUTF8;
SHVString8 condition8;
SHVString8 orderby8;

	GetDataSession()->GetFactory()->GetDataEngine().BuildKeySQL(StructCache->GetIndex(SortIndex), condition8, orderby8,"",reverse);
	if (!condition.IsNull() && condition != _T(""))
	{
		queryUTF8.Format("select * from %s where (%s) and %s order by %s", 
			StructCache->GetTableName().GetSafeBuffer(),
			condition.ToStrUTF8().GetSafeBuffer(),
			condition8.GetSafeBuffer(),
			orderby8.GetSafeBuffer());
	}
	else
	{
		queryUTF8.Format("select * from %s where %s order by %s", 
			StructCache->GetTableName().GetSafeBuffer(),
			condition8.GetSafeBuffer(),
			orderby8.GetSafeBuffer());
	}
	return queryUTF8.ReleaseBuffer();
}

/*************************************
 * AdjustRowCount
 *************************************/
void SHVDataRowListC_SQLite::AdjustRowCount(int delta)
{
	RowCount += delta;
}

/*************************************
 * TempReset
 *************************************/
SHVBool SHVDataRowListC_SQLite::TempReset()
{
	return Reset();
}

/*************************************
 * Reposition
 *************************************/
void SHVDataRowListC_SQLite::Reposition()
{
}

/*************************************
 * RowChanged
 *************************************/
SHVBool SHVDataRowListC_SQLite::InternalRowChanged(SHVDataRow* row)
{
	return SHVBool::True;
}

