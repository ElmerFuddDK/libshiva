#include "stdafx.h"
#include "../../../../include/platformspc.h"
#include "../../../../include/utils/shvstringutf8.h"
#include "../../include/dataengineimpl/shvdatarowlistc_indexed.h"
#include "../../include/dataengineimpl/shvdatarowc_sqlite.h"
#include "../../include/dataengineimpl/shvdatarow_impl.h"
#include "../../include/dataengineimpl/shvdatastructc_sqlite.h"
#include "../../include/dataengineimpl/shvdatarowkey_impl.h"
#include "../../include/shvdataengine.h"


/*************************************
 * Constructor
 *************************************/
SHVDataRowListC_Indexed::SHVDataRowListC_Indexed(SHVDataSession* session, const SHVDataStructC* dataStruct, const SHVStringC& condition, size_t index): SHVDataRowListC_SQLite(session, dataStruct)
{
SHVStringSQLite rest(NULL);
SHVSQLiteWrapperRef SQLite = (SHVSQLiteWrapper*) session->GetProvider();
	SortIndex = index;
	TempPos = -1;
	SHVASSERT(GetStruct()->GetIndex(index));
	if (!GetStruct()->GetIndex(index))
	{
		Ok = SHVBool::False;
		return;
	}
SHVStringUTF8 query = BuildQuery(condition, false);
	if (Ok)
		Statement = SQLite->PrepareUTF8(Ok, query, rest);
	Eof = !Ok;
}

/*************************************
 * Reset
 *************************************/
SHVBool SHVDataRowListC_Indexed::Reset()
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
		Statement->SetParameterNullUTF8("idx");
		retVal = Statement->Reset();
		if (!retVal)
			Eof = SHVBool::True;
	}
	return retVal;
}

/*************************************
 * Destructor
 *************************************/
SHVDataRowListC_Indexed::~SHVDataRowListC_Indexed()
{
SHVSQLiteWrapperRef SQLite = (SHVSQLiteWrapper*) GetDataSession()->GetProvider();
SHVStringUTF8 sql;
SHVStringSQLite rest("");
SHVBool ok;
SHVSQLiteStatementRef statement;
	sql.Format("drop table memdb.%s", IndexTableName.GetSafeBuffer());
	Statement = NULL;
	statement = SQLite->ExecuteUTF8(ok, sql, rest);
}

/*************************************
 * GetRowCount
 *************************************/
int SHVDataRowListC_Indexed::GetRowCount() const
{
	return RowCount;
}

/*************************************
 * BuildQuery
 *************************************/
