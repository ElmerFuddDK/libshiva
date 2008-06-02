#include "stdafx.h"

#include "../../../include/platformspc.h"
#include "../../../include/utils/shvstringutf8.h"
#include "../../../include/threadutils/shvmutexlocker.h"
#include "../include/shvdatasessionsqlite.h"
#include "../include/shvdatarowlistsqlite.h"
#include "../include/shvdatarowlistcsqlite.h"
#include "../include/shvdatarowlistcindexed.h"
#include "../../../include/modules/dataengine/shvdataengine.h"

/*************************************
 * Constructor
 *************************************/
SHVDataSessionSQLite::SHVDataSessionSQLite(SHVModuleList& modules, SHVSQLiteWrapper* sqlite, SHVDataFactory* factory): Modules(modules), SchemaValid(true)
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

	if (Factory) 
	{
		Factory->LockExclusive();
		ok = InternalBeginTransaction();
		if (!ok)
			Factory->Unlock();
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
	if (!IsEditting())
		return SHVBool(SHVSQLiteWrapper::SQLite_MISUSE);
	if (Factory)
	{
		ok = InternalEndTransaction();
		Factory->Unlock();
		if (ok && !ChangeSubscriber.IsNull())
		{
		SHVDataRowVector* RowVector = new SHVDataRowVector();
			for (size_t i = 0;  i < ChangedRows.CalculateCount(); i++)
				RowVector->Add(ChangedRows[i]);
			ChangedRows.Clear();
			ChangeSubscriber->EmitNow(Modules, new SHVEventDataChangeSet(SHVDataRowVectorPtr(RowVector), NULL, EventChangeSet, SHVInt(), this));
		}
	}
	return ok;
}

/*************************************
 * Rollback
 *************************************/
