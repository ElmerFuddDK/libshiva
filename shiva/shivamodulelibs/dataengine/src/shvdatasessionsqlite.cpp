/*
 *   Copyright (C) 2008 by Mogens Bak Nielsen, Lars Eriksen
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of the
 *   License, or (at your option) any later version with the following
 *   exeptions:
 *
 *   1) Static linking to the library does not constitute derivative work
 *      and does not require the author to provide source code for the
 *      application.
 *      Compiling applications with the source code directly linked in is
 *      Considered static linking as well.
 *
 *   2) You do not have to provide a copy of the license with programs
 *      that are linked against this code.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "stdafx.h"

#include "../../../include/platformspc.h"
#include "../../../include/threadutils/shvmutexlocker.h"
#include "../include/shvdatasessionsqlite.h"
#include "../include/shvdatarowlistsqlite.h"
#include "../include/shvdatarowlistcsqlite.h"
#include "../include/shvdatarowlistcindexed.h"
#include "../include/shvdatafunctionimpl.h"
#include "../../../include/modules/dataengine/shvdataengine.h"
#include "../../../include/utils/shvstringstream.h"
#include "../include/shvdatastatementimpl.h"

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
SHVDataRowListC* SHVDataSessionSQLite::QueryUTF8(const SHVStringUTF8C& query, const SHVDataRowKey* sortKey)
{
SHVDataRowListC* retVal = NULL;
	SHVASSERT(SessionValid());
	if (CheckConnection())
		retVal = new SHVDataRowListCSQLite(this, query, sortKey);
	return retVal;
}
/*************************************
 * PrepareFunction
 *************************************/
SHVDataFunction* SHVDataSessionSQLite::PrepareFunction(const SHVStringC &query, const SHVDataRowKey *sortKey)
{
	return new SHVDataFunctionImpl((SHVDataRowListCSQLite*) Query(query, sortKey));
}

/*************************************
 * GetDataChangeFunction
 *************************************/
SHVDataFunction *SHVDataSessionSQLite::GetDataChangeFunction(SHVDataRowList *rowList, int function)
{
SHVDataFunctionRef retVal = rowList->GetDataChangeFunction(function);
SHVStringStreamUTF8 sql;
	if (retVal == NULL)
	{
		switch (function)
		{
		case SHVDataRowList::ChangeFunctionAdd:
			InsertRowSQL(rowList, sql, rowList->GetReplaceIfDuplicate());
			retVal = PrepareFunction(sql);
			rowList->SetDataChangeFunction(retVal, SHVDataRowList::ChangeFunctionAdd);
			break;
		case SHVDataRowList::ChangeFunctionUpdate:
			UpdateRowSQL(rowList, sql);
			retVal = PrepareFunction(sql);
			rowList->SetDataChangeFunction(retVal, SHVDataRowList::ChangeFunctionUpdate);
			break;
		case SHVDataRowList::ChangeFunctionDelete:
			DeleteRowSQL(rowList, sql);
			retVal = PrepareFunction(sql);
			rowList->SetDataChangeFunction(retVal, SHVDataRowList::ChangeFunctionDelete);
			break;
		}
	}
	return retVal.ReleaseReference();
}

/*************************************
 * PrepareStatement
 *************************************/
SHVDataStatement* SHVDataSessionSQLite::PrepareStatementUTF8(const SHVStringUTF8C &query)
{
	return new SHVDataStatementImpl(SQLite,query);
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
		else
		{
			retVal = NULL;
		}
	}
	else
	{
		retVal = NULL;
	}
	return retVal;
}

/*************************************
 * QueryTableIndexed
 *************************************/
