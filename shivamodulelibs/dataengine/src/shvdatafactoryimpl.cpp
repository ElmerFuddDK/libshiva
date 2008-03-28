#include "stdafx.h"

#include "../../../include/platformspc.h"
#include "../../../include/utils/shvstringutf8.h"
#include "../../../include/threadutils/shvmutexlocker.h"
#include "../include/shvdatafactoryimpl.h"
#include "../include/shvdatasessionsqlite.h"
#include "../include/shvdatarowlistcsqlite.h"
#include "../include/shvdatastructimpl.h"
#include "../include/shvdatastructcsqlite.h"
#include "../include/shvdatavariantimpl.h"
#include "../include/shvdatarowkeyimpl.h"

/*************************************
 * Constructor
 *************************************/
SHVDataFactoryImpl::SHVDataFactoryImpl(SHVDataEngine& engine, const SHVStringC& database): DataEngine(engine), Database(database)
{
	SQLite = DataEngine.CreateConnection(Ok, database);
}

SHVDataFactoryImpl::SHVDataFactoryImpl(SHVDataEngine& engine, const SHVStringC& database, const SHVDataSchema* schema): DataEngine(engine), Database(database)
{
	SQLite = DataEngine.CreateConnection(Ok, database);
	if (Ok && schema)
	{
		for (size_t table = 0; table < schema->CalculateCount(); table++)
		{
			RegisterTable((*schema)[table]);
		}
	}
}

/*************************************
 * RegisterTable
 *************************************/
SHVBool SHVDataFactoryImpl::RegisterTable(const SHVDataStructC* dataStruct, bool createTable)
{
SHVMutexLocker lock(FactoryLock);

const SHVDataStructC* found;
bool create = false;
bool drop   = false;
SHVStringSQLite rest(NULL);
SHVStringUTF8 sql;
SHVSQLiteStatementRef statement;
SHVBool retVal = SHVBool::True;

	found = InternalFindStruct(dataStruct->GetTableName());
	if (found)
	{
		drop = create = !dataStruct->IsEqual(found) && createTable;
	}
	else
	{
		if (createTable)
		{
			create = !TableMatch(SQLite, dataStruct, dataStruct->GetTableName(), drop);
			if (!create)
				drop = false;
		}
	}
	if (drop)
	{
		sql.Format("drop table %s", dataStruct->GetTableName());
		statement = SQLite->ExecuteUTF8(retVal, sql, rest);
		retVal = SHVBool(retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE);
	}
	if (retVal && create) 
	{
		retVal = CreateTable(SQLite, dataStruct, dataStruct->GetTableName());
		if (retVal)
		{
			for (size_t key = 1; key < dataStruct->IndexCount() && retVal; key++)
			{
				retVal = CreateIndex(SQLite, dataStruct, dataStruct->GetTableName(), key);
			}
			if (!retVal)
			{
			SHVBool dropOk;
				sql.Format("drop table %s", dataStruct->GetTableName());
				statement = SQLite->ExecuteUTF8(dropOk, sql, rest);
			}
		}
	}
	if (retVal && !found)
		Schema.Add((SHVDataStructC*) dataStruct);
	return retVal;
}

/*************************************
 * RegisterAlias
 *************************************/
SHVBool SHVDataFactoryImpl::RegisterAlias(const SHVString8C& table, const SHVString8C& alias, bool clear, SHVDataSession* useSession)
{
SHVMutexLocker lock(FactoryLock);

const SHVDataStructC* found;
SHVDataStructCRef aliasfound;
SHVStringUTF8 sql;
SHVStringSQLite rest("");
SHVBool retVal = SHVBool::True;
SHVBool dropOk;
bool create;
bool exists;
SHVSQLiteWrapperRef sqlite;

	if (useSession)
		sqlite = (SHVSQLiteWrapper*) useSession->GetProvider();
	else
		sqlite = SQLite;

	aliasfound = (SHVDataStructC*) InternalFindAlias(alias);
	found = InternalFindStruct(table);	
	if (found)
	{
		create = !TableMatch(sqlite, found, alias, exists) || clear;
		if (create && exists)
		{
			if (aliasfound)
			{
				InternalUnregisterAlias(sqlite, alias);
				aliasfound = NULL;
			}
			else
			{
				sql.Format("drop table %s", alias);
				sqlite->ExecuteUTF8(dropOk, sql, rest)->ValidateRefCount();
			}
		}
		if (create)
		{
			retVal = CreateTable(sqlite, found, alias);
			if (retVal)
			{
				for (size_t key = 1; key < found->IndexCount() && retVal; key++)
				{
					retVal = CreateIndex(sqlite, found, alias, key);
				}
				if (!retVal)
				{
				SHVBool dropOk;
					sql.Format("drop table %s", alias);
					sqlite->ExecuteUTF8(dropOk, sql, rest)->ValidateRefCount();
				}
			}
		}
	}
	else
		retVal = SHVBool::False;

	if (retVal && aliasfound.IsNull())
		Alias.AddTail(SHVDataStructReg(alias, found));
	return retVal;
}

