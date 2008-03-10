#include "stdafx.h"

#include "../../../../include/platformspc.h"
#include "../../../../include/utils/shvstringutf8.h"
#include "../../../../include/threadutils/shvmutexlocker.h"
#include "../../include/dataengineimpl/shvdatafactory_impl.h"
#include "../../include/dataengineimpl/shvdatasession_sqlite.h"
#include "../../include/dataengineimpl/shvdatarowlistc_sqlite.h"
#include "../../include/dataengineimpl/shvdatastruct_impl.h"
#include "../../include/dataengineimpl/shvdatastructc_sqlite.h"
#include "../../include/dataengineimpl/shvdatavariant_impl.h"
#include "../../include/dataengineimpl/shvdatarowkey_impl.h"

/*************************************
 * Constructor
 *************************************/
SHVDataFactory_impl::SHVDataFactory_impl(SHVDataEngine& engine, const SHVStringC& database): DataEngine(engine), Database(database)
{
	SQLite = DataEngine.CreateConnection(Ok, database);
}

SHVDataFactory_impl::SHVDataFactory_impl(SHVDataEngine& engine, const SHVStringC& database, const SHVDataSchema* schema): DataEngine(engine), Database(database)
{
	SQLite = DataEngine.CreateConnection(Ok, database);
	if (schema)
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
SHVBool SHVDataFactory_impl::RegisterTable(const SHVDataStructC* dataStruct)
{
const SHVDataStructC* found;
bool create = false;
bool drop   = false;
SHVStringSQLite rest(NULL);
SHVStringUTF8 sql;
SHVSQLiteStatementRef statement;
SHVBool retVal = SHVBool::True;
SHVMutexLocker lock(FactoryLock);

	found = FindStruct(dataStruct->GetTableName());
	if (found)
	{
		drop = create = dataStruct->IsEqual(found);
	}
	else
	{
	SHVDataStructCRef sqliteStruct = new SHVDataStructC_SQLite(SQLite, dataStruct->GetTableName());
		drop = sqliteStruct->GetColumnCount() != 0;
		create = !dataStruct->IsEqual(sqliteStruct);
		if (!create)
			drop = false;
	}
	if (drop)
	{
		sql.Format("DROP TABLE %s", dataStruct->GetTableName());
		statement = SQLite->ExecuteUTF8(retVal, sql, rest);
		retVal = SHVBool(retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE);
	}
	if (retVal && create) 
	{
		retVal = CreateTable(dataStruct);
		if (retVal)
		{
			for (size_t key = 1; key < dataStruct->IndexCount() && retVal; key++)
			{
				retVal = CreateIndex(dataStruct, key);
			}
			if (!retVal)
			{
			SHVBool dropOk;
				sql.Format("DROP TABLE %s", dataStruct->GetTableName());
				statement = SQLite->ExecuteUTF8(dropOk, sql, rest);
			}
		}
	}
	if (retVal && !found)
		Schema.Add((SHVDataStructC*) dataStruct);
	return retVal;
}

/*************************************
 * FindStruct
 *************************************/
const SHVDataStructC* SHVDataFactory_impl::FindStruct(const SHVString8C& table) const
{
const SHVDataStructC* found = NULL;
SHVMutexLocker lock(FactoryLock);
	for(size_t t = Schema.CalculateCount(); t && !found;)
	{
		if (Schema[--t]->GetTableName() == table)
			found = Schema[t];
	}
	return found;
}

/*************************************
 * GetDataSchema
 *************************************/
const SHVDataSchema& SHVDataFactory_impl::GetDataSchema() const
{
	return Schema;
}

/*************************************
 * CreateStruct
 *************************************/
SHVDataStruct* SHVDataFactory_impl::CreateStruct() const
{
	return new SHVDataStruct_impl();
}

/*************************************
 * CreateVariant
 *************************************/
SHVDataVariant* SHVDataFactory_impl::CreateVariant() const
{
	return new SHVDataVariant_impl();
}
/*************************************
 * CreateKey
 *************************************/
SHVDataRowKey* SHVDataFactory_impl::CreateKey() const 
{
	return new SHVDataRowKey_impl();
}

/*************************************
 * CopyKey
 *************************************/
SHVDataRowKey* SHVDataFactory_impl::CopyKey(const SHVDataRowKey* key) const 
{
	return new SHVDataRowKey_impl(key);
}

/*************************************
 * GetDatabase
 *************************************/
const SHVStringC& SHVDataFactory_impl::GetDatabase() const
{
	return Database;
}

/*************************************
 * BuildKeySQL
 *************************************/
void SHVDataFactory_impl::BuildKeySQL(const SHVDataRowKey* key, SHVString8& condition, SHVString8& orderby, const SHVString8C& table, bool reverse) const
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
void SHVDataFactory_impl::SubscribeRowChange(SHVEventSubscriberBase* sub)
{
	DataChangedSubscription = sub;
}

/*************************************
 * GetDataEngine
 *************************************/
SHVDataEngine& SHVDataFactory_impl::GetDataEngine()
{
	return DataEngine;
}

/*************************************
 * GetDataEngine
 *************************************/
SHVStringBuffer SHVDataFactory_impl::GetErrorMessage() const
{
	return SQLite->GetErrorMsg();
}

/*************************************
 * IsOk
 *************************************/
SHVBool SHVDataFactory_impl::IsOk() const
{
	return Ok;
}

/*************************************
 * Destructor
 *************************************/
SHVDataFactory_impl::~SHVDataFactory_impl()
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
SHVDataSession* SHVDataFactory_impl::CreateSession()
{
SHVDataSession* retVal = new SHVDataSession_SQLite(DataEngine.GetModuleList(), SQLite, this);
	RegisterDataSession(retVal);
	return retVal;
}

/*************************************
 * CreateTable
 *************************************/
SHVBool SHVDataFactory_impl::CreateTable(const SHVDataStructC* dataStruct)
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
	query.Format("create table %s(%s,primary key (%s), unique(%s))", dataStruct->GetTableName().GetSafeBuffer(), cols.GetSafeBuffer(), pkey.GetSafeBuffer(), pkey.GetSafeBuffer());
	
	SHVSQLiteStatementRef statement = SQLite->ExecuteUTF8(ok, query, notparsed);
	return SHVBool(ok.GetError() == SHVSQLiteWrapper::SQLite_DONE);
}

