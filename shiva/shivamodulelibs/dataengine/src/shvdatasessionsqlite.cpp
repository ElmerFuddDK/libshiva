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
#include "../../../include/utils/shvstringutf8.h"
#include "../../../include/threadutils/shvmutexlocker.h"
#include "../include/shvdatasessionsqlite.h"
#include "../include/shvdatarowlistsqlite.h"
#include "../include/shvdatarowlistcsqlite.h"
#include "../include/shvdatarowlistcindexed.h"
#include "../include/shvdatafunctionimpl.h"
#include "../../../include/modules/dataengine/shvdataengine.h"
#include "../../../include/utils/shvstringstream.h"

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
 * PrepareFunction
 *************************************/
SHVDataFunction* SHVDataSessionSQLite::PrepareFunction(const SHVStringC &query, const SHVDataRowKey *sortKey)
{
	return new SHVDataFunctionImpl((SHVDataRowListCSQLite*) Query(query, sortKey));
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
void SHVDataSessionSQLite::UpdateRowSQL(SHVDataRow *row, SHVStringStreamUTF8 &sql)
{
const SHVDataStructC& st = *row->GetStruct();

	sql.WriteStringUTF8("update or fail ");
	sql.WriteString8(row->GetStruct()->GetTableName());
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
	StreamWhereSQL(row,sql,true);
	sql.Finalize();
}

/*************************************
 * DoInsertRow
 *************************************/
void SHVDataSessionSQLite::InsertRowSQL(SHVDataRow *row, SHVStringStreamUTF8 &sql, bool replaceIfDuplicate)
{
const SHVDataStructC& st = *row->GetStruct();
bool empty;

	if (replaceIfDuplicate)
		sql.WriteStringUTF8("insert or replace into ");
	else
		sql.WriteStringUTF8("insert or fail into ");
	sql.WriteString8(row->GetStruct()->GetTableName());
	
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
		sql.WriteStringUTF8( SHVStringUTF8C::LongToString(row->GetRowListC()->GetAliasID()) );
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
 * DoDeleteRow
 *************************************/
void SHVDataSessionSQLite::DeleteRowSQL(SHVDataRow *row, SHVStringStreamUTF8 &sql)
{
	sql.WriteStringUTF8("delete from ");
	sql.WriteString8(row->GetStruct()->GetTableName());
	sql.WriteStringUTF8(" where ");
	StreamWhereSQL(row,sql,true);
	sql.Finalize();
}

/*************************************
 * WhereSQL
 *************************************/
SHVStringUTF8 SHVDataSessionSQLite::WhereSQL(SHVDataRow* row)
{
SHVStringStreamUTF8 retVal(128);
	StreamWhereSQL(row,retVal,false);
	retVal.Finalize();
	return retVal;
}

/*************************************
 * StreamWhereSQL
 *************************************/
void SHVDataSessionSQLite::StreamWhereSQL(SHVDataRow* row, SHVStringStreamUTF8& str, bool parameterized)
{
const SHVDataRowKey& Key = *row->GetStruct()->GetPrimaryIndex();
bool empty = true;
	
	if (row->GetStruct()->GetIsMultiInstance())
	{
		str.WriteStringUTF8("shv_alias=");
		str.WriteStringUTF8(SHVStringUTF8C::LongToString(row->GetRowListC()->GetAliasID()));
		empty = false;
	}
	for (size_t k = 0; k < Key.Count(); k++)
	{
	size_t colIdx;
		if (row->GetStruct()->FindColumnIndex(colIdx, Key[k].Key))
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
			if (parameterized)
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
SHVBool SHVDataSessionSQLite::UpdateRow(SHVDataRow* row, bool replaceIfDuplicate)
{
SHVBool retVal;
SHVDataFunctionRef Func;
SHVStringStreamUTF8 sql;
const SHVDataStructC& st = *row->GetStruct();
const SHVDataRowKey& Key = *row->GetStruct()->GetPrimaryIndex();
SHVDataRowListC* Result;

	SHVASSERT(SessionValid());
	switch (row->GetRowState())
	{
	case SHVDataRow::RowStateChanging:
		Func = row->GetRowList()->GetDataChangeFunction(SHVDataRowList::ChangeFunctionUpdate);
		if (Func.IsNull())
		{
			UpdateRowSQL(row, sql);
			Func = PrepareFunction(sql);
			row->GetRowList()->SetDataChangeFunction(Func, SHVDataRowList::ChangeFunctionUpdate);
		}
		if (!Func.IsNull())
		{
			for (size_t c = 0; c < st.GetColumnCount(); c++)
			{
				if (row->IsNull(c))
				{
					Func->SetParameterNullUTF8(st[c]->GetColumnName().GetBufferConst());
				}
				else
				{
					switch (st[c]->GetDataType())
					{
					case SHVDataVariant::TypeInt:
						Func->SetParameterIntUTF8(st[c]->GetColumnName().GetBufferConst(), row->AsInt(c));
						break;
					case SHVDataVariant::TypeBool:
						Func->SetParameterIntUTF8(st[c]->GetColumnName().GetBufferConst(), row->AsBool(c) ? 1 : 0);
						break;
					case SHVDataVariant::TypeInt64:
						Func->SetParameterInt64UTF8(st[c]->GetColumnName().GetBufferConst(), row->AsInt64(c));
						break;
					case SHVDataVariant::TypeDouble:
						Func->SetParameterDoubleUTF8(st[c]->GetColumnName().GetBufferConst(), row->AsDouble(c));
						 break;
					default:
						Func->SetParameterStringUTF8(st[c]->GetColumnName().GetBufferConst(), row->AsString(c).ToStrUTF8());
						break;
					}
				}
			}
		}
		break;
	case SHVDataRow::RowStateAdding:
		Func = row->GetRowList()->GetDataChangeFunction(SHVDataRowList::ChangeFunctionAdd);
		if (Func.IsNull())
		{
			InsertRowSQL(row, sql, replaceIfDuplicate);
			Func = PrepareFunction(sql);
			row->GetRowList()->SetDataChangeFunction(Func, SHVDataRowList::ChangeFunctionAdd);
		}
		if (!Func.IsNull())
		{
			for (size_t c = 0; c < st.GetColumnCount(); c++)
			{
				if (row->IsNull(c))
				{
					Func->SetParameterNullUTF8(st[c]->GetColumnName().GetBufferConst());
				}
				else
				{
					switch (st[c]->GetDataType())
					{
					case SHVDataVariant::TypeInt:
						Func->SetParameterIntUTF8(st[c]->GetColumnName().GetBufferConst(), row->AsInt(c));
						break;
					case SHVDataVariant::TypeBool:
						Func->SetParameterIntUTF8(st[c]->GetColumnName().GetBufferConst(), row->AsBool(c) ? 1 : 0);
						break;
					case SHVDataVariant::TypeInt64:
						Func->SetParameterInt64UTF8(st[c]->GetColumnName().GetBufferConst(), row->AsInt64(c));
						break;
					case SHVDataVariant::TypeDouble:
						Func->SetParameterDoubleUTF8(st[c]->GetColumnName().GetBufferConst(), row->AsDouble(c));
						 break;
					default:
						Func->SetParameterStringUTF8(st[c]->GetColumnName().GetBufferConst(), row->AsString(c).ToStrUTF8());
						break;
					}
				}
			}
		}
		break;
	case SHVDataRow::RowStateDeleting:
		Func = row->GetRowList()->GetDataChangeFunction(SHVDataRowList::ChangeFunctionDelete);
		if (Func.IsNull())
		{
			DeleteRowSQL(row, sql);
			Func = PrepareFunction(sql);
			row->GetRowList()->SetDataChangeFunction(Func, SHVDataRowList::ChangeFunctionDelete);
		}
		if (!Func.IsNull())
		{
			for (size_t c = 0; c < Key.Count(); c++)
			{
			size_t colIdx;
				if (st.FindColumnIndex(colIdx, Key[c].Key))
				{
					if (row->IsNull(colIdx))
					{
						Func->SetParameterNullUTF8(st[colIdx]->GetColumnName().GetBufferConst());
					}
					else
					{
						switch (st[colIdx]->GetDataType())
						{
						case SHVDataVariant::TypeInt:
							Func->SetParameterIntUTF8(st[colIdx]->GetColumnName().GetBufferConst(), row->AsInt(colIdx));
							break;
						case SHVDataVariant::TypeBool:
							Func->SetParameterIntUTF8(st[c]->GetColumnName().GetBufferConst(), row->AsBool(c) ? 1 : 0);
							break;
						case SHVDataVariant::TypeInt64:
							Func->SetParameterInt64UTF8(st[colIdx]->GetColumnName().GetBufferConst(), row->AsInt64(colIdx));
							break;
						case SHVDataVariant::TypeDouble:
							Func->SetParameterDoubleUTF8(st[colIdx]->GetColumnName().GetBufferConst(), row->AsDouble(colIdx));
							 break;
						default:
							Func->SetParameterStringUTF8(st[colIdx]->GetColumnName().GetBufferConst(), row->AsString(colIdx).ToStrUTF8());
							break;
						}
					}
				}
			}

		}
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