/*************************************
 * UnregisterAlias
 *************************************/
SHVBool SHVDataFactoryImpl::UnregisterAlias(const SHVString8C& alias)
{
SHVMutexLocker lock(FactoryLock);
	return InternalUnregisterAlias(SQLite, alias);
}


/*************************************
 * FindStruct
 *************************************/
const SHVDataStructC* SHVDataFactoryImpl::FindStruct(const SHVString8C& table) const
{
const SHVDataStructC* found = NULL;
SHVMutexLocker lock(FactoryLock);
	found = InternalFindStruct(table);
	if (!found)
	{
		found = InternalFindAlias(table);
	}
	return found;
}

/*************************************
 * GetDataSchema
 *************************************/
const SHVDataSchema& SHVDataFactoryImpl::GetDataSchema() const
{
	return Schema;
}

/*************************************
 * CreateStruct
 *************************************/
SHVDataStruct* SHVDataFactoryImpl::CreateStruct() const
{
	return new SHVDataStructImpl();
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
				colCondition.Format("(@%s is null or %s <= @%s)", Key[i].Key.GetSafeBuffer(), Key[i].Key.GetSafeBuffer(), Key[i].Key.GetSafeBuffer());
			else
				colCondition.Format("(@%s is null or %s >= @%s)", Key[i].Key.GetSafeBuffer(), Key[i].Key.GetSafeBuffer(), Key[i].Key.GetSafeBuffer());
		}
		else
		{
			if (Key[i].Desc != reverse)
				colCondition.Format("(@%s is null or %s.%s <= @%s)", Key[i].Key.GetSafeBuffer(), table.GetSafeBuffer(), Key[i].Key.GetSafeBuffer(), Key[i].Key.GetSafeBuffer());
			else
				colCondition.Format("(@%s is null or %s.%s >= @%s)", Key[i].Key.GetSafeBuffer(), table.GetSafeBuffer(), Key[i].Key.GetSafeBuffer(), Key[i].Key.GetSafeBuffer());
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
 * GetDataEngine
 *************************************/
SHVStringBuffer SHVDataFactoryImpl::GetErrorMessage() const
{
	return SQLite->GetErrorMsg();
}

/*************************************
 * IsOk
 *************************************/
SHVBool SHVDataFactoryImpl::IsOk() const
{
	return Ok;
}

/*************************************
 * Destructor
 *************************************/
SHVDataFactoryImpl::~SHVDataFactoryImpl()
{
SHVListPos p = NULL;
SHVMutexLocker lock(FactoryLock);
	while (ActiveSessions.MoveNext(p))
	{
		ReleaseDataSession(ActiveSessions.GetAt(p));
	}
}

/*************************************
 * CreateSession
 *************************************/
SHVDataSession* SHVDataFactoryImpl::CreateSession()
{
SHVBool ok;
SHVDataSession* retVal = new SHVDataSessionSQLite(DataEngine.GetModuleList(), DataEngine.CreateConnection(ok, Database), this);
	RegisterDataSession(retVal);
	return retVal;
}

/*************************************
 * CreateTable
 *************************************/
SHVBool SHVDataFactoryImpl::CreateTable(SHVSQLiteWrapper* sqlite, const SHVDataStructC* dataStruct, const SHVString8C& tableName)
{
SHVStringUTF8 query;
SHVStringUTF8 col;
SHVStringUTF8 type;
SHVStringUTF8 cols;
SHVStringUTF8 pkey;
SHVStringSQLite notparsed("");
SHVBool ok;

	for (size_t i = 0; i < dataStruct->GetColumnCount(); i++)
	{
		if (i)
			cols += ", ";

		switch ((*dataStruct)[i]->GetDataType())
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
				type.Format("%s(%d)", __SQLITE_TYPE_STRING, (*dataStruct)[i]->GetDataLength());
				break;
			case SHVDataVariant::TypeTime:
				type = __SQLITE_TYPE_DATETIME;
		}
		col.Format("%s %s", (*dataStruct)[i]->GetColumnName().GetSafeBuffer(), type.GetSafeBuffer());
		cols += col;
	}		
	for (size_t i = 0; i < dataStruct->GetPrimaryIndex()->Count(); i++)
	{
		if (i)
			pkey += ", ";
		pkey += (*dataStruct->GetPrimaryIndex())[i].Key.ToStrUTF8();
	}
	query.Format("create table %s(%s,primary key (%s), unique(%s))", tableName.GetSafeBuffer(), cols.GetSafeBuffer(), pkey.GetSafeBuffer(), pkey.GetSafeBuffer());
	
	SHVSQLiteStatementRef statement = sqlite->ExecuteUTF8(ok, query, notparsed);
	return SHVBool(ok.GetError() == SHVSQLiteWrapper::SQLite_DONE);
}

/*************************************
 * CreateIndex
 *************************************/