/*************************************
 * CreateIndex
 *************************************/
SHVBool SHVDataFactory_impl::CreateIndex(const SHVDataStructC* dataStruct, size_t index)
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
			dataStruct->GetTableName().GetSafeBuffer(),
			index,
			dataStruct->GetTableName().GetSafeBuffer(),
			keys);
		statement = SQLite->ExecuteUTF8(retVal, sql, rest);
		retVal = SHVBool(retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE);
	}
	return retVal;
}
/*************************************
 * SetSQLite
 *************************************/
void SHVDataFactory_impl::SetSQLite(SHVSQLiteWrapper* sqlite)
{
	SQLite = sqlite;
}

/*************************************
 * RegisterDataList
 *************************************/
void SHVDataFactory_impl::RegisterDataList(SHVDataRowListC* rowList)
{
SHVMutexLocker lock(FactoryLock);
	ActiveDataLists.AddTail(rowList);
}

/*************************************
 * UnregisterDataList
 *************************************/
void SHVDataFactory_impl::UnregisterDataList(SHVDataRowListC* rowList)
{
SHVMutexLocker lock(FactoryLock);
SHVListPos pos = ActiveDataLists.Find(rowList);
	if (pos)
		ActiveDataLists.RemoveAt(pos);
}

/*************************************
 * RegisterDataSession
 *************************************/
void SHVDataFactory_impl::RegisterDataSession(SHVDataSession* session)
{
SHVMutexLocker lock(FactoryLock);
	ActiveSessions.AddTail(session);
}

/*************************************
 * UnregisterDataSession
 *************************************/
void SHVDataFactory_impl::UnregisterDataSession(SHVDataSession* session)
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
void SHVDataFactory_impl::RowChanged(SHVDataRow* row)
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
 * SessionReset
 *************************************/
SHVBool SHVDataFactory_impl::SessionReset(SHVDataSession* session)
{
SHVMutexLocker lock(FactoryLock);
SHVBool retVal = SHVBool::True;
SHVListIterator<SHVDataRowListC*> iter(ActiveDataLists);
	while (retVal && iter.MoveNext())
	{
		if (iter.Get()->GetDataSession() == session)
			retVal =  DataListTempReset(iter.Get());
	}
	return retVal;
}

/*************************************
 * SessionReposition
 *************************************/
void SHVDataFactory_impl::SessionReposition(SHVDataSession* session)
{
SHVMutexLocker lock(FactoryLock);
SHVListIterator<SHVDataRowListC*> iter(ActiveDataLists);
	while (iter.MoveNext())
	{
		if (iter.Get()->GetDataSession() == session)
			DataListReposition(iter.Get());
	}
}
