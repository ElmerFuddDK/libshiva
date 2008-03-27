#include "stdafx.h"

#include "../../../../include/platformspc.h"
#include "../../include/shvdatasessionsqlite.h"
#include "../../include/shvdataengineimpl.h"
#include "../../include/shvdatafactoryimpl.h"


/*************************************
 * Constructor
 *************************************/
SHVDataEngine_impl::SHVDataEngine_impl(SHVModuleList& modules): SHVDataEngine(modules)
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
		Factory = new SHVDataFactory_impl(*this, datapath);
		if (!Factory->IsOk())
		{
			Modules.AddStartupError(Factory->GetErrorMessage());
		}
	}
}

SHVDataEngine_impl::~SHVDataEngine_impl()
{
	Factory = NULL;
}

/*************************************
 * Register
 *************************************/
SHVBool SHVDataEngine_impl::Register()
{
	return SHVModule::Register();
}

/*************************************
 * Unregister
 *************************************/
void SHVDataEngine_impl::Unregister()
{
	SHVModule::Unregister();
}

/*************************************
 * RegisterTable
 *************************************/
SHVBool SHVDataEngine_impl::RegisterTable(const SHVDataStructC* dataStruct, bool createTable)
{
	return Factory->RegisterTable(dataStruct, createTable);
}

/*************************************
 * RegisterAlias
 *************************************/
SHVBool SHVDataEngine_impl::RegisterAlias(const SHVString8C& table, const SHVString8C& alias, bool clear, SHVDataSession* useSession)
{
	return Factory->RegisterAlias(table, alias, clear, useSession);
}

/*************************************
 * UnregisterAlias
 *************************************/
SHVBool SHVDataEngine_impl::UnregisterAlias(const SHVString8C& alias)
{
	return Factory->UnregisterAlias(alias);
}


/*************************************
 * FindStruct
 *************************************/
const SHVDataStructC* SHVDataEngine_impl::FindStruct(const SHVString8C& table) const
{
	return Factory->FindStruct(table);
}

/*************************************
 * CreateSession
 *************************************/
SHVDataSession* SHVDataEngine_impl::CreateSession()
{
	return Factory->CreateSession();
}

/*************************************
 * CreateStruct
 *************************************/
SHVDataStruct* SHVDataEngine_impl::CreateStruct() const
{
	return Factory->CreateStruct();
}

/*************************************
 * CreateVariant
 *************************************/
SHVDataVariant* SHVDataEngine_impl::CreateVariant() const
{
	return Factory->CreateVariant();
}
/*************************************
 * CreateKey
 *************************************/
SHVDataRowKey* SHVDataEngine_impl::CreateKey() const
{
	return Factory->CreateKey();
}

/*************************************
 * CopyKey
 *************************************/
SHVDataRowKey* SHVDataEngine_impl::CopyKey(const SHVDataRowKey* key) const
{
	return Factory->CopyKey(key);
}
/*************************************
 * BuildKeySQL
 *************************************/
void SHVDataEngine_impl::BuildKeySQL(const SHVDataRowKey* key, SHVString8& condition, SHVString8& orderby, const SHVString8C& table, bool reverse) const
{
	return Factory->BuildKeySQL(key, condition, orderby, table, reverse);
}

/*************************************
 * GetDataSchema
 *************************************/
const SHVDataSchema& SHVDataEngine_impl::GetDataSchema() const
{
	return Factory->GetDataSchema();
}

/*************************************
 * GetDatabase
 *************************************/
const SHVStringC& SHVDataEngine_impl::GetDatabase() const
{
	return Factory->GetDatabase();
}

/*************************************
 * CreateConnection
 *************************************/
SHVSQLiteWrapper* SHVDataEngine_impl::CreateConnection(SHVBool& Ok, const SHVStringC& dataBase)
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
SHVDataFactory* SHVDataEngine_impl::CreateFactory(const SHVStringC& database, const SHVDataSchema* schema)
{
	return new SHVDataFactory_impl(*this, database, schema);
}

/*************************************
 * GetDefaultFactory
 *************************************/
SHVDataFactory* SHVDataEngine_impl::GetDefaultFactory()
{
	return Factory;
}

/*************************************
 * SubscribeRowChange
 *************************************/
void SHVDataEngine_impl::SubscribeRowChange(SHVEventSubscriberBase* sub)
{
}

/*************************************
 * RegisterDataList
 *************************************/
void SHVDataEngine_impl::RegisterDataList(SHVDataRowListC* rowList)
{
}

/*************************************
 * UnregisterDataList
 *************************************/
void SHVDataEngine_impl::UnregisterDataList(SHVDataRowListC* rowList)
{
}

/*************************************
 * RowChanged
 *************************************/
void SHVDataEngine_impl::RowChanged(SHVDataRow* row)
{
	EmitEvent(new SHVEventDataRowChanged(row, this, SHVDataFactory::EventRowChanged, SHVInt(), Factory));
}