SHVBool SHVDataFactoryImpl::CreateIndex(SHVSQLiteWrapper* sqlite, const SHVDataStructC* dataStruct, const SHVString8C& tableName, size_t index)
{
SHVStringUTF8 keys;
SHVStringUTF8 sql;
SHVStringSQLite rest(NULL);
SHVBool retVal = SHVBool::False;
SHVSQLiteStatementRef statement;
	if (dataStruct->GetIndex(index))
	{
	const SHVDataRowKey& Key = *dataStruct->GetIndex(index);

		for (size_t k = 0; k < Key.Count(); k++)
		{
			if (k)
				keys += ", ";
			keys += Key[k].Key.GetSafeBuffer();
			if (Key[k].Desc)
				keys += " desc";
		}

		sql.Format("create index %s%05d on %s(%s)",
			tableName.GetSafeBuffer(),
			index,
			tableName.GetSafeBuffer(),
			keys);
		statement = sqlite->ExecuteUTF8(retVal, sql, rest);
		retVal = SHVBool(retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE);
	}
	return retVal;
}

/*************************************
 * TableMatch
 *************************************/
bool SHVDataFactoryImpl::TableMatch(SHVSQLiteWrapper* sqlite, const SHVDataStructC* dataStruct, const SHVString8C& tableName, bool& exists)
{
SHVDataStructCRef sqliteStruct = new SHVDataStructCSQLite(sqlite, tableName);
	exists = sqliteStruct->GetColumnCount() != 0;
	return sqliteStruct->IsEqual(dataStruct);
}

/*************************************
 * RegisterDataSession
 *************************************/
void SHVDataFactoryImpl::RegisterDataSession(SHVDataSession* session)
{
SHVMutexLocker lock(FactoryLock);
	ActiveSessions.AddTail(session);
}

/*************************************
 * UnregisterDataSession
 *************************************/
void SHVDataFactoryImpl::UnregisterDataSession(SHVDataSession* session)
{
SHVMutexLocker lock(FactoryLock);
SHVListPos pos = ActiveSessions.Find(session);
	if (pos)
	{
		ActiveSessions.RemoveAt(pos);
		ReleaseDataSession(session);
	}
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
 * SetSQLite
 *************************************/
void SHVDataFactoryImpl::SetSQLite(SHVSQLiteWrapper* sqlite)
{
	SQLite = sqlite;
}

/*************************************
 * InternalFindAlias
 *************************************/
const SHVDataStructC* SHVDataFactoryImpl::InternalFindAlias(const SHVString8C& table) const
{
const SHVDataStructC* found = NULL;
SHVListIterator<SHVDataStructReg> Iter(Alias);
	while (Iter.MoveNext() && Iter.Get().GetAlias() != table);
	if (Iter)
	{
		found = &Iter.Get().GetStruct();
	}
	return found;
}

/*************************************
 * InternalFindStruct
 *************************************/
const SHVDataStructC* SHVDataFactoryImpl::InternalFindStruct(const SHVString8C& table) const
{
const SHVDataStructC* found = NULL;
	for(size_t t = Schema.CalculateCount(); t && !found;)
	{
		if (Schema[--t]->GetTableName() == table)
			found = Schema[t];
	}
	return found;
}
/*************************************
 * CheckAlias
 *************************************/
bool SHVDataFactoryImpl::CheckAlias(SHVDataSession* session, const SHVString8C& alias)
{
SHVMutexLocker lock(FactoryLock);
	if (PendingUnregisterAlias.Find(alias))
		return InternalUnregisterAlias((SHVSQLiteWrapper*) session->GetProvider(), alias);
	else
		return true;
}

/*************************************
 * InternalUnregisterAlias
 *************************************/
SHVBool SHVDataFactoryImpl::InternalUnregisterAlias(SHVSQLiteWrapper* sqlite, const SHVString8C& alias)
{
SHVMutexLocker lock(FactoryLock);

const SHVDataStructC* aliasfound;
SHVBool retVal = SHVBool::False;
aliasfound = FindStruct(alias);	

	if (aliasfound)
	{
	SHVStringUTF8 sql;
	SHVStringSQLite rest("");
	SHVBool dropOk;
	SHVListIterator<SHVDataStructReg> Iter(Alias);
	// Lets see if there's any active list with that alias
	SHVListIterator<SHVDataSession*> DLIter(ActiveSessions);
	retVal = SHVBool::True;

		while (DLIter.MoveNext() && retVal) 
		{
			retVal = !DLIter.Get()->AliasActive(alias);
		}	
		if (retVal)
		{
		SHVListPos al = PendingUnregisterAlias.Find(alias);
			if (al)
				PendingUnregisterAlias.RemoveAt(al);
			sql.Format("drop table %s", alias);
			sqlite->ExecuteUTF8(retVal, sql, rest)->ValidateRefCount();
			if (retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
			{
				while (Iter.MoveNext() && Iter.Get().GetAlias() != alias);
				if (Iter)
				{
					Alias.RemoveAt(Iter.Pos());
				}
				retVal = SHVBool::True;
			}
		}
		else
		{
			if (!PendingUnregisterAlias.Find(alias))
				PendingUnregisterAlias.AddTail(alias);
			retVal = SHVBool(SHVSQLiteWrapper::SQLite_LOCKED);
		}
	}
	return retVal;
}