SHVBool SHVDataSessionSQLite::Rollback()
{
SHVBool ok;
	if (!IsEditting() || !Factory)
	{
		ok = SHVBool(SHVSQLiteWrapper::SQLite_MISUSE);
	}
	else
	{
		ok = InternalRollbackTransaction();
		Factory->Unlock();
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
	if (CheckConnection())
	{
		st = Factory->FindStruct(tableName);	
		if (st)
		{
			innerList = new SHVDataRowListCSQLite(this, st, tableName, condition, index);
			retVal = new SHVDataRowListSQLite(this, innerList);
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
	if (CheckConnection())
	{
		st = Factory->FindStruct(tableName);	
		if (st)
		{
			innerList = new SHVDataRowListCIndexed(this, st, tableName, condition, index);
			retVal = new SHVDataRowListSQLite(this, innerList);
		}
	}
	return retVal;
}

/*************************************
 * Query
 *************************************/
SHVDataRowListC* SHVDataSessionSQLite::Query(const SHVStringC& query, const SHVDataRowKey* sortKey)
{
SHVDataRowListC* retVal = NULL;
	SHVASSERT(SessionValid());
	if (CheckConnection())
		retVal = new SHVDataRowListCSQLite(this, query, sortKey);
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
	if (CheckConnection())
	{
		st = Factory->FindStruct(tableName);	
		if (st)
		{
			retVal = new SHVDataRowListCSQLite(this, st, tableName, condition, index);
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
	if (CheckConnection())
	{
		st = Factory->FindStruct(tableName);	
		if (st)
		{
			retVal = new SHVDataRowListCIndexed(this, st, tableName, condition, index);
		}
	}
	return retVal;
}

/*************************************
 * ExecuteNonQuery
 *************************************/
SHVBool SHVDataSessionSQLite::ExecuteNonQuery(const SHVStringC& sql)
{
SHVBool retVal;
SHVStringSQLite rest("");
SHVStringUTF8 sqlUTF8 = sql.ToStrUTF8();
SHVStringSQLite blank("");
SHVSQLiteStatementRef statement;

	SHVASSERT(SessionValid());
	retVal = CheckConnection();
	if (retVal)
	{
		statement = SQLite->ExecuteUTF8(retVal, sqlUTF8, rest);
		if (retVal.GetError() == SHVSQLiteWrapper::SQLite_ROW ||
			retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
		{
			while (!rest.IsNull() && rest != blank && 
				(retVal.GetError() == SHVSQLiteWrapper::SQLite_ROW ||
				 retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE))
			{
				statement = SQLite->ExecuteUTF8(retVal, sqlUTF8, rest);
			}
		}
		if (retVal.GetError() == SHVSQLiteWrapper::SQLite_ROW ||
			retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
			retVal = SHVBool::True;
	}
	return retVal;
}

/*************************************
 * SubscribeDataChange
 *************************************/
void SHVDataSessionSQLite::SubscribeDataChange(SHVEventSubscriberBase* sub)
{
	ChangeSubscriber = sub;
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
	return (SHVDataFactory*) Factory.AsConst();
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
	UnregisterDataSession();
}

/*************************************
 * DoUpdateRow
 *************************************/
SHVBool SHVDataSessionSQLite::DoUpdateRow(SHVDataRow* row)
{
SHVBool retVal;
SHVStringUTF8 cols;
SHVStringUTF8 sql;
SHVStringSQLite rest(NULL);
SHVSQLiteStatementRef statement;
const SHVDataStructC& st = *row->GetStruct();

	for (size_t c = 0; c  < st.GetColumnCount(); c++)
	{
		if (c)
			cols += ", ";
		cols += SHVStringUTF8C::Format("%s = %s", st[c]->GetColumnName().GetSafeBuffer(), row->AsDBString(c).ToStrUTF8().GetSafeBuffer());
	}
	sql.Format("update or fail %s set %s where %s",
		row->GetStruct()->GetTableName().GetSafeBuffer(),
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

	if (st.GetIsMultiInstance())
	{
		cols = "shv_alias";
		vals.Format("%d", row->GetRowListC()->GetAliasID());
	}
	for (size_t c = 0; c  < st.GetColumnCount(); c++)
	{
		if (!cols.IsEmpty())
		{
			cols += ", ";
			vals += ", ";
		}
		cols += st[c]->GetColumnName().GetSafeBuffer();
		vals += row->AsDBString(c).ToStrUTF8();
	}
	sql.Format("insert or fail into %s (%s) values(%s)",
		row->GetStruct()->GetTableName().GetSafeBuffer(),
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

	sql.Format("delete from %s where %s", row->GetStruct()->GetTableName().GetSafeBuffer(), WhereSQL(row).GetSafeBuffer());
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
	
	if (row->GetStruct()->GetIsMultiInstance())
		retVal = SHVStringUTF8C::Format("shv_alias=%d", row->GetRowListC()->GetAliasID());
	for (size_t k = 0; k < Key.Count(); k++)
	{
	size_t colIdx;
		if (row->GetStruct()->FindColumnIndex(colIdx, Key[k].Key))
		{
		const SHVDataStructColumnC& col = *(*row->GetStruct())[colIdx];
			if (!retVal.IsEmpty())
				retVal += " and ";
			keycond.Format("%s = %s", Key[k].Key.GetSafeBuffer(), row->OriginalValue(Key[k].Key)->AsDBString().ToStrUTF8().GetSafeBuffer());
			retVal += keycond;
		}
	}
	return retVal;
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
 * UpdateRow
 *************************************/
SHVBool SHVDataSessionSQLite::UpdateRow(SHVDataRow* row)
{
SHVBool retVal;

	SHVASSERT(SessionValid());
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
	return true;
}

/*************************************
 * SchemaChanged
 *************************************/
void SHVDataSessionSQLite::SchemaChanged()
{
SHVMutexLocker lock(Lock);
	SchemaValid = false;
}

/*************************************
 * CheckConnection
 *************************************/
SHVBool SHVDataSessionSQLite::CheckConnection()
{
SHVMutexLocker lock(Lock);
SHVBool retVal = SHVBool::True;
	if (!SchemaValid && Factory)
	{
		SQLite = Factory->GetDataEngine().CreateConnection(retVal, Factory->GetDatabase());
		if (retVal)
			SchemaValid = true;
	}
	return retVal;
}

