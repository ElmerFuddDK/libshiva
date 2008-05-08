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

// ============================================= Transaction locker ==========================================

class SHVTransactionLocker
{
public:
	inline SHVTransactionLocker(SHVDataFactoryImpl& owner, SHVMutex& outerLock);
	inline ~SHVTransactionLocker();
	inline bool IsOk();
private:
	SHVDataFactoryImpl& Owner;
	SHVMutex& Outerlock;
	bool ok;
};

/*************************************
 * Constructor
 *************************************/
SHVTransactionLocker::SHVTransactionLocker(SHVDataFactoryImpl& owner, SHVMutex& outerlock): Owner(owner), Outerlock(outerlock)
{
	Outerlock.Lock();
	ok = Owner.LockTransaction();
}

/*************************************
 * Destructor
 *************************************/
SHVTransactionLocker::~SHVTransactionLocker()
{
	if (ok)
		Owner.UnlockTransaction();
	Outerlock.Unlock();
}

/*************************************
 * IsOk
 *************************************/
bool SHVTransactionLocker::IsOk()
{
	return ok;
}

// =============================================== Implementation ============================================
/*************************************
 * Constructor
 *************************************/
SHVDataFactoryImpl::SHVDataFactoryImpl(SHVDataEngine& engine, const SHVStringC& database): DataEngine(engine), Database(database), ConnectionDirty(false)
{
	SQLite = DataEngine.CreateConnection(Ok, database);
}

SHVDataFactoryImpl::SHVDataFactoryImpl(SHVDataEngine& engine, const SHVStringC& database, const SHVDataSchema* schema): DataEngine(engine), Database(database), ConnectionDirty(false)
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
SHVTransactionLocker lockTrans(*this, FactoryLock);
const SHVDataStructC* found;
bool create = false;
bool drop   = false;
SHVStringSQLite rest(NULL);
SHVStringUTF8 sql;
SHVSQLiteStatementRef statement;
SHVBool retVal = SHVBool::True;

	if (lockTrans.IsOk())
	{
		if (createTable)
			BeginTransaction(NULL);
	}

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
	CheckConnection();
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
		}
	}
	if (retVal && !found)
		Schema.Add((SHVDataStructC*) dataStruct);

	if (lockTrans.IsOk())
	{
		if (retVal)
			EndTransaction(NULL);
		else
			RollbackTransaction(NULL);
	}
	return retVal;
}

/*************************************
 * RegisterAlias
 *************************************/
