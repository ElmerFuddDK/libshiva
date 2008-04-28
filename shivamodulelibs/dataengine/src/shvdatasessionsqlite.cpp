#include "stdafx.h"

#include "../../../include/platformspc.h"
#include "../../../include/utils/shvstringutf8.h"
#include "../include/shvdatasessionsqlite.h"
#include "../include/shvdatarowlistsqlite.h"
#include "../include/shvdatarowlistcsqlite.h"
#include "../include/shvdatarowlistcindexed.h"
#include "../../../include/modules/dataengine/shvdataengine.h"

/*************************************
 * Constructor
 *************************************/
SHVDataSessionSQLite::SHVDataSessionSQLite(SHVModuleList& modules, SHVSQLiteWrapper* sqlite, SHVDataFactory* factory): Modules(modules), Valid(true)
{
	SQLite = sqlite;
	Factory = factory;
}

/*************************************
 * StartEdit
 *************************************/
SHVBool SHVDataSessionSQLite::StartEdit()
{
SHVBool ok;
	if (LockTransaction())
	{
		ok = InternalBeginTransaction();
		if (ok == SHVSQLiteWrapper::SQLite_DONE)
		{
			Editting = SHVBool::True;
			ok = SHVBool::True;
		}
		else
			UnlockTransaction();
	}
	else
		ok.SetError(SHVSQLiteWrapper::SQLite_LOCKED);
	return ok;
}

/*************************************
 * Commit
 *************************************/
SHVBool SHVDataSessionSQLite::Commit()
{
SHVBool ok;
	if (!Editting)
		return SHVBool(SHVSQLiteWrapper::SQLite_MISUSE);
	ok = SessionReset();
	if (ok)
		ok = InternalEndTransaction();
	SessionReposition();
	if (ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
	{
		ok = SHVBool::True;
		Editting = SHVBool::False;
	}
	UnlockTransaction();
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
SHVBool SHVDataSessionSQLite::Rollback()
{
SHVBool ok;
	if (!Editting)
	{
		ok = SHVBool(SHVSQLiteWrapper::SQLite_MISUSE);
	}
	else
	{
		SessionReset();
		ok = InternalRollbackTransaction();
		SessionReposition();
		Editting = SHVBool::False;
		UnlockTransaction();
		if (ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
			ok = SHVBool::True;
	}
	return ok;
}

/*************************************
 * GetRows
 *************************************/
SHVDataRowList* SHVDataSessionSQLite::GetRows(const SHVString8C& tableName, const SHVStringC& condition, size_t index)
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
			innerList = new SHVDataRowListCSQLite(this, st, tableName, condition, index);
			retVal = new SHVDataRowListSQLite(this, innerList);
			RegisterDataList(retVal);
		}
	}
	return retVal;
}
/*************************************
 * GetRowsIndexed
 *************************************/
SHVDataRowList* SHVDataSessionSQLite::GetRowsIndexed(const SHVString8C& tableName, const SHVStringC& condition, size_t index)
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
			innerList = new SHVDataRowListCIndexed(this, st, tableName, condition, index);
			retVal = new SHVDataRowListSQLite(this, innerList);
			RegisterDataList(retVal);
		}
	}
	return retVal;
}

/*************************************
 * Query
 *************************************/
SHVDataRowListC* SHVDataSessionSQLite::Query(const SHVStringC& query, const SHVDataRowKey* sortKey)
{
SHVDataRowListC* retVal = new SHVDataRowListCSQLite(this, query, sortKey);
	RegisterDataList(retVal);
	return retVal;
}

/*************************************
 * QueryTable
 *************************************/
SHVDataRowListC* SHVDataSessionSQLite::QueryTable(const SHVString8C& tableName, const SHVStringC& condition, size_t index)
{
const SHVDataStructC* st; 
SHVDataRowListC* retVal;
	SHVASSERT(SessionValid());
	if (SessionValid())
	{
		st = Factory->FindStruct(tableName);	
		if (st)
		{
			retVal = new SHVDataRowListCSQLite(this, st, tableName, condition, index);
			RegisterDataList(retVal);
		}
	}
	return retVal;
}

/*************************************
 * QueryTableIndexed
 *************************************/
SHVDataRowListC* SHVDataSessionSQLite::QueryTableIndexed(const SHVString8C& tableName, const SHVStringC& condition, size_t index)
{
const SHVDataStructC* st; 
SHVDataRowListC* retVal;
	SHVASSERT(SessionValid());
	if (SessionValid())
	{
		st = Factory->FindStruct(tableName);	
		if (st)
		{
			retVal = new SHVDataRowListCIndexed(this, st, tableName, condition, index);
			RegisterDataList(retVal);
		}
	}
	return retVal;
}

/*************************************
 * ExecuteNonQuery
 *************************************/
SHVBool SHVDataSessionSQLite::ExecuteNonQuery(const SHVStringC& sql)
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
SHVBool SHVDataSessionSQLite::IsEditting() const
{
	return Editting;
}