SHVStringBufferUTF8 SHVDataRowListC_Indexed::BuildQuery(const SHVStringC& condition, bool reverse)
{
SHVStringUTF8 queryUTF8;
SHVStringUTF8 col;
SHVStringUTF8 colDefs;
SHVStringUTF8 cols;
SHVStringUTF8 joinCond;
SHVStringUTF8 type;
SHVStringSQLite rest("");
SHVString8 condition8;
SHVString8 orderby8;
SHVSQLiteWrapperRef SQLite = (SHVSQLiteWrapper*) GetDataSession()->GetProvider();
const SHVDataStructC& Struct = *GetStruct();
const SHVDataRowKey& Key = *Struct.GetIndex(0);
bool first = true;
SHVSQLiteStatementRef statement;
long rc;

// Let the magic begin
	IndexTableName.Format("%s%d", Struct.GetTableName().GetSafeBuffer(), GetDataSession()->GetFactory()->GetDataEngine().GetModuleList().CreateTag());
	for (size_t i = 0; i < Key.Count(); i++)
	{
	size_t colIdx;
		if (Struct.FindColumnIndex(colIdx, Key[i].Key))
		{
			if (!first)
			{
				first = false;
				colDefs += ", ";
				cols += ", ";
				joinCond += " and ";
			}
			switch (Struct[i]->GetDataType())
			{
				case SHVDataVariant::TypeInt:
					type = __SQLITE_TYPE_INT;
					break;
				case SHVDataVariant::TypeBool:
					type = __SQLITE_TYPE_BOOL;
					break;
				case SHVDataVariant::TypeDouble:
					type = __SQLITE_TYPE_DOUBLE;
					break;
				case SHVDataVariant::TypeString:
					type.Format("%s(%d)", __SQLITE_TYPE_STRING, Struct[i]->GetDataLength());
					break;
				case SHVDataVariant::TypeTime:
					type = __SQLITE_TYPE_DATETIME;
			}
			col.Format("%s %s", Struct[i]->GetColumnName().GetSafeBuffer(), type.GetSafeBuffer());
			colDefs += col;
			col.Format("memdb.%s.%s = %s.%s", 
				IndexTableName.GetSafeBuffer(),
				Struct[i]->GetColumnName().GetSafeBuffer(),
				Struct.GetTableName().GetSafeBuffer(),
				Struct[i]->GetColumnName().GetSafeBuffer());
			joinCond += col;
			cols += Struct[i]->GetColumnName().GetSafeBuffer();
		}
		queryUTF8.Format("create table memdb.%s(idx integer primary key autoincrement, %s)", 
			IndexTableName.GetSafeBuffer(), 
			cols.GetSafeBuffer());
		statement = SQLite->ExecuteUTF8(Ok, queryUTF8, rest);
		if (Ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
			Ok = SHVBool::True;
		queryUTF8.Format("create index memdb.%s_index on %s(%s)",
			IndexTableName.GetSafeBuffer(),
			IndexTableName.GetSafeBuffer(),
			cols.GetSafeBuffer());
		statement = SQLite->ExecuteUTF8(Ok, queryUTF8, rest);
		if (Ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
			Ok = SHVBool::True;
		if (Ok)
		{
			GetDataSession()->GetFactory()->GetDataEngine().BuildKeySQL(Struct.GetIndex(SortIndex), condition8, orderby8, Struct.GetTableName().GetSafeBuffer(), reverse);
			queryUTF8.Format("insert into memdb.%s(%s) select %s from %s %s %s order by %s", 
				IndexTableName.GetSafeBuffer(), cols.GetSafeBuffer(), 
				cols.GetSafeBuffer(), 
				Struct.GetTableName(),
				(condition.IsNull() || condition == _T("") ? "" : "where"),
				condition.ToStrUTF8().GetSafeBuffer(),
				orderby8.GetSafeBuffer()
			);
			statement = SQLite->ExecuteUTF8(Ok, queryUTF8, rest);
			if (Ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
				Ok = SHVBool::True;
			queryUTF8.Format("select count(*) from memdb.%s", IndexTableName.GetSafeBuffer());
			statement = SQLite->ExecuteUTF8(Ok, queryUTF8, rest);
			if (Ok.GetError() == SHVSQLiteWrapper::SQLite_ROW)
				Ok = SHVBool::True;			
			statement->GetLong(rc, 0);
			RowCount = rc;
		}
		if (Ok)
		{
			queryUTF8.Format("select %s.*,idx from %s join memdb.%s on %s where %s and (@idx is null or idx >= @idx) order by idx",
				Struct.GetTableName().GetSafeBuffer(),
				Struct.GetTableName().GetSafeBuffer(),
				IndexTableName.GetSafeBuffer(),
				joinCond.GetSafeBuffer(),
				condition8.GetSafeBuffer());
		}		
	}
	return queryUTF8.ReleaseBuffer();	
}

/*************************************
 * TempReset
 *************************************/
SHVBool SHVDataRowListC_Indexed::TempReset()
{
	if (GetCurrentRow())
	{
	long p;
		if (Statement->GetLong(p, (int) GetStruct()->GetColumnCount()))
			TempPos = p;
	}
	else
		TempPos = -1;
	Reset();
	return SHVBool::True;
}

/*************************************
 * Reposition
 *************************************/
void SHVDataRowListC_Indexed::Reposition()
{
	if (TempPos >= 0)
	{
	SHVDataRowKeyRef Key = new SHVDataRowKey_impl();
		Key->AddKey("idx", new SHVDataVariant_impl(SHVInt(TempPos)), false);
		Find(Key);
		TempPos = -1;
	}
}

/*************************************
 * InternalRowChanged
 *************************************/
SHVBool SHVDataRowListC_Indexed::InternalRowChanged(SHVDataRow* row)
{
SHVBool retVal = SHVBool::True;
	if (row->GetRowState() == SHVDataRow::RowStateAdding)
	{
	SHVStringUTF8 val;
	SHVStringUTF8 vals;
	SHVStringUTF8 cols;
	SHVStringUTF8 sql;
	SHVStringSQLite rest("");
	SHVSQLiteWrapperRef SQLite = (SHVSQLiteWrapper*) GetDataSession()->GetProvider();
	SHVDataRowKeyRef key = row->GetKey(0);
	const SHVDataRowKey& Key = *key.AsConst();
		for (size_t i = 0; i < Key.Count(); i++)
		{
			if (i)
			{
				vals += ", ";
				cols += ", ";
			}
			if (Key[i].Value && !Key[i].Value->IsNull())
			{
				if (Key[i].Value->GetDataType() == SHVDataVariant::TypeString ||
					Key[i].Value->GetDataType() == SHVDataVariant::TypeTime)
					val.Format("'%s'", Key[i].Value->AsString().ToStrUTF8().GetSafeBuffer());
				else
					val = Key[i].Value->AsString().ToStrUTF8().GetSafeBuffer();
				vals += val;
				cols += Key[i].Key.GetSafeBuffer();
			}
		}
		sql.Format("insert into memdb.%s(%s) values(%s)", IndexTableName.GetSafeBuffer(), cols.GetSafeBuffer(), vals.GetSafeBuffer());
		SQLite->ExecuteUTF8(retVal, sql, rest);
		if (retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
			retVal = SHVBool::True;
	}
	else
	if (row->GetRowState() == SHVDataRow::RowStateDeleting)
	{
	SHVStringUTF8 sql;
	SHVSQLiteWrapperRef SQLite = (SHVSQLiteWrapper*) GetDataSession()->GetProvider();
	SHVStringUTF8 condition;
	SHVStringUTF8 subcondition;
	SHVStringSQLite rest("");
	bool first = true;
	const SHVDataStructC& Struct = *GetStruct();
	const SHVDataRowKey& Key = *GetStruct()->GetIndex(0);

		for (size_t i = 0; i < Key.Count(); i++)
		{
		size_t colIdx;
			if (Struct.FindColumnIndex(colIdx, Key[i].Key))
			{
			const SHVDataVariant* value = row->OriginalValue(colIdx);
				if (!value->IsNull())
				{
					if (!first)
					{
						condition += " and ";
					}
					else
						first = false;
					if (Struct[colIdx]->GetDataType() == SHVDataVariant::TypeString ||
						Struct[colIdx]->GetDataType() == SHVDataVariant::TypeTime)
						subcondition.Format("%s = '%s'", Key[i].Key.GetSafeBuffer(), value->AsString().ToStrUTF8().GetSafeBuffer());
					else
						subcondition.Format("%s = %s", Key[i].Key.GetSafeBuffer(), value->AsString().ToStrUTF8().GetSafeBuffer());
					condition += subcondition;
				}
			}
		}
		sql.Format("delete from memdb.%s where %s", IndexTableName.GetSafeBuffer(), condition.GetSafeBuffer());
		SQLite->ExecuteUTF8(retVal, sql, rest);
		if (retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
			retVal = SHVBool::True;
	}
	return retVal;
}