SHVBool SHVDataFactoryImpl::RegisterAlias(const SHVString8C& table, const SHVString8C& alias, bool clear, SHVDataSession* useSession)
{
SHVTransactionLocker lockTrans(*this, FactoryLock);
const SHVDataStructC* found;
SHVDataStructCRef aliasfound;
SHVStringUTF8 sql;
SHVStringSQLite rest("");
SHVBool retVal = SHVBool::True;
SHVBool dropOk;
bool create;
bool exists;
SHVSQLiteWrapperRef sqlite;
SHVSQLiteStatementRef statement;

	if (useSession)
	{
		sqlite = (SHVSQLiteWrapper*) useSession->GetProvider();
		ConnectionDirty = true;
	}
	else
		sqlite = SQLite;

	aliasfound = (SHVDataStructC*) InternalFindAlias(alias);
	found = InternalFindStruct(table);	
	if (found)
	{
		if (lockTrans.IsOk())
		{
			BeginTransaction(useSession);
		}
		create = !TableMatch(sqlite, found, alias, exists);
		if (create && exists)
		{
			if (aliasfound)
			{
				InternalUnregisterAlias(alias, lockTrans.IsOk());
				aliasfound = NULL;
			}
			else
			{
				sql.Format("drop table %s", alias);
				statement = sqlite->ExecuteUTF8(retVal, sql, rest);
				if (retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
					retVal = SHVBool::True;
			}
		}
		if (create && retVal)
		{
			retVal = CreateTable(sqlite, found, alias);
			if (retVal)
			{
				for (size_t key = 1; key < found->IndexCount() && retVal; key++)
				{
					retVal = CreateIndex(sqlite, found, alias, key);
				}
			}
		}
		if (clear && exists)
		{
			sql.Format("delete from %s", alias);
			statement = sqlite->ExecuteUTF8(retVal, sql, rest);
		}
		if (lockTrans.IsOk())
		{
			if (retVal)
				EndTransaction(useSession);
			else
				RollbackTransaction(useSession);
		}
		if (retVal && aliasfound.IsNull())
			Alias.AddTail(SHVDataStructReg(alias, found));
	}
	else
		retVal.SetError(SHVSQLiteWrapper::SQLite_MISUSE);
	return retVal;
}

/*************************************
 * UnregisterAlias
 *************************************/
SHVBool SHVDataFactoryImpl::UnregisterAlias(const SHVString8C& alias)
{
SHVTransactionLocker lockTrans(*this, FactoryLock);
	return InternalUnregisterAlias(alias, lockTrans.IsOk());
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
 * CreateSession
 *************************************/
SHVDataSession* SHVDataFactoryImpl::CreateSession()
{
SHVBool ok;
SHVSQLiteWrapperRef sqlite;
{
SHVTransactionLocker transLock(*this, FactoryLock);
	sqlite = DataEngine.CreateConnection(ok, Database);
}
SHVDataSession* retVal = new SHVDataSessionSQLite(DataEngine.GetModuleList(), sqlite, this);
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
 * GetErrorMessage
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
SHVString8 tableNameNoDB;
SHVStringSQLite rest(NULL);
SHVBool retVal = SHVBool::False;
SHVSQLiteStatementRef statement;
size_t tokPos = 0;
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
		tableName.Tokenize(".", tokPos);
		tableNameNoDB = tableName.Tokenize(".", tokPos);
		if (!tableNameNoDB.GetLength())
			tableNameNoDB = tableName;
		sql.Format("create index %s%05d on %s(%s)",
			tableName.GetSafeBuffer(),
			index,
			tableNameNoDB.GetSafeBuffer(),
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
 * InternalFindAlias
 *************************************/
const SHVDataStructC* SHVDataFactoryImpl::InternalFindAlias(const SHVString8C& table) const
{
const SHVDataStructC* found = NULL;
SHVListIterator<SHVDataStructReg> Iter(Alias);
	while (Iter.MoveNext() && (Iter.Get().GetDeleted() || Iter.Get().GetAlias() != table));
	if (Iter && !Iter.Get().GetDeleted())
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
 * InternalUnregisterAlias
 *************************************/
SHVBool SHVDataFactoryImpl::InternalUnregisterAlias(const SHVString8C& alias, bool hasLock)
{
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
	retVal = hasLock;

		while (DLIter.MoveNext() && retVal) 
		{
			retVal = !DLIter.Get()->AliasActive(alias);
		}	
		if (retVal)
		{
		SHVListPos al = PendingUnregisterAlias.Find(alias);
			if (al)
				PendingUnregisterAlias.RemoveAt(al);
			CheckConnection();
			sql.Format("drop table %s", alias);
			SQLite->ExecuteUTF8(retVal, sql, rest)->ValidateRefCount();
			if (retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
			{
				while (Iter.MoveNext() && Iter.Get().GetAlias() != alias);
				if (Iter)
				{
					Iter.Get().SetDeleted(true);
				}
				retVal = SHVBool::True;
			}
		}
		else
		{
			if (hasLock)
			{
				if (!PendingUnregisterAlias.Find(alias))
					PendingUnregisterAlias.AddTail(alias);
			}
			else
			{
				if (!PendingUnregisterAliasTransaction.Find(alias))
					PendingUnregisterAliasTransaction.AddTail(alias);
			}
			retVal = SHVBool(SHVSQLiteWrapper::SQLite_LOCKED);
		}
	}
	return retVal;
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
 * CheckAlias
 *************************************/
bool SHVDataFactoryImpl::CheckAlias(SHVDataSession* session, const SHVString8C& alias)
{
SHVTransactionLocker lockTrans(*this, FactoryLock);
bool retVal = PendingUnregisterAlias.Find(alias) != NULL;
SHVSQLiteWrapperRef sqlite;
	if (retVal)
	{
		if (session)
			sqlite = (SHVSQLiteWrapper*) session->GetProvider();
		else
			sqlite = SQLite;
		CheckConnection();
		if (lockTrans.IsOk())
		{
			BeginTransaction(NULL);
		}
		retVal = InternalUnregisterAlias(alias, lockTrans.IsOk());
		if (lockTrans.IsOk())
		{
			EndTransaction(NULL);
		}
	}
	else
		retVal = true;
	return retVal;
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
 * LockTransaction
 *************************************/
bool SHVDataFactoryImpl::LockTransaction()
{
	TransactionLock.Lock();
	if (TransactionLock.GetLockCount() > 1)
	{
		TransactionLock.Unlock();
		return false;
	}
	return true;
}

/*************************************
 * UnlockTransaction
 *************************************/
void SHVDataFactoryImpl::UnlockTransaction()
{
	TransactionLock.Unlock();
}

/*************************************
 * BeginTransaction
 *************************************/
SHVBool SHVDataFactoryImpl::BeginTransaction(SHVDataSession* session)
{
SHVStringSQLite rest(NULL);
SHVBool ok;
SHVSQLiteWrapperRef sqlite;

	sqlite = (session ? (SHVSQLiteWrapper*) session->GetProvider() : SQLite);	
	sqlite->ExecuteUTF8(ok, "BEGIN TRANSACTION", rest)->ValidateRefCount();
	return ok;
}

/*************************************
 * EndTransaction
 *************************************/
SHVBool SHVDataFactoryImpl::EndTransaction(SHVDataSession* session)
{
SHVStringSQLite rest(NULL);
SHVBool ok;
SHVSQLiteWrapperRef sqlite = (session ? (SHVSQLiteWrapper*) session->GetProvider() : SQLite);
SHVListPos p;
bool more;

	sqlite->ExecuteUTF8(ok, "END TRANSACTION", rest)->ValidateRefCount();
	if (ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
	{
		while (PendingUnregisterAliasTransaction.GetCount())
			InternalUnregisterAlias(PendingUnregisterAliasTransaction.PopHead(), true);
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
	}
	return ok;
}

/*************************************
 * RollbackTransaction
 *************************************/
SHVBool SHVDataFactoryImpl::RollbackTransaction(SHVDataSession* session)
{
SHVStringSQLite rest(NULL);
SHVBool ok;
SHVSQLiteWrapperRef sqlite;
SHVListPos p;
bool more;

	sqlite = (session ? (SHVSQLiteWrapper*) session->GetProvider() : SQLite);
    sqlite->ExecuteUTF8(ok, "ROLLBACK TRANSACTION", rest)->ValidateRefCount();
	if (ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
	{
		while (PendingUnregisterAliasTransaction.GetCount())
			InternalUnregisterAlias(PendingUnregisterAliasTransaction.PopHead(), true);
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
	return ok;
}

/*************************************
 * Reopen
 *************************************/
void SHVDataFactoryImpl::CheckConnection()
{
	if (ConnectionDirty)
	{
		SQLite = DataEngine.CreateConnection(Ok, Database);
		ConnectionDirty = false;
	}
}
