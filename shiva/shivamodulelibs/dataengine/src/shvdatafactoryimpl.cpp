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
#include "../include/shvdatafactoryimpl.h"
#include "../include/shvdatasessionsqlite.h"
#include "../include/shvdatarowlistcsqlite.h"
#include "../include/shvdatastructimpl.h"
#include "../include/shvdatastructcsqlite.h"
#include "../include/shvdatavariantimpl.h"
#include "../include/shvdatarowkeyimpl.h"

// =============================================== Implementation ============================================
/*************************************
 * Constructor
 *************************************/
SHVDataFactoryImpl::SHVDataFactoryImpl(SHVDataEngine& engine, const SHVStringC& database): DataEngine(engine), Database(database), InTransaction(0), SchemaHasChanged(false)
{
}

/*************************************
 * RegisterTable
 *************************************/
SHVBool SHVDataFactoryImpl::RegisterTable(const SHVDataStructC* dataStruct, SHVDataSession* useSession, bool strict, int flags)
{
SHVDataFactoryExclusiveLocker lock(this);
size_t found;
bool create = false;
bool drop   = false;
bool expand = false;
SHVStringSQLite rest(NULL);
SHVStringUTF8 sql;
SHVSQLiteStatementRef statement;
SHVBool retVal = SHVBool::True;
SHVSQLiteWrapperRef SQLite = (useSession ? (SHVSQLiteWrapper*) useSession->GetProvider() : DataEngine.CreateConnection(Ok, Database));
SHVDataStructRef orgStruct;
SHVDataStructRef newStruct = GetInternalStruct((SHVDataStructC*) dataStruct);

	found = InternalFindStruct(dataStruct->GetTableName());
	if (found != SIZE_T_MAX)
	{
	SHVBool isEq(dataStruct->IsEqual(Schema[found],strict));
		if (isEq.GetError() == SHVDataEngine::ErrIsEqualPartialLess && (flags&SHVDataEngine::FlagExpandCols))
			expand = true;
		else
			drop = create = !isEq;
		orgStruct = GetInternalStruct((SHVDataStructC*) Schema[found]);
		Schema.Replace(found, newStruct);
	}
	else
	{
	SHVDataStructCRef dbStruct = RetrieveStruct(dataStruct->GetTableName(), useSession);
	SHVBool isEq(SHVBool::False);
		if (!dbStruct)
			orgStruct = NULL;
		else
			orgStruct = GetInternalStruct(dbStruct);
		if (!orgStruct.IsNull())
		{
			isEq = orgStruct->IsEqual(dataStruct,strict);
			if (isEq.GetError() == SHVDataEngine::ErrIsEqualPartialLess && (flags&SHVDataEngine::FlagExpandCols))
				expand = true;
			else
				drop = create = !isEq;
		}
		else
		{
			create = true;
		}
	}
	if (expand)
	{
		InternalBeginTransaction(SQLite);
		retVal = ExpandTable(SQLite, orgStruct, dataStruct);
		if (retVal)
		{
			SchemaHasChanged = true;
			InternalEndTransaction(SQLite);
		}
		else
			InternalRollbackTransaction(SQLite);
	}
	if (!drop && !orgStruct.IsNull())
	{
	size_t maxIndex = (orgStruct->IndexCount() < dataStruct->IndexCount() ? 
		                 orgStruct->IndexCount() : dataStruct->IndexCount());
	bool equal = true;
	size_t dropFrom;

		for (dropFrom = 0; dropFrom < maxIndex && equal; dropFrom++)
		{
			equal = orgStruct->GetIndex(dropFrom)->KeyDefEquals(dataStruct->GetIndex(dropFrom));
		}
		while (retVal && orgStruct->IndexCount() > dropFrom)
		{
			sql.Format("drop index if exists %s%05d", dataStruct->GetTableName().GetSafeBuffer(), orgStruct->IndexCount() - 1);
			SQLite->ExecuteUTF8(retVal, sql, rest);
			retVal = SHVBool(retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE);
			if (retVal)
				orgStruct->RemoveLastIndex();
		}
		if (orgStruct->IndexCount() < dataStruct->IndexCount())
		{
			for (size_t i = orgStruct->IndexCount(); i < dataStruct->IndexCount(); i++)
			{
				CreateIndex(SQLite, dataStruct, i);
			}
		}
	}
	if (drop && (flags&SHVDataEngine::FlagSuppressDrop))
	{
		retVal.SetError(SHVDataEngine::ErrWouldDrop);
	}
	else if (create && retVal)
	{
		InternalBeginTransaction(SQLite);
		if (drop)
		{
			sql.Format("drop table %s", dataStruct->GetTableName().GetSafeBuffer());
			statement = SQLite->ExecuteUTF8(retVal, sql, rest);
			retVal = SHVBool(retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE);
		}
		if (retVal && create) 
		{
			retVal = CreateTable(SQLite, dataStruct);
			if (retVal)
			{
				for (size_t key = 1; key < dataStruct->IndexCount() && retVal; key++)
				{
					retVal = CreateIndex(SQLite, dataStruct, key);
				}
			}
		}

		if (retVal)
		{
			SchemaHasChanged = true;
			InternalEndTransaction(SQLite);
		}
		else
			InternalRollbackTransaction(SQLite);
	}
	if (retVal)
	{
		if (found == SIZE_T_MAX)
			Schema.Add((SHVDataStructC*) dataStruct);
		if (!dataStruct->GetIsMultiInstance() && !InternalFindAlias(dataStruct->GetTableName()))
			Alias.AddTail(SHVDataStructReg(dataStruct->GetTableName(), -1, dataStruct));
	}
	return retVal;
}