SHVDataRowListC* SHVDataSessionSQLite::QueryTableIndexed(const SHVString8C& tableName, const SHVStringC& condition, size_t index)
{
const SHVDataStructC* st; 
SHVDataRowListC* retVal = NULL;

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
 * QueryTableIndexed
 *************************************/
SHVDataRowList* SHVDataSessionSQLite::CopyAlias(const SHVString8C& sourceAlias, const SHVString8C& destAlias)
{
	if (StartEdit())
	{
	const SHVDataStructC* sourceStruct = Factory->FindStruct(sourceAlias);
	int sourceAliasID;
	int destAliasID;
	SHVBool retVal = SHVBool::True;
	SHVStringSQLite rest("");
	SHVSQLiteStatementRef statement;

		if (sourceStruct)
		{
			if (!Factory->FindStruct(destAlias))
			{
				retVal = Factory->RegisterAlias(sourceStruct->GetTableName(), destAlias, true, this);
			}
		}
		else
			retVal = SHVBool::False;
		if (!retVal)
		{
			Rollback();
			return NULL;
		}
		sourceAliasID = GetAliasID(sourceAlias);
		destAliasID = GetAliasID(destAlias);
		SQLite->ExecuteUTF8(retVal, SHVStringUTF8C::Format("delete from %s where shv_alias=%d", sourceStruct->GetTableName().GetSafeBuffer(), destAliasID), rest)->ValidateRefCount();
		if (retVal.GetError() != SHVSQLiteWrapper::SQLite_DONE)
		{
			Rollback();
			return NULL;
		}
		SQLite->ExecuteUTF8(retVal, SHVStringUTF8C::Format("insert into %s select %d, * from %s", sourceStruct->GetTableName().GetSafeBuffer(), destAliasID, sourceAlias.GetSafeBuffer()), rest)->ValidateRefCount();
		if (retVal.GetError() != SHVSQLiteWrapper::SQLite_DONE)
		{
			Rollback();
			return NULL;
		}
		Commit();
	}
	else
		return NULL;
	return this->GetRows(destAlias, _S(""), 0);
}

/*************************************
 * ExecuteNonQuery
 *************************************/
SHVBool SHVDataSessionSQLite::ExecuteNonQueryUTF8(const SHVStringUTF8C &sql)
{
SHVBool retVal;
SHVStringSQLite rest("");
SHVStringSQLite blank("");
SHVStringUTF8 sqlUTF8 = sql;
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
			SHVStringUTF8 oldQuery(sqlUTF8.ReleaseBuffer()); // Ensures the buffer doesn't get deleted before sqlUTF8 is refilled - rest points into query
				sqlUTF8 = rest;
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
 * GetRecentChanges
 *************************************/
int SHVDataSessionSQLite::GetRecentChanges()
{
	if (!SQLite.IsNull())
		return SQLite->GetRecentChanges();
	else
		return -1;
}

/*************************************
 * GetTotalChanges
 *************************************/
int SHVDataSessionSQLite::GetTotalChanges()
{
	if (!SQLite.IsNull())
		return SQLite->GetTotalChanges();
	else
		return -1;
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
void* SHVDataSessionSQLite::GetProvider(bool checkConnection)
{
	if (!checkConnection || CheckConnection())
		return SQLite;
	else
		return NULL;
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
 * UpdateRowSQL
 *************************************/
void SHVDataSessionSQLite::UpdateRowSQL(const SHVDataRowList *rowList, SHVStringStreamUTF8 &sql)
{
const SHVDataStructC& st = *rowList->GetStruct();

	sql.WriteStringUTF8("update or fail ");
	sql.WriteString8(st.GetTableName());
	for (size_t c = 0; c  < st.GetColumnCount(); c++)
	{
		if (c)
			sql.WriteStringUTF8(", \"");
		else
			sql.WriteStringUTF8(" set \"");
		sql.WriteStringUTF8(st[c]->GetColumnName().GetSafeBuffer());
		sql.WriteStringUTF8("\" = ");
		sql.WriteStringUTF8("@");
		sql.WriteStringUTF8(st[c]->GetColumnName().GetBufferConst());
	}
	sql.WriteStringUTF8(" where ");
	StreamWhereSQL(rowList,sql,NULL);
	sql.Finalize();
}

/*************************************
 * InitUpdateFunction
 *************************************/
void SHVDataSessionSQLite::InitUpdateFunction(SHVDataFunction *func, SHVDataRow *row)
{
	if (func)
	{
	const SHVDataStructC& st = *row->GetStruct();
		for (size_t c = 0; c < st.GetColumnCount(); c++)
		{
			if (row->IsNull(c))
			{
				func->SetParameterNullUTF8(st[c]->GetColumnName().GetBufferConst());
			}
			else
			{
				switch (st[c]->GetDataType())
				{
				case SHVDataVariant::TypeInt:
					func->SetParameterIntUTF8(st[c]->GetColumnName().GetBufferConst(), row->AsInt(c));
					break;
				case SHVDataVariant::TypeBool:
					func->SetParameterIntUTF8(st[c]->GetColumnName().GetBufferConst(), row->AsBool(c) ? 1 : 0);
					break;
				case SHVDataVariant::TypeInt64:
					func->SetParameterInt64UTF8(st[c]->GetColumnName().GetBufferConst(), row->AsInt64(c));
					break;
				case SHVDataVariant::TypeDouble:
					func->SetParameterDoubleUTF8(st[c]->GetColumnName().GetBufferConst(), row->AsDouble(c));
					 break;
				default:
					func->SetParameterStringUTF8(st[c]->GetColumnName().GetBufferConst(), row->AsString(c).ToStrUTF8());
					break;
				}
			}
		}
	}
}

/*************************************
 * InsertRowSQL
 *************************************/
void SHVDataSessionSQLite::InsertRowSQL(const SHVDataRowList *rowList, SHVStringStreamUTF8 &sql, bool replaceIfDuplicate)
{
const SHVDataStructC& st = *rowList->GetStruct();
bool empty;

	if (replaceIfDuplicate)
		sql.WriteStringUTF8("insert or replace into ");
	else
		sql.WriteStringUTF8("insert or fail into ");
	sql.WriteString8(st.GetTableName());
	
	sql.WriteStringUTF8(" (");
	if (st.GetIsMultiInstance())
	{
		empty = false;
		sql.WriteStringUTF8("shv_alias");
	}
	else
	{
		empty = true;
	}
	for (size_t c = 0; c  < st.GetColumnCount(); c++)
	{
		if (!empty)
			sql.WriteStringUTF8(", ");
		else
			empty = false;
		sql.WriteStringUTF8("\"");
		sql.WriteStringUTF8(st[c]->GetColumnName().GetSafeBuffer());
		sql.WriteStringUTF8("\"");
	}
	
	sql.WriteStringUTF8(") values(");
	if (st.GetIsMultiInstance())
	{
		empty = false;
		sql.WriteStringUTF8( SHVStringUTF8C::LongToString(rowList->GetAliasID()) );
	}
	else
	{
		empty = true;
	}
	for (size_t c = 0; c  < st.GetColumnCount(); c++)
	{
		if (!empty)
			sql.WriteStringUTF8(", ");
		else
			empty = false;
		sql.WriteStringUTF8("@");
		sql.WriteStringUTF8(st[c]->GetColumnName().GetBufferConst());
	}
	sql.WriteStringUTF8(")");
	sql.Finalize();
}

/*************************************
 * InitInsertFunction
 *************************************/
void SHVDataSessionSQLite::InitInsertFunction(SHVDataFunction *func, SHVDataRow *row)
{
	if (func)
	{
	const SHVDataStructC& st = *row->GetStruct();
		for (size_t c = 0; c < st.GetColumnCount(); c++)
		{
			if (row->IsNull(c))
			{
				func->SetParameterNullUTF8(st[c]->GetColumnName().GetBufferConst());
			}
			else
			{
				switch (st[c]->GetDataType())
				{
				case SHVDataVariant::TypeInt:
					func->SetParameterIntUTF8(st[c]->GetColumnName().GetBufferConst(), row->AsInt(c));
					break;
				case SHVDataVariant::TypeBool:
					func->SetParameterIntUTF8(st[c]->GetColumnName().GetBufferConst(), row->AsBool(c) ? 1 : 0);
					break;
				case SHVDataVariant::TypeInt64:
					func->SetParameterInt64UTF8(st[c]->GetColumnName().GetBufferConst(), row->AsInt64(c));
					break;
				case SHVDataVariant::TypeDouble:
					func->SetParameterDoubleUTF8(st[c]->GetColumnName().GetBufferConst(), row->AsDouble(c));
					 break;
				default:
					func->SetParameterStringUTF8(st[c]->GetColumnName().GetBufferConst(), row->AsString(c).ToStrUTF8());
					break;
				}
			}
		}
	}
}

/*************************************
 * DeleteRowSQL
 *************************************/
void SHVDataSessionSQLite::DeleteRowSQL(const SHVDataRowList *rowList, SHVStringStreamUTF8 &sql)
{
	sql.WriteStringUTF8("delete from ");
	sql.WriteString8(rowList->GetStruct()->GetTableName());
	sql.WriteStringUTF8(" where ");
	StreamWhereSQL(rowList,sql,NULL);
	sql.Finalize();
}

/*************************************
 * InitDeleteFunction
 *************************************/
void SHVDataSessionSQLite::InitDeleteFunction(SHVDataFunction *func, const SHVDataRow *row)
{
SHVDataRowKeyRef key(row->GetKey());
	InitDeleteFunction(func, key, row->GetStruct());
}

void SHVDataSessionSQLite::InitDeleteFunction(SHVDataFunction *func, const SHVDataRowKey *key, const SHVDataStructC *struc)
{
	if (func)
	{
	const SHVDataStructC& st = *struc;
	const SHVDataRowKey& Key = *key;
		for (size_t c = 0; c < Key.Count(); c++)
		{
		size_t colIdx;
			if (st.FindColumnIndex(colIdx, Key[c].Key))
			{
				if (!Key[c].Value)
				{
					func->SetParameterNullUTF8(st[colIdx]->GetColumnName().GetBufferConst());
				}
				else
				{
					switch (st[colIdx]->GetDataType())
					{
					case SHVDataVariant::TypeInt:
						func->SetParameterIntUTF8(st[colIdx]->GetColumnName().GetBufferConst(), Key[c].Value->AsInt());
						break;
					case SHVDataVariant::TypeBool:
						func->SetParameterIntUTF8(st[colIdx]->GetColumnName().GetBufferConst(), Key[c].Value->AsBool() ? 1 : 0);
						break;
					case SHVDataVariant::TypeInt64:
						func->SetParameterInt64UTF8(st[colIdx]->GetColumnName().GetBufferConst(), Key[c].Value->AsInt64());
						break;
					case SHVDataVariant::TypeDouble:
						func->SetParameterDoubleUTF8(st[colIdx]->GetColumnName().GetBufferConst(), Key[c].Value->AsDouble());
						 break;
					default:
						func->SetParameterStringUTF8(st[colIdx]->GetColumnName().GetBufferConst(), Key[c].Value->AsString().ToStrUTF8());
						break;
					}
				}
			}
		}
	}
}

/*************************************
 * WhereSQL
 *************************************/
SHVStringUTF8 SHVDataSessionSQLite::WhereSQL(SHVDataRow* row)
{
SHVStringStreamUTF8 retVal(128);
	StreamWhereSQL(row->GetRowList(),retVal,row);
	retVal.Finalize();
	return retVal;
}

/*************************************
 * StreamWhereSQL
 *************************************/
void SHVDataSessionSQLite::StreamWhereSQL(const SHVDataRowList *rowList, SHVStringStreamUTF8 &str, SHVDataRow *row)
{
const SHVDataRowKey& Key = *rowList->GetStruct()->GetPrimaryIndex();
bool empty = true;
	
	if (rowList->GetStruct()->GetIsMultiInstance())
	{
		str.WriteStringUTF8("shv_alias=");
		str.WriteStringUTF8(SHVStringUTF8C::LongToString(rowList->GetAliasID()));
		empty = false;
	}
	for (size_t k = 0; k < Key.Count(); k++)
	{
	size_t colIdx;
		if (rowList->GetStruct()->FindColumnIndex(colIdx, Key[k].Key))
		{
			if (empty)
			{
				str.WriteStringUTF8("\"");
				empty = false;
			}
			else
			{
				str.WriteStringUTF8(" and \"");
			}
			str.WriteStringUTF8(Key[k].Key.GetSafeBuffer());
			str.WriteStringUTF8("\" = ");
			if (!row)
			{
				str.WriteStringUTF8("@");
				str.WriteStringUTF8(Key[k].Key.GetSafeBuffer());
			}
			else
			{
				str.WriteString(row->OriginalValue(Key[k].Key)->AsDBString());
			}
		}
	}
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
SHVDataFunctionRef Func;
SHVDataRowListC* Result;

	SHVASSERT(SessionValid());
	switch (row->GetRowState())
	{
	case SHVDataRow::RowStateChanging:		
		Func = GetDataChangeFunction(row->GetRowList(), SHVDataRowList::ChangeFunctionUpdate);
		InitUpdateFunction(Func, row);
		break;
	case SHVDataRow::RowStateAdding:
		Func = GetDataChangeFunction(row->GetRowList(), SHVDataRowList::ChangeFunctionAdd);
		InitInsertFunction(Func, row);
		break;
	case SHVDataRow::RowStateDeleting:
		Func = GetDataChangeFunction(row->GetRowList(), SHVDataRowList::ChangeFunctionDelete);
		InitDeleteFunction(Func, row);
		break;
	default:
		retVal = SHVBool::False;
	}
	if (!Func.IsNull())
	{
		Result = Func->Exec();
		retVal = Func->GetLastError();
		if (retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
			retVal = SHVBool::True;
		if (Result)
			Result->Reset();
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

/*************************************
 * PrepareFunction
 *************************************/
SHVDataFunction* SHVDataSessionSQLite::PrepareFunction(const SHVStringUTF8C& query, const SHVDataRowKey* sortKey)
{
SHVDataFunction* retVal = NULL;
	SHVASSERT(SessionValid());
	if (CheckConnection())
		retVal = new SHVDataFunctionImpl(this, query, sortKey);
	return retVal;
}
