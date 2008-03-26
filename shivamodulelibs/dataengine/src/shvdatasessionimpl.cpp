#include "stdafx.h"

#include "../../../../include/platformspc.h"
#include "../../../../include/utils/shvstringutf8.h"
#include "../../include/dataengineimpl/shvdatasession_sqlite.h"
#include "../../include/dataengineimpl/shvdatarowlist_sqlite.h"
#include "../../include/dataengineimpl/shvdatarowlistc_sqlite.h"
#include "../../include/dataengineimpl/shvdatarowlistc_indexed.h"
#include "../../include/shvdataengine.h"

/*************************************
 * Constructor
 *************************************/
SHVDataSession_SQLite::SHVDataSession_SQLite(SHVModuleList& modules, SHVSQLiteWrapper* sqlite, SHVDataFactory* factory): Modules(modules), Valid(true)
{
	SQLite = sqlite;
	Factory = factory;
}

/*************************************
 * StartEdit
 *************************************/
SHVBool SHVDataSession_SQLite::StartEdit()
{
SHVStringSQLite rest(NULL);
SHVBool ok;
SHVSQLiteStatementRef statement = SQLite->ExecuteUTF8(ok, "BEGIN TRANSACTION", rest);
Editting = SHVBool::True;
	return ok.GetError() == SHVSQLiteWrapper::SQLite_DONE;
}

/*************************************
 * Commit
 *************************************/