/*************************************
 * RegisterAlias
 *************************************/
SHVBool SHVDataFactoryImpl::RegisterAlias(const SHVString8C& table, const SHVString8C& alias, bool clear, SHVDataSession* useSession)
{
SHVDataFactoryExclusiveLocker lock(this);
const SHVDataStructC* found;
size_t foundIdx;
const SHVDataStructReg* aliasfound;
SHVStringUTF8 sql;
SHVStringSQLite rest("");
SHVBool retVal = SHVBool::True;
bool exists;
SHVSQLiteWrapperRef SQLite = (useSession ? (SHVSQLiteWrapper*) useSession->GetProvider() : DataEngine.CreateConnection(Ok, Database));
SHVSQLiteStatementRef statement;
int id = -1;
	
	SHVASSERT(table != alias);
 	aliasfound = InternalFindAlias(alias);
	SHVASSERT(!aliasfound || aliasfound->GetAliasID() != -1);
	foundIdx = InternalFindStruct(table);
	if (foundIdx != SIZE_T_MAX && !InternalFindAlias(table))
		found = Schema[foundIdx];
	else
		found = NULL;
	InternalBeginTransaction(SQLite);
	if (found)
	{
		SQLite->ExecuteUTF8(retVal, SHVStringUTF8C::Format("select * from sqlite_master where type='view' and name='%s'", alias.GetSafeBuffer()), rest);
		exists = retVal.GetError() == SHVSQLiteWrapper::SQLite_ROW;
		if (retVal.GetError() == SHVSQLiteWrapper::SQLite_ROW ||
			retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
			retVal = SHVBool::True;
		if (retVal)
		{
			if (!exists)
			{
				retVal = CreateView(SQLite, found, alias, id);
				if (retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
				{
					retVal = SHVBool::True;
					SchemaHasChanged = true;
				}				
			}
			else
			{
				if (aliasfound)
					id = aliasfound->GetAliasID();
				else
					id = GetAliasID(SQLite, alias, false);
				if (clear)
				{
					if (id != -1)
					{
						SQLite->ExecuteUTF8(retVal, SHVStringUTF8C::Format("delete from %s where shv_alias=%d", found->GetTableName().GetSafeBuffer(), id), rest)->ValidateRefCount();
						if (retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
							retVal = SHVBool::True;
					}
					else
						retVal.SetError(SHVSQLiteWrapper::SQLite_MISUSE);
				}
			}
		}
	}
	else
		retVal.SetError(SHVSQLiteWrapper::SQLite_MISUSE);
	if (retVal)
	{
		if (!aliasfound)
			Alias.AddTail(SHVDataStructReg(alias, id, found));
		InternalEndTransaction(SQLite);
	}
	else
		InternalRollbackTransaction(SQLite);
	return retVal;
}
/*************************************
 * RegisterIndex
 *************************************/
size_t SHVDataFactoryImpl::RegisterIndex(const SHVString8C &table, SHVDataRowKey *IndexKey, SHVDataSession* useSession)
{
SHVDataFactoryExclusiveLocker lock(this);
bool Exists = false;
size_t found = InternalFindStruct(table);
size_t retVal = SIZE_T_MAX;
SHVSQLiteWrapperRef SQLite = (useSession ? (SHVSQLiteWrapper*) useSession->GetProvider() : DataEngine.CreateConnection(Ok, Database));

	SHVASSERT(found != SIZE_T_MAX);
	if (found != SIZE_T_MAX)
	{
	SHVDataStruct* dataStruct = GetInternalStruct((SHVDataStructC* )Schema[found]);		
		for (retVal = 0; retVal < dataStruct->IndexCount() && !Exists; retVal++)
		{
			Exists = dataStruct->GetIndex(retVal)->KeyDefEquals(IndexKey);
			if (Exists)
				break;
		}
		if (!Exists)
		{
			retVal = dataStruct->AddIndex(IndexKey);
			if (!CreateIndex(SQLite, Schema[found], retVal))
			{
				dataStruct->RemoveLastIndex();
				retVal = SIZE_T_MAX;
			}
		}
	}
	return retVal;
}

/*************************************
 * UnregisterAlias
 *************************************/
SHVBool SHVDataFactoryImpl::UnregisterAlias(const SHVString8C& alias, SHVDataSession* useSession)
{
SHVDataFactoryExclusiveLocker lock(this);
SHVDataStructReg* aliasfound;

	aliasfound = InternalFindAlias(alias);
	if (aliasfound)
		return InternalDropAlias(aliasfound->GetStruct().GetTableName(), alias, aliasfound, useSession);
	else
		return SHVBool::False;
}

/*************************************
 * DropAlias
 *************************************/
SHVBool SHVDataFactoryImpl::DropAlias(const SHVString8C& table, const SHVString8C& alias, SHVDataSession* useSession)
{
SHVDataFactoryExclusiveLocker lock(this);
SHVDataStructReg* aliasfound;

	aliasfound = InternalFindAlias(alias);	
	SHVASSERT(!aliasfound || aliasfound->GetAliasID() != -1);
	return InternalDropAlias(table, alias, aliasfound, useSession);
}


/*************************************
 * ClearTable
 *************************************/
SHVBool SHVDataFactoryImpl::ClearTable(const SHVString8C &table, SHVDataSession *useSession)
{
SHVDataFactoryExclusiveLocker(this);
SHVSQLiteWrapperRef SQLite = (useSession ? (SHVSQLiteWrapper*) useSession->GetProvider() : DataEngine.CreateConnection(Ok, Database));
SHVStringSQLite rest("");
SHVBool retVal;
	retVal = InternalBeginTransaction(SQLite);
	if (retVal)
	{
		SQLite->ExecuteUTF8(retVal, SHVStringUTF8C::Format("delete from %s", table.GetSafeBuffer()), rest);
		if (retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
			retVal = InternalEndTransaction(SQLite);
		else
			retVal = InternalRollbackTransaction(SQLite);
	}
	return retVal;
}

/*************************************
 * FindStruct
 *************************************/
const SHVDataStructC* SHVDataFactoryImpl::FindStruct(const SHVString8C& table) const
{
size_t found = SIZE_T_MAX;
const SHVDataStructC* retVal;
	((SHVDataFactoryImpl*) this)->LockShared();
	found = InternalFindStruct(table);
	if (found == SIZE_T_MAX)
	{
	const SHVDataStructReg* reg = InternalFindAlias(table);
		retVal = (reg ? &reg->GetStruct() : NULL);
	}
	else
		retVal = Schema[found];
	((SHVDataFactoryImpl*) this)->Unlock();
	return retVal;
}

/*************************************
 * ReloadStruct
 *************************************/
const SHVDataStructC* SHVDataFactoryImpl::ReloadStruct(const SHVString8C& table, SHVDataSession* useSession)
{

SHVDataStructC* retVal = NULL;

	((SHVDataFactoryImpl*) this)->LockShared();

	retVal = RetrieveStruct(table,useSession);
	if (retVal)
	{
	size_t found = InternalFindStruct(table);
		if (found != SIZE_T_MAX)
			Schema.Replace(found, retVal);
		else
			Schema.Add(retVal);
	}
	
	((SHVDataFactoryImpl*) this)->Unlock();
	
	return retVal;
}

/*************************************
 * GetDataSchema
 *************************************/
const SHVDataSchema& SHVDataFactoryImpl::GetDataSchema() const
{
	return Schema;
}

/*************************************
 * CreateSession
 *************************************/
SHVDataSession* SHVDataFactoryImpl::CreateSession()
{
SHVDataFactoryExclusiveLocker(this);
SHVBool ok;
SHVSQLiteWrapperRef sqlite;
SHVDataSession* retVal;
	retVal = new SHVDataSessionSQLite(DataEngine.GetModuleList(), DataEngine.CreateConnection(ok, Database), this);
	RegisterDataSession(retVal);
	return retVal;
}

/*************************************
 * CreateStruct
 *************************************/
SHVDataStruct* SHVDataFactoryImpl::CreateStruct() const
{
	return new SHVDataStructImpl();
}

/*************************************
 * CopyStruct
 *************************************/
SHVDataStruct * SHVDataFactoryImpl::CopyStruct(const SHVDataStructC *struc)
{
SHVDataStruct* retVal = CreateStruct();
const SHVDataStructC& dStruc = *struc;
	retVal->SetTableName(struc->GetTableName());
	for (size_t c = 0; c < struc->GetColumnCount(); c++)
	{
		retVal->Add(dStruc[c]->GetColumnName(),
					dStruc[c]->GetDataType(),
					dStruc[c]->GetDataLength(),
					false,
					dStruc[c]->GetAllowNull(),
					dStruc[c]->GetAutoInc());
	}
	for (size_t i = 0; i < struc->IndexCount(); i++)
		retVal->AddIndex(CopyKey(struc->GetIndex(i)));
	return retVal;
}

/*************************************
 * CreateVariant
 *************************************/
SHVDataVariant* SHVDataFactoryImpl::CreateVariant() const
{
	return new SHVDataVariantImpl();
}
/*************************************
 * CreateKey
 *************************************/
SHVDataRowKey* SHVDataFactoryImpl::CreateKey() const 
{
	return new SHVDataRowKeyImpl();
}

/*************************************
 * CopyKey
 *************************************/
SHVDataRowKey* SHVDataFactoryImpl::CopyKey(const SHVDataRowKey* key) const 
{
	return new SHVDataRowKeyImpl(key);
}

/*************************************
 * RetrieveStruct
 *************************************/
SHVDataStructC* SHVDataFactoryImpl::RetrieveStruct(const SHVString8C table, SHVDataSession* useSession) const
{
SHVBool ok;
SHVSQLiteWrapperRef SQLite = (useSession ? (SHVSQLiteWrapper*) useSession->GetProvider() : DataEngine.CreateConnection(ok, Database));
SHVDataStructCSQLite* struc = new SHVDataStructCSQLite((SHVSQLiteWrapper*) SQLite.AsConst(), table);
	if (!struc->GetColumnCount())
	{
		struc->ValidateRefCount();
		struc = NULL;
	}
	return struc;
}

/*************************************
 * GetDatabase
 *************************************/
const SHVStringC& SHVDataFactoryImpl::GetDatabase() const
{
	return Database;
}

/*************************************
 * BuildKeySQL
 *************************************/
void SHVDataFactoryImpl::BuildKeySQL(const SHVDataRowKey* key, SHVString8& condition, SHVString8& orderby, const SHVString8C& table, bool reverse) const
{
SHVString8 colCondition;
const SHVDataRowKey& Key = *key;
	for (size_t i = 0; i < Key.Count(); i++)
	{
		if (table.IsNull() || table == "")
		{
			if (Key[i].Desc != reverse)
				colCondition.Format("(@%s is null or \"%s\" <= @%s)", Key[i].Key.GetSafeBuffer(), Key[i].Key.GetSafeBuffer(), Key[i].Key.GetSafeBuffer());
			else
				colCondition.Format("(@%s is null or \"%s\" >= @%s)", Key[i].Key.GetSafeBuffer(), Key[i].Key.GetSafeBuffer(), Key[i].Key.GetSafeBuffer());
		}
		else
		{
			if (Key[i].Desc != reverse)
				colCondition.Format("(@%s is null or %s.\"%s\" <= @%s)", Key[i].Key.GetSafeBuffer(), table.GetSafeBuffer(), Key[i].Key.GetSafeBuffer(), Key[i].Key.GetSafeBuffer());
			else
				colCondition.Format("(@%s is null or %s.\"%s\" >= @%s)", Key[i].Key.GetSafeBuffer(), table.GetSafeBuffer(), Key[i].Key.GetSafeBuffer(), Key[i].Key.GetSafeBuffer());
		}
		
		if (!i)
		{
			condition = colCondition;
			orderby = Key[i].Key;
		}
		else
		{
			condition += " and ";
			condition += colCondition;
			orderby += ", ";
			orderby += Key[i].Key;
		}
		if (Key[i].Desc != reverse)
			orderby += " desc";
	}
}

/*************************************
 * SubscribeRowChange
 *************************************/
void SHVDataFactoryImpl::SubscribeRowChange(SHVEventSubscriberBase* sub)
{
	DataChangedSubscription = sub;
}

/*************************************
 * GetDataEngine
 *************************************/
SHVDataEngine& SHVDataFactoryImpl::GetDataEngine()
{
	return DataEngine;
}

/*************************************
 * IsOk
 *************************************/
SHVBool SHVDataFactoryImpl::IsOk() const
{
	return Ok;
}

/*************************************
 * LockShared
 *************************************/
void SHVDataFactoryImpl::LockShared()
{
#ifdef DEBUG
int lockCount = 0;
	SharedLocksMutex.Lock();
	if (!Lock.IsExclusiveLocked())
	{
		if (SharedLocks.Find(SHVThreadBase::GetCurrentThreadID()))
			lockCount = SharedLocks[SHVThreadBase::GetCurrentThreadID()];
		lockCount++;
		SharedLocks[SHVThreadBase::GetCurrentThreadID()] = lockCount;
	}
	SharedLocksMutex.Unlock();
#endif
	Lock.LockShared();
}

/*************************************
 * LockExclusive
 *************************************/
void SHVDataFactoryImpl::LockExclusive()
{
#ifdef DEBUG
	SharedLocksMutex.Lock();
	SHVASSERT(!SharedLocks.Find(SHVThreadBase::GetCurrentThreadID()));
	SharedLocksMutex.Unlock();
#endif
	Lock.LockExclusive();
}

/*************************************
 * TryLockExclusive
 *************************************/
SHVBool SHVDataFactoryImpl::TryLockExclusive()
{
#ifdef DEBUG
	SharedLocksMutex.Lock();
	SHVASSERT(!SharedLocks.Find(SHVThreadBase::GetCurrentThreadID()));
	SharedLocksMutex.Unlock();
#endif
	return Lock.TryLockExclusive();
}

/*************************************
 * Unlock
 *************************************/
void SHVDataFactoryImpl::Unlock()
{
#ifdef DEBUG
int lockCount = 0;
	SharedLocksMutex.Lock();
	if (!Lock.IsExclusiveLocked())
	{
		if (SharedLocks.Find(SHVThreadBase::GetCurrentThreadID()))
		{
			lockCount = SharedLocks[SHVThreadBase::GetCurrentThreadID()] - 1;
			if (!lockCount)	
				SharedLocks.Remove(SHVThreadBase::GetCurrentThreadID());
			else
				SharedLocks[SHVThreadBase::GetCurrentThreadID()] = lockCount;
		}
	}
	SharedLocksMutex.Unlock();
#endif
	Lock.Unlock();
}

/*************************************
 * Destructor
 *************************************/
SHVDataFactoryImpl::~SHVDataFactoryImpl()
{
}


/*************************************
 * CreateTable
 *************************************/
SHVBool SHVDataFactoryImpl::CreateTable(SHVSQLiteWrapper* sqlite, const SHVDataStructC* dataStruct)
{
SHVDataFactoryExclusiveLocker(this);
SHVStringUTF8 query;
SHVStringUTF8 col;
SHVStringUTF8 type;
SHVStringUTF8 cols;
SHVStringUTF8 pkey;
SHVStringSQLite notparsed("");
SHVBool ok;

	if (dataStruct->GetIsMultiInstance())
		cols = "shv_alias " __SQLITE_TYPE_INT;
	for (size_t i = 0; i < dataStruct->GetColumnCount(); i++)
	{
		if (!cols.IsEmpty())
			cols += ", ";

		switch ((*dataStruct)[i]->GetDataType())
		{
			case SHVDataVariant::TypeInt:
				type = __SQLITE_TYPE_INT;
				break;
			case SHVDataVariant::TypeInt64:
				type = __SQLITE_TYPE_INT64;
				break;
			case SHVDataVariant::TypeBool:
				type = __SQLITE_TYPE_BOOL;
				break;
			case SHVDataVariant::TypeDouble:
				type = __SQLITE_TYPE_DOUBLE;
				break;
			case SHVDataVariant::TypeString:
				type.Format("%s(%d)", __SQLITE_TYPE_STRING, (*dataStruct)[i]->GetDataLength());
				break;
			case SHVDataVariant::TypeTime:
				type = __SQLITE_TYPE_DATETIME;
		}
		col.Format("\"%s\" %s", (*dataStruct)[i]->GetColumnName().GetSafeBuffer(), type.GetSafeBuffer());
		cols += col;
	}
	if (dataStruct->GetIsMultiInstance())
		pkey = "shv_alias";
	for (size_t i = 0; i < dataStruct->GetPrimaryIndex()->Count(); i++)
	{
		if (!pkey.IsEmpty())
			pkey += ", ";
		pkey += "\"" + (*dataStruct->GetPrimaryIndex())[i].Key.ToStrUTF8() + "\"";
	}
	query.Format("create table %s(%s,primary key (%s), unique(%s))", dataStruct->GetTableName().GetSafeBuffer(), cols.GetSafeBuffer(), pkey.GetSafeBuffer(), pkey.GetSafeBuffer());
	
	SHVSQLiteStatementRef statement = sqlite->ExecuteUTF8(ok, query, notparsed);
	return SHVBool(ok.GetError() == SHVSQLiteWrapper::SQLite_DONE);
}

/*************************************
 * ExpandTable
 *************************************/
SHVBool SHVDataFactoryImpl::ExpandTable(SHVSQLiteWrapper* sqlite, const SHVDataStructC* oldStruct, const SHVDataStructC* newStruct)
{
SHVDataFactoryExclusiveLocker(this);
SHVStringUTF8 query;
SHVStringUTF8 type;
SHVStringSQLite notparsed("");
SHVBool ok;
SHVBool result = true;
size_t j;

	for (size_t i = 0; i < newStruct->GetColumnCount() && result; i++)
	{
	bool exists = false;
	SHVSQLiteStatementRef statement;

		for (j = 0; j < oldStruct->GetColumnCount() && !exists; j++)
		{
			exists = ((*oldStruct)[j]->GetColumnName() == (*newStruct)[i]->GetColumnName());
		}

		if (exists)
			continue;

		switch ((*newStruct)[i]->GetDataType())
		{
			case SHVDataVariant::TypeInt:
				type = __SQLITE_TYPE_INT;
				break;
			case SHVDataVariant::TypeInt64:
				type = __SQLITE_TYPE_INT64;
				break;
			case SHVDataVariant::TypeBool:
				type = __SQLITE_TYPE_BOOL;
				break;
			case SHVDataVariant::TypeDouble:
				type = __SQLITE_TYPE_DOUBLE;
				break;
			case SHVDataVariant::TypeString:
				type.Format("%s(%d)", __SQLITE_TYPE_STRING, (*newStruct)[i]->GetDataLength());
				break;
			case SHVDataVariant::TypeTime:
				type = __SQLITE_TYPE_DATETIME;
		}
		query.Format("alter table [%s] add \"%s\" %s;"
					 ,newStruct->GetTableName().GetSafeBuffer()
					 ,(*newStruct)[i]->GetColumnName().GetSafeBuffer()
					 ,type.GetSafeBuffer()
					 );
		statement = sqlite->ExecuteUTF8(ok, query, notparsed);
		result = (ok.GetError() == SHVSQLiteWrapper::SQLite_DONE);
	}

	return result;
}

/*************************************
 * CreateView
 *************************************/
SHVBool SHVDataFactoryImpl::CreateView(SHVSQLiteWrapper* sqlite, const SHVDataStructC* dataStruct, const SHVString8C& viewName, int &id)
{
SHVDataFactoryExclusiveLocker(this);
SHVStringUTF8 selectcols;
SHVStringSQLite rest("");
SHVBool ok;

	for (size_t i = 0; i < dataStruct->GetColumnCount(); i++)
	{
		if (i)
			selectcols += ",";
		selectcols += SHVStringUTF8C("\"") + (*dataStruct)[i]->GetColumnName().GetSafeBuffer() + SHVStringUTF8C("\"");
	}
	id = GetAliasID(sqlite, viewName, true);
	if (id != -1)
		sqlite->ExecuteUTF8(ok, SHVStringUTF8C::Format("create view if not exists %s as select %s from %s where shv_alias = %d", 
				viewName.GetSafeBuffer(), 
				selectcols.GetSafeBuffer(),
				dataStruct->GetTableName().GetSafeBuffer(),
				id), rest);
	return ok.GetError() == SHVSQLiteWrapper::SQLite_DONE;
}

/*************************************
 * CreateIndex
 *************************************/
SHVBool SHVDataFactoryImpl::CreateIndex(SHVSQLiteWrapper* sqlite, const SHVDataStructC* dataStruct, size_t index)
{
SHVDataFactoryExclusiveLocker(this);
SHVStringUTF8 keys;
SHVStringUTF8 sql;
SHVString8 tableNameNoDB;
SHVStringSQLite rest(NULL);
SHVBool retVal = SHVBool::False;
SHVSQLiteStatementRef statement;
SHVString8 tableName = dataStruct->GetTableName();
size_t tokPos = 0;
	if (dataStruct->GetIndex(index))
	{
	const SHVDataRowKey& Key = *dataStruct->GetIndex(index);

		if (dataStruct->GetIsMultiInstance())
			keys = "shv_alias";
		for (size_t k = 0; k < Key.Count(); k++)
		{
			if (!keys.IsEmpty())
				keys += ", ";
			keys += SHVStringUTF8C("\"") + Key[k].Key.GetSafeBuffer() + SHVStringUTF8C("\"");
			if (Key[k].Desc)
				keys += " desc";
		}
		tableName.Tokenize(".", tokPos);
		tableNameNoDB = tableName.Tokenize(".", tokPos);
		if (!tableNameNoDB.GetLength())
			tableNameNoDB = tableName; 
		sql.Format("create index %s%05d on %s(%s)",
			tableName.GetSafeBuffer(),
			index,
			tableNameNoDB.GetSafeBuffer(),
			keys.GetSafeBuffer());
		statement = sqlite->ExecuteUTF8(retVal, sql, rest);
		retVal = SHVBool(retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE);
	}
	return retVal;
}

/*************************************
 * GetAliasID
 *************************************/
int SHVDataFactoryImpl::GetAliasID(SHVSQLiteWrapper* sqlite, const SHVString8C& alias, bool create)
{
SHVBool ok;
SHVStringSQLite rest(NULL);
long id = 0;
	if (!sqlite->ExecuteUTF8(ok, SHVStringUTF8C::Format("select id from shv_alias where alias='%s'", alias.GetSafeBuffer()), rest)->GetLong(id, 0))
	{
		sqlite->ExecuteUTF8(ok, "select max(id) from shv_alias", rest)->GetLong(id, 0);
		id++;
		if (create)
		{
			sqlite->ExecuteUTF8(ok, SHVStringUTF8C::Format("insert into shv_alias values(%d, '%s')", id, alias.GetSafeBuffer()), rest);
			if (ok.GetError() != SHVSQLiteWrapper::SQLite_DONE)
				id = -1;
		}
		else
			id = -1;
	}
	return id;
}

/*************************************
 * TableMatch
 *************************************/
bool SHVDataFactoryImpl::TableMatch(SHVSQLiteWrapper* sqlite, const SHVDataStructC* dataStruct, const SHVString8C& tableName, bool& exists, bool strict)
{
SHVDataStructCRef sqliteStruct;
	LockShared();
	sqliteStruct = new SHVDataStructCSQLite(sqlite, tableName);
	Unlock();
	exists = sqliteStruct->GetColumnCount() != 0;
	return sqliteStruct->IsEqual(dataStruct,strict);
}

/*************************************
 * InternalFindAlias
 *************************************/
SHVDataStructReg* SHVDataFactoryImpl::InternalFindAlias(const SHVString8C& table) const
{
SHVDataStructReg* found = NULL;
SHVListIterator<SHVDataStructReg> Iter(Alias);
	((SHVDataFactoryImpl*) this)->LockShared();
	while (Iter.MoveNext() && (Iter.Get().GetDeleted() || Iter.Get().GetAlias() != table));
	if (Iter && !Iter.Get().GetDeleted())
	{
		found = &Iter.Get();
	}
	((SHVDataFactoryImpl*) this)->Unlock();
	return found;
}

/*************************************
 * InternalFindStruct
 *************************************/
size_t SHVDataFactoryImpl::InternalFindStruct(const SHVString8C& table) const
{
size_t found = SIZE_T_MAX;
	((SHVDataFactoryImpl*) this)->LockShared();
	for(size_t t = Schema.CalculateCount(); t && found == SIZE_T_MAX;)
	{
		if (Schema[--t]->GetTableName() == table)
			found = t;
	}
	((SHVDataFactoryImpl*) this)->Unlock();
	return found;
}

/*************************************
 * SchemaChanged
 *************************************/
void SHVDataFactoryImpl::SchemaChanged(SHVSQLiteWrapper* sqlite)
{
SessionLock.LockShared();
SHVListIterator<SHVDataSession*> Iter(ActiveSessions);
	while(Iter.MoveNext())
	{
		if (sqlite != Iter.Get()->GetProvider(false))
			SHVDataFactory::SchemaChanged(Iter.Get());
	}
	SessionLock.Unlock();
}

/*************************************
 * InternalDropAlias
 *************************************/
SHVBool SHVDataFactoryImpl::InternalDropAlias(const SHVString8C& table, const SHVString8C& alias, SHVDataStructReg* aliasfound, SHVDataSession* useSession)
{
int id = -1;
SHVBool retVal = SHVBool::False;
SHVStringSQLite rest("");
SHVSQLiteWrapperRef SQLite = (useSession ? (SHVSQLiteWrapper*) useSession->GetProvider() : DataEngine.CreateConnection(Ok, Database));

	if (aliasfound)
		id = aliasfound->GetAliasID();
	else
		id = GetAliasID(SQLite, alias, false);
	InternalBeginTransaction(SQLite);
	if (aliasfound)
		aliasfound->SetDeleted(true);

	if (id == -1) // Table mode
	{
		SQLite->ExecuteUTF8(retVal, SHVStringUTF8C::Format("delete from %s", alias.GetSafeBuffer()), rest);
	}
	else // View mode
	{
		SQLite->ExecuteUTF8(retVal, SHVStringUTF8C::Format("drop view %s", alias.GetSafeBuffer()), rest);
		if (retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
		{
			SQLite->ExecuteUTF8(retVal,
				SHVStringUTF8C::Format("delete from %s where shv_alias=%d",
					table.GetSafeBuffer(),
					id
				),
				rest);
			if (retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
			{
				SQLite->ExecuteUTF8(retVal,
					SHVStringUTF8C::Format("delete from shv_alias where id=%d",
						id
					),
					rest);
			}
			retVal = (retVal == SHVSQLiteWrapper::SQLite_DONE ? SHVBool::True : retVal);
		}
	}
	if (retVal)
	{
		SchemaHasChanged = true;
		InternalEndTransaction(SQLite);
	}
	else
		InternalRollbackTransaction(SQLite);
	return retVal;
}

/*************************************
 * RegisterDataSession
 *************************************/
void SHVDataFactoryImpl::RegisterDataSession(SHVDataSession* session)
{
SessionLock.LockExclusive();
	ActiveSessions.AddTail(session);
	SessionLock.Unlock();
} 

/*************************************
 * UnregisterDataSession
 *************************************/
void SHVDataFactoryImpl::UnregisterDataSession(SHVDataSession* session)
{
SessionLock.LockExclusive();
SHVListPos pos = ActiveSessions.Find(session);
	if (pos)
	{
		ActiveSessions.RemoveAt(pos);
	}
	SessionLock.Unlock();
}

/*************************************
 * RowChanged
 *************************************/
void SHVDataFactoryImpl::RowChanged(SHVDataRow* row)
{
	DataEngine.FactoryRowChanged(row);
	if (!DataChangedSubscription.IsNull())
	{
		if (row->GetRowState() != SHVDataRow::RowStateUnchanged &&
			row->GetRowState() == SHVDataRow::RowStateInvalid)
			DataChangedSubscription->EmitNow(DataEngine.GetModuleList(), new SHVEventDataRowChanged(row, NULL, SHVDataFactory::EventRowChanged, SHVInt(), this));
	}
}


/*************************************
 * BeginTransaction
 *************************************/
SHVBool SHVDataFactoryImpl::BeginTransaction(SHVDataSession* session, bool wait)
{
SHVBool retVal = SHVBool::True;
	if (wait)
		LockExclusive();
	else
	{
		retVal = TryLockExclusive();
		if (!retVal)
			retVal.SetError(SHVSQLiteWrapper::SQLite_LOCKED);
	}
	return (retVal ? InternalBeginTransaction((SHVSQLiteWrapper*) session->GetProvider()) : retVal);
}

SHVBool SHVDataFactoryImpl::InternalBeginTransaction(SHVSQLiteWrapper* sqlite)
{
SHVStringSQLite rest(NULL);
SHVBool ok = SHVBool::True;
	if (!InTransaction)
	{
		BeginningTransaction(&DataEngine);
		sqlite->ExecuteUTF8(ok, "BEGIN TRANSACTION", rest);
		if (ok == SHVSQLiteWrapper::SQLite_DONE)
			ok = SHVBool::True;
#ifdef DEBUG
		else
			SHVTRACE(_S("Begin transaction: %s\n"), sqlite->GetErrorMsg().GetSafeBuffer());
#endif
	}
	if (ok)
		InTransaction++;
	return ok;
}

/*************************************
 * EndTransaction
 *************************************/
SHVBool SHVDataFactoryImpl::EndTransaction(SHVDataSession* session)
{
SHVBool retVal;
	SHVASSERT(InTransaction);
	retVal = InternalEndTransaction((SHVSQLiteWrapper*) session->GetProvider());
	Unlock();
	return retVal;
}

SHVBool SHVDataFactoryImpl::InternalEndTransaction(SHVSQLiteWrapper* sqlite)
{
SHVStringSQLite rest(NULL);
SHVBool ok = SHVBool::True;
SHVListPos p;
bool more;

	if (--InTransaction == 0)
	{
		sqlite->ExecuteUTF8(ok, "END TRANSACTION", rest);
		if (ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
		{
			ok = SHVBool::True;
			p = Alias.GetHeadPosition();
			more = p != NULL;
			while (more)
			{
				if (Alias.GetAt(p).GetDeleted())
				{
				SHVListPos rem = p;
					more = Alias.MoveNext(p);
					Alias.RemoveAt(rem);
				}
				else
				{
					Alias.GetAt(p).SetTemp(false);
					more = Alias.MoveNext(p);
				}			
			}
			if (SchemaHasChanged)
			{
				SchemaChanged(sqlite);
				SchemaHasChanged = false;
			}
		}
		else
		{
#ifdef DEBUG
			SHVTRACE(_S("End transaction: %s\n"), sqlite->GetErrorMsg().GetSafeBuffer());
#endif
			InTransaction++;
			InternalRollbackTransaction(sqlite);
		}
		FinishedTransaction(&DataEngine,ok);
	}
	return ok;
}

/*************************************
 * RollbackTransaction
 *************************************/
SHVBool SHVDataFactoryImpl::RollbackTransaction(SHVDataSession* session)
{
SHVBool retVal;
	SHVASSERT(InTransaction);
	retVal = InternalRollbackTransaction((SHVSQLiteWrapper*) session->GetProvider());
	Unlock();
	return retVal;
}

SHVBool SHVDataFactoryImpl::InternalRollbackTransaction(SHVSQLiteWrapper* sqlite)
{
SHVStringSQLite rest(NULL);
SHVBool ok = SHVBool::True;
SHVListPos p;
bool more;

	if (--InTransaction == 0)
	{
		sqlite->ExecuteUTF8(ok, "ROLLBACK TRANSACTION", rest);
		if (ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
		{
			ok = SHVBool::True;
			p = Alias.GetHeadPosition();
			more = p != NULL;
			while (more)
			{
				if (Alias.GetAt(p).GetTemp())
				{
				SHVListPos rem = p;
					more = Alias.MoveNext(p);
					Alias.RemoveAt(rem);
				}
				else
				{
					Alias.GetAt(p).SetDeleted(false);
					more = Alias.MoveNext(p);
				}			
			}
		}
		FinishedTransaction(&DataEngine,false);
	}
	return ok;
}
/*************************************
 * GetInTransaction
 *************************************/
bool SHVDataFactoryImpl::GetInTransaction() const
{
	return InTransaction != 0;
}

/*************************************
 * GetAliasID
 *************************************/
int SHVDataFactoryImpl::GetAliasID(const SHVDataSession *dataSession, const SHVString8C& alias) const
{
	return ((SHVDataFactoryImpl*) this)->GetAliasID((SHVSQLiteWrapper*) ((SHVDataSession*) dataSession)->GetProvider(), alias, false);
}

