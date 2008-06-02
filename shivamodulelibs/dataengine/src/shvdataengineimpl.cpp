#include "stdafx.h"

#include "../../../include/platformspc.h"
#include "../include/shvdatasessionsqlite.h"
#include "../include/shvdataengineimpl.h"
#include "../include/shvdatafactoryimpl.h"
#include "../../../include/utils/shvdir.h"


/*************************************
 * Constructor
 *************************************/
SHVDataEngineImpl::SHVDataEngineImpl(SHVModuleList& modules): SHVDataEngine(modules)
{
SHVString database = modules.GetConfig().Find(__DATAENGINE_DEFAULT_DATABASE, _T("database.db")).ToString();
SHVString datapath = modules.GetConfig().Find(__DATAENGINE_DATAPATH, Modules.GetConfig().Find(SHVModuleList::DefaultCfgAppPath).ToString()).ToString();
SHVBool ok;

	datapath += SHVDir::Delimiter() + database;
	
	ok = SQLiteDll.Load(SQLiteDll.CreateLibFileName(_T("shivasqlite"),modules.GetConfig().Find(SHVModuleList::DefaultCfgAppPath).ToString()));
	if (!ok)
	{
		Modules.AddStartupError(_T("Could not load shvsqlite.dll"));
	}
	else
	{
		Factory = new SHVDataFactoryImpl(*this, datapath);
	}
}

SHVDataEngineImpl::~SHVDataEngineImpl()
{
	Factory = NULL;
}

/*************************************
 * Register
 *************************************/
SHVBool SHVDataEngineImpl::Register()
{
	return SHVModule::Register();
}

/*************************************
 * Unregister
 *************************************/
void SHVDataEngineImpl::Unregister()
{
	SHVModule::Unregister();
}

/*************************************
 * RegisterTable
 *************************************/
SHVBool SHVDataEngineImpl::RegisterTable(const SHVDataStructC* dataStruct, SHVDataSession* useSession)
{
	return Factory->RegisterTable(dataStruct, useSession);
}

/*************************************
 * RegisterAlias
 *************************************/
SHVBool SHVDataEngineImpl::RegisterAlias(const SHVString8C& table, const SHVString8C& alias, bool clear, SHVDataSession* useSession)
{
	return Factory->RegisterAlias(table, alias, clear, useSession);
}

/*************************************
 * RegisterIndex
 *************************************/
size_t SHVDataEngineImpl::RegisterIndex(const SHVString8C& table, SHVDataRowKey* IndexKey, SHVDataSession* useSession)
{
	return Factory->RegisterIndex(table, IndexKey, useSession);
}

/*************************************
 * UnregisterAlias
 *************************************/
SHVBool SHVDataEngineImpl::UnregisterAlias(const SHVString8C& alias, SHVDataSession* useSession)
{
	return Factory->UnregisterAlias(alias, useSession);
}

/*************************************
 * ClearTable
 *************************************/
SHVBool SHVDataEngineImpl::ClearTable(const SHVString8C &table, SHVDataSession *useSession)
{
	return Factory->ClearTable(table, useSession);
}

/*************************************
 * FindStruct
 *************************************/
const SHVDataStructC* SHVDataEngineImpl::FindStruct(const SHVString8C& table) const
{
	return Factory->FindStruct(table);
}

/*************************************
 * GetDataSchema
 *************************************/
const SHVDataSchema& SHVDataEngineImpl::GetDataSchema() const
{
	return Factory->GetDataSchema();
}

/*************************************
 * GetDatabase
 *************************************/
const SHVStringC& SHVDataEngineImpl::GetDatabase() const
{
	return Factory->GetDatabase();
}

/*************************************
 * CreateSession
 *************************************/
SHVDataSession* SHVDataEngineImpl::CreateSession()
{
	return Factory->CreateSession();
}

/*************************************
 * CreateStruct
 *************************************/
SHVDataStruct* SHVDataEngineImpl::CreateStruct() const
{
	return Factory->CreateStruct();
}

/*************************************
 * CreateVariant
 *************************************/
SHVDataVariant* SHVDataEngineImpl::CreateVariant() const
{
	return Factory->CreateVariant();
}

/*************************************
 * CreateKey
 *************************************/
SHVDataRowKey* SHVDataEngineImpl::CreateKey() const
{
	return Factory->CreateKey();
}

/*************************************
 * CopyKey
 *************************************/
SHVDataRowKey* SHVDataEngineImpl::CopyKey(const SHVDataRowKey* key) const
{
	return Factory->CopyKey(key);
}

/*************************************
 * RetrieveStruct
 *************************************/
SHVDataStructC* SHVDataEngineImpl::RetrieveStruct(const SHVString8C table, SHVDataSession* useSession) const
{
	return Factory->RetrieveStruct(table, useSession);
}

/*************************************
 * BuildKeySQL
 *************************************/
void SHVDataEngineImpl::BuildKeySQL(const SHVDataRowKey* key, SHVString8& condition, SHVString8& orderby, const SHVString8C& table, bool reverse) const
{
	return Factory->BuildKeySQL(key, condition, orderby, table, reverse);
}

/*************************************
 * CreateConnection
 *************************************/
SHVSQLiteWrapper* SHVDataEngineImpl::CreateConnection(SHVBool& Ok, const SHVStringC& dataBase)
{
SHVSQLiteWrapperRef retVal = (SHVSQLiteWrapper*) SQLiteDll.CreateObjectInt(&Modules, SHVDll::ClassTypeUser);
	Ok = retVal->Open(dataBase);
	if (Ok)
	{
	SHVStringSQLite rest("");
	// Lets setup a default memory database
		if (dataBase != _T(":memory"))
			retVal->ExecuteUTF8(Ok, "attach database :memory as memdb", rest);
		if (Ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
		{
			retVal->ExecuteUTF8(Ok, "create table if not exists shv_alias(id integer primary key, alias varchar(200))", rest);
			if (Ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
				Ok = SHVBool::True;
		}		
	}
	return retVal.ReleaseReference();
}

/*************************************
 * CreateFactory
 *************************************/
SHVDataFactory* SHVDataEngineImpl::CreateFactory(const SHVStringC& database)
{
	return new SHVDataFactoryImpl(*this, database);
}

/*************************************
 * GetDefaultFactory
 *************************************/
SHVDataFactory* SHVDataEngineImpl::GetDefaultFactory()
{
	return Factory;
}

/*************************************
 * SubscribeRowChange
 *************************************/
void SHVDataEngineImpl::SubscribeRowChange(SHVEventSubscriberBase* sub)
{
}

/*************************************
 * RegisterDataList
 *************************************/
void SHVDataEngineImpl::RegisterDataList(SHVDataRowListC* rowList)
{
}

/*************************************
 * UnregisterDataList
 *************************************/
void SHVDataEngineImpl::UnregisterDataList(SHVDataRowListC* rowList)
{
}

/*************************************
 * RowChanged
 *************************************/
void SHVDataEngineImpl::RowChanged(SHVDataRow* row)
{
	EmitEvent(new SHVEventDataRowChanged(row, this, SHVDataFactory::EventRowChanged, SHVInt(), Factory));
}