/*************************************
 * SubscribeDataChange
 *************************************/
void SHVDataSessionSQLite::SubscribeDataChange(SHVEventSubscriberBase* sub)
{
	ChangeSubscriber = sub;
}

/*************************************
 * AliasActive
 *************************************/
bool SHVDataSessionSQLite::AliasActive(const SHVString8C& alias)
{
SHVListIterator<SHVDataRowListC*> Iter(ActiveDataLists);
bool retVal = false;
	while (Iter.MoveNext() && !retVal)
		retVal = Iter.Get()->GetAlias() == alias;
	return retVal;
}

/*************************************
 * GetProvider
 *************************************/
void* SHVDataSessionSQLite::GetProvider()
{
	return SQLite;
}

/*************************************
 * GetFactory
 *************************************/
SHVDataFactory* SHVDataSessionSQLite::GetFactory() const
{
	return (SHVDataFactory*) Factory;
}

/*************************************
 * GetErrorMessage
 *************************************/
SHVStringBuffer SHVDataSessionSQLite::GetErrorMessage() const
{
	return SQLite->GetErrorMsg();
}


/*************************************
 * Destructor
 *************************************/
SHVDataSessionSQLite::~SHVDataSessionSQLite()
{
	if (Editting)
		Rollback();
	UnregisterDataSession();
}

/*************************************
 * DoUpdateRow
 *************************************/
SHVBool SHVDataSessionSQLite::DoUpdateRow(SHVDataRow* row)
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
		col.Format("%s = %s", st[c]->GetColumnName().GetSafeBuffer(), row->AsDBString(c).ToStrUTF8().GetSafeBuffer());
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
SHVBool SHVDataSessionSQLite::DoInsertRow(SHVDataRow* row)
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
		cols += st[c]->GetColumnName().GetSafeBuffer();
		vals += row->AsDBString(c).ToStrUTF8();
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
	else
	{
		SHVTRACE(_T("Error in %s: %s\n"), sql.ToStrT().GetSafeBuffer(), GetErrorMessage().GetSafeBuffer());
	}
	return retVal;
}

/*************************************
 * DoDeleteRow
 *************************************/
SHVBool SHVDataSessionSQLite::DoDeleteRow(SHVDataRow* row)
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
SHVStringUTF8 SHVDataSessionSQLite::WhereSQL(SHVDataRow* row)
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
			keycond.Format("%s = %s", Key[k].Key.GetSafeBuffer(), row->OriginalValue(Key[k].Key)->AsDBString().ToStrUTF8().GetSafeBuffer());
			retVal += keycond;
		}
	}
	return keycond;
}

/*************************************
 * CheckSchema
 *************************************/
void SHVDataSessionSQLite::CheckSchema()
{
	if (!Valid)
		SchemaChanged();
}

/*************************************
 * EmptySlot
 *************************************/
bool SHVDataSessionSQLite::EmptySlot(const SHVDataRowVector& vector, size_t& idx)
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
SHVDataRow* SHVDataSessionSQLite::Find(const SHVDataRowVector& vector, const SHVDataRowKey* key, size_t& idx) const
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
 * ClearOwnership
 *************************************/
void SHVDataSessionSQLite::ClearOwnership()
{
	Factory = NULL;
}

/*************************************
 * UpdateRow
 *************************************/
SHVBool SHVDataSessionSQLite::UpdateRow(SHVDataRow* row)
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
 * IsValid
 *************************************/
SHVBool SHVDataSessionSQLite::IsValid() const
{
	return Valid;
}

/*************************************
 * SchemaChanged
 *************************************/
bool SHVDataSessionSQLite::SchemaChanged()
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
 * RegisterDataList
 *************************************/
void SHVDataSessionSQLite::RegisterDataList(SHVDataRowListC* rowList)
{
	ActiveDataLists.AddTail(rowList);
}

/*************************************
 * UnregisterDataList
 *************************************/
void SHVDataSessionSQLite::UnregisterDataList(SHVDataRowListC* rowList)
{
SHVListPos pos = ActiveDataLists.Find(rowList);
	if (pos)
	{
		CheckAlias(rowList->GetAlias());
		ActiveDataLists.RemoveAt(pos);
	}
}

/*************************************
 * SessionReset
 *************************************/
SHVBool SHVDataSessionSQLite::SessionReset()
{
SHVBool retVal = SHVBool::True;
SHVListIterator<SHVDataRowListC*> iter(ActiveDataLists);
	while (retVal && iter.MoveNext())
	{
		retVal =  DataListTempReset(iter.Get());
	}
	return retVal;
}

/*************************************
 * SessionReposition
 *************************************/
void SHVDataSessionSQLite::SessionReposition()
{
SHVListIterator<SHVDataRowListC*> iter(ActiveDataLists);
	while (iter.MoveNext())
	{
		DataListReposition(iter.Get());
	}
}
