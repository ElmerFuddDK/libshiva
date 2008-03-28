#include "stdafx.h"

#include "../../../include/platformspc.h"
#include "../include/shvdatasessionsqlite.h"
#include "../include/shvdataengineimpl.h"
#include "../include/shvdatafactoryimpl.h"


/*************************************
 * Constructor
 *************************************/
SHVDataEngineImpl::SHVDataEngineImpl(SHVModuleList& modules): SHVDataEngine(modules)
{
SHVString database = modules.GetConfig().Find(__DATAENGINE_DEFAULT_DATABASE, _T("database.db")).ToString();
SHVString datapath = modules.GetConfig().Find(__DATAENGINE_DATAPATH, _T("./")).ToString();
SHVBool ok;
	datapath += database;
	
	ok = SQLiteDll.Load(SQLiteDll.CreateLibFileName(_T("shivasqlite")));
	if (!ok)
	{
		Modules.AddStartupError(_T("Could not load shvsqlite.dll"));
	}
	else
	{
		Factory = new SHVDataFactoryImpl(*this, datapath);
		if (!Factory->IsOk())
		{
			Modules.AddStartupError(Factory->GetErrorMessage());
		}
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
SHVBool SHVDataEngineImpl::RegisterTable(const SHVDataStructC* dataStruct, bool createTable)
{
	return Factory->RegisterTable(dataStruct, createTable);
}

/*************************************
 * RegisterAlias
 *************************************/
SHVBool SHVDataEngineImpl::RegisterAlias(const SHVString8C& table, const SHVString8C& alias, bool clear, SHVDataSession* useSession)
{
	return Factory->RegisterAlias(table, alias, clear, useSession);
}

/*************************************
 * UnregisterAlias
 *************************************/
SHVBool SHVDataEngineImpl::UnregisterAlias(const SHVString8C& alias)
{
	return Factory->UnregisterAlias(alias);
}


/*************************************
 * FindStruct
 *************************************/
const SHVDataStructC* SHVDataEngineImpl::FindStruct(const SHVString8C& table) const
{
	return Factory->FindStruct(table);
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
 * BuildKeySQL
 *************************************/
void SHVDataEngineImpl::BuildKeySQL(const SHVDataRowKey* key, SHVString8& condition, SHVString8& orderby, const SHVString8C& table, bool reverse) const
{
	return Factory->BuildKeySQL(key, condition, orderby, table, reverse);
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
			retVal->ExecuteUTF8(Ok, "attach database :memory as memdb", rest)->ValidateRefCount();
		if (Ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
			Ok = SHVBool::True;
	}
	return retVal.ReleaseReference();
}

/*************************************
 * CreateFactory
 *************************************/
SHVDataFactory* SHVDataEngineImpl::CreateFactory(const SHVStringC& database, const SHVDataSchema* schema)
{
	return new SHVDataFactoryImpl(*this, database, schema);
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