SHVBool SHVDataSession_SQLite::Commit()
{
SHVStringSQLite rest(NULL);
SHVBool ok;
SHVSQLiteStatementRef statement;
	ok = SessionReset();
	if (ok)		
		statement = SQLite->ExecuteUTF8(ok, "END TRANSACTION", rest);
	SessionReposition();
	if (ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
	{
		ok = SHVBool::True;
		Editting = SHVBool::False;
	}
	if (ok && !ChangeSubscriber.IsNull())
	{
	SHVDataRowVector* RowVector = new SHVDataRowVector();
		for (size_t i = 0;  i < ChangedRows.CalculateCount(); i++)
			RowVector->Add(ChangedRows[i]);
		ChangedRows.Clear();
		ChangeSubscriber->EmitNow(Modules, new SHVEventDataChangeSet(SHVDataRowVectorPtr(RowVector), NULL, EventChangeSet, SHVInt(), this));
	}
	return ok;
}

/*************************************
 * Rollback
 *************************************/
SHVBool SHVDataSession_SQLite::Rollback()
{
SHVStringSQLite rest(NULL);
SHVBool ok;
SHVSQLiteStatementRef statement = SQLite->ExecuteUTF8(ok, "ROLLBACK TRANSACTION", rest);
	Editting = SHVBool::False;
	return ok.GetError() == SHVSQLiteWrapper::SQLite_DONE;
}

/*************************************
 * GetRows
 *************************************/
SHVDataRowList* SHVDataSession_SQLite::GetRows(const SHVString8C& tableName, const SHVStringC& condition, size_t index)
{
SHVDataRowList* retVal = NULL;
const SHVDataStructC* st;
SHVDataRowListC* innerList;

	SHVASSERT(SessionValid());
	if (SessionValid())
	{
		st = Factory->FindStruct(tableName);	
		if (st)
		{
			innerList = new SHVDataRowListC_SQLite(this, st, tableName, condition, index);
			retVal = new SHVDataRowList_SQLite(this, innerList);
			RegisterDataList(retVal);
		}
	}
	return retVal;
}
/*************************************
 * GetRowsIndexed
 *************************************/
SHVDataRowList* SHVDataSession_SQLite::GetRowsIndexed(const SHVString8C& tableName, const SHVStringC& condition, size_t index)
{
SHVDataRowList* retVal = NULL;
const SHVDataStructC* st;
SHVDataRowListC* innerList;

	SHVASSERT(SessionValid());
	if (SessionValid())
	{
		st = Factory->FindStruct(tableName);	
		if (st)
		{
			innerList = new SHVDataRowListC_Indexed(this, st, tableName, condition, index);
			retVal = new SHVDataRowList_SQLite(this, innerList);
			RegisterDataList(retVal);
		}
	}
	return retVal;
}

/*************************************
 * Query
 *************************************/
SHVDataRowListC* SHVDataSession_SQLite::Query(const SHVStringC& query, const SHVDataRowKey* sortKey)
{
SHVDataRowListC* retVal = new SHVDataRowListC_SQLite(this, query, sortKey);
	RegisterDataList(retVal);
	return retVal;
}

/*************************************
 * QueryTable
 *************************************/
SHVDataRowListC* SHVDataSession_SQLite::QueryTable(const SHVString8C& tableName, const SHVStringC& condition, size_t index)
{
const SHVDataStructC* st; 
SHVDataRowListC* retVal;
	SHVASSERT(SessionValid());
	if (SessionValid())
	{
		st = Factory->FindStruct(tableName);	
		if (st)
		{
			retVal = new SHVDataRowListC_SQLite(this, st, tableName, condition, index);
			RegisterDataList(retVal);
		}
	}
	return retVal;
}

/*************************************
 * QueryTableIndexed
 *************************************/
SHVDataRowListC* SHVDataSession_SQLite::QueryTableIndexed(const SHVString8C& tableName, const SHVStringC& condition, size_t index)
{
const SHVDataStructC* st; 
SHVDataRowListC* retVal;
	SHVASSERT(SessionValid());
	if (SessionValid())
	{
		st = Factory->FindStruct(tableName);	
		if (st)
		{
			retVal = new SHVDataRowListC_Indexed(this, st, tableName, condition, index);
			RegisterDataList(retVal);
		}
	}
	return retVal;
}

/*************************************
 * ExecuteNonQuery
 *************************************/
SHVBool SHVDataSession_SQLite::ExecuteNonQuery(const SHVStringC& sql)
{
SHVBool ok;
SHVStringSQLite rest("");
SHVStringUTF8 sqlUTF8 = sql.ToStrUTF8();
SHVStringSQLite blank("");
SHVSQLiteStatementRef statement = SQLite->ExecuteUTF8(ok, sqlUTF8, rest);
	if (ok.GetError() == SHVSQLiteWrapper::SQLite_SCHEMA && SchemaChanged())
		statement = SQLite->ExecuteUTF8(ok, sqlUTF8, rest);
	if (ok.GetError() == SHVSQLiteWrapper::SQLite_ROW ||
		ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
	{
		while (!rest.IsNull() && rest != blank && 
			(ok.GetError() == SHVSQLiteWrapper::SQLite_ROW ||
			 ok.GetError() == SHVSQLiteWrapper::SQLite_DONE))
		{
			statement = SQLite->ExecuteUTF8(ok, sqlUTF8, rest);
		}
	}
	if (ok.GetError() == SHVSQLiteWrapper::SQLite_ROW ||
		ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
		return SHVBool::True;
	else
		return ok;
}

/*************************************
 * IsEditting
 *************************************/ 
SHVBool SHVDataSession_SQLite::IsEditting() const
{
	return Editting;
}

/*************************************
 * UpdateRow
 *************************************/
SHVBool SHVDataSession_SQLite::UpdateRow(SHVDataRow* row)
{
SHVBool retVal;
	switch (row->GetRowState())
	{
	case SHVDataRow::RowStateChanging:
		retVal = DoUpdateRow(row);
		break;
	case SHVDataRow::RowStateAdding:
		retVal = DoInsertRow(row);
		break;
	case SHVDataRow::RowStateDeleting:
		retVal = DoDeleteRow(row);
		break;
	default:
		retVal = SHVBool::False;
	}
	if (retVal && !ChangeSubscriber.IsNull())
	{
		ChangedRows.Add(row);
	}
	return retVal;
}

/*************************************
 * SubscribeDataChange
 *************************************/
void SHVDataSession_SQLite::SubscribeDataChange(SHVEventSubscriberBase* sub)
{
	ChangeSubscriber = sub;
}

/*************************************
 * GetProvider
 *************************************/
void* SHVDataSession_SQLite::GetProvider()
{
	return SQLite;
}

/*************************************
 * GetFactory
 *************************************/
SHVDataFactory* SHVDataSession_SQLite::GetFactory() const
{
	return (SHVDataFactory*) Factory;
}

/*************************************
 * Destructor
 *************************************/
SHVDataSession_SQLite::~SHVDataSession_SQLite()
{
	if (Editting)
		Rollback();
	UnregisterDataSession();
}

/*************************************
 * ClearOwnership
 *************************************/
void SHVDataSession_SQLite::ClearOwnership()
{
	Factory = NULL;
}

/*************************************
 * IsValid
 *************************************/
SHVBool SHVDataSession_SQLite::IsValid() const
{
	return Valid;
}

/*************************************
 * SchemaChanged
 *************************************/
bool SHVDataSession_SQLite::SchemaChanged()
{
	if (!HasPendingDataLists() && Factory)
	{
	SHVBool valid;
		SQLite = Factory->GetDataEngine().CreateConnection(valid, Factory->GetDatabase());
		Valid = valid;
	}
	else
		Valid = false;
	return Valid;
}

/*************************************
 * CheckSchema
 *************************************/
void SHVDataSession_SQLite::CheckSchema()
{
	if (!Valid)
		SchemaChanged();
}

/*************************************
 * EmptySlot
 *************************************/
bool SHVDataSession_SQLite::EmptySlot(const SHVDataRowVector& vector, size_t& idx)
{
bool found = false;
	for (idx = vector.CalculateCount(); idx && !found; idx--)
	{
		found = vector[idx] == NULL;	
	}
	return found;
}

/*************************************
 * Find
 *************************************/
SHVDataRow* SHVDataSession_SQLite::Find(const SHVDataRowVector& vector, const SHVDataRowKey* key, size_t& idx) const
{
SHVDataRow* found = NULL;
	for (idx = vector.CalculateCount(); idx && !found; idx--)
	{		
		if (vector[idx] && vector[idx]->MatchKey(key))
			found = (SHVDataRow*) vector[idx];
	}
	return found;
}

/*************************************
 * DoUpdateRow
 *************************************/
SHVBool SHVDataSession_SQLite::DoUpdateRow(SHVDataRow* row)
{
SHVBool retVal;
SHVStringUTF8 col;
SHVStringUTF8 cols;
SHVStringUTF8 sql;
SHVStringSQLite rest(NULL);
SHVSQLiteStatementRef statement;
const SHVDataStructC& st = *row->GetStruct();
	for (size_t c = 0; c  < st.GetColumnCount(); c++)
	{
		if (c)
			cols += ", ";
		if (st[c]->GetDataType() == SHVDataVariant::TypeString ||
			st[c]->GetDataType() == SHVDataVariant::TypeTime)
			col.Format("%s = '%s'", st[c]->GetColumnName().GetSafeBuffer(), row->AsString(c).ToStrUTF8().GetSafeBuffer());
		else
			col.Format("%s = %s", st[c]->GetColumnName().GetSafeBuffer(), row->AsString(c).ToStrUTF8().GetSafeBuffer());
		cols += col;
	}
	sql.Format("update or fail %s set %s where %s",
		row->GetAlias().GetSafeBuffer(),
		cols.GetSafeBuffer(),
		WhereSQL(row).GetSafeBuffer());
	statement = SQLite->ExecuteUTF8(retVal, sql, rest);
	if (retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
	{
		retVal = SHVBool::True;

	}
	return retVal;
}

/*************************************
 * DoInsertRow
 *************************************/
SHVBool SHVDataSession_SQLite::DoInsertRow(SHVDataRow* row)
{
SHVBool retVal;
SHVStringUTF8 val;
SHVStringUTF8 vals;
SHVStringUTF8 cols;
SHVStringUTF8 sql;
SHVStringSQLite rest(NULL);
SHVSQLiteStatementRef statement;
const SHVDataStructC& st = *row->GetStruct();
	for (size_t c = 0; c  < st.GetColumnCount(); c++)
	{
		if (c)
		{
			cols += ", ";
			vals += ", ";
		}
		if (st[c]->GetDataType() == SHVDataVariant::TypeString ||
			st[c]->GetDataType() == SHVDataVariant::TypeTime)
		{
			val.Format("'%s'", row->AsString(c).ToStrUTF8().GetSafeBuffer());
			vals += val;
		}
		else
			vals += row->AsString(c).ToStrUTF8().GetSafeBuffer();
		cols += st[c]->GetColumnName().GetSafeBuffer();
	}
	sql.Format("insert or fail into %s (%s) values(%s)",
		row->GetAlias().GetSafeBuffer(),
		cols.GetSafeBuffer(),
		vals.GetSafeBuffer());
	statement = SQLite->ExecuteUTF8(retVal, sql, rest);
	if (retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
	{
		retVal = SHVBool::True;

	}
	return retVal;
}

/*************************************
 * DoDeleteRow
 *************************************/
SHVBool SHVDataSession_SQLite::DoDeleteRow(SHVDataRow* row)
{
SHVBool retVal;
SHVStringUTF8 sql;
SHVStringSQLite rest(NULL);
SHVSQLiteStatementRef statement;
	sql.Format("delete from %s where %s", row->GetAlias().GetSafeBuffer(), WhereSQL(row).GetSafeBuffer());
	statement = SQLite->ExecuteUTF8(retVal, sql, rest);
	if (retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
	{
		retVal = SHVBool::True;

	}
	return retVal;
}

/*************************************
 * WhereSQL
 *************************************/
SHVStringUTF8 SHVDataSession_SQLite::WhereSQL(SHVDataRow* row)
{
SHVStringUTF8 retVal;
SHVStringUTF8 keycond;
SHVStringUTF8 rest;
const SHVDataRowKey& Key = *row->GetStruct()->GetPrimaryIndex();
SHVSQLiteStatementRef statement;
	for (size_t k = 0; k < Key.Count(); k++)
	{
	size_t colIdx;
		if (row->GetStruct()->FindColumnIndex(colIdx, Key[k].Key))
		{
		const SHVDataStructColumnC& col = *(*row->GetStruct())[colIdx];
			if (k)
				retVal += " and ";
			if (col.GetDataType() == SHVDataVariant::TypeString ||
				col.GetDataType() == SHVDataVariant::TypeTime)
				keycond.Format("%s = '%s'", Key[k].Key.GetSafeBuffer(), row->OriginalValue(Key[k].Key)->AsString().ToStrUTF8().GetSafeBuffer());
			else
				keycond.Format("%s = %s", Key[k].Key.GetSafeBuffer(), row->OriginalValue(Key[k].Key)->AsString().ToStrUTF8().GetSafeBuffer());
			retVal += keycond;
		}
	}
	return keycond;
}
