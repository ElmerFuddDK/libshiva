#include "stdafx.h"

#include "../../../../include/platformspc.h"
#include "../../include/dataengineimpl/shvdatasession_sqlite.h"
#include "../../include/dataengineimpl/shvdataengine_impl.h"
#include "../../include/dataengineimpl/shvdatafactory_impl.h"


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
	SHVSQLiteWrapperRef sqlite = CreateConnection(ok, datapath);
		if (!ok)
		{
			Modules.AddStartupError(sqlite->GetErrorMsg());
		}
		else
			Factory = new SHVDataFactory_impl(*this, sqlite, datapath);
	}
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
SHVBool SHVDataEngine_impl::RegisterTable(const SHVDataStructC* dataStruct)
{
	return Factory->RegisterTable(dataStruct);
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
 * BuildKeySQL
 *************************************/
void SHVDataEngine_impl::BuildKeySQL(const SHVDataRowKey* key, SHVString8& condition, SHVString8& orderby, bool reverse) const
{
	return Factory->BuildKeySQL(key, condition, orderby, reverse);
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
SHVSQLiteWrapper* retVal = (SHVSQLiteWrapper*) SQLiteDll.CreateObjectInt(&Modules, SHVDll::ClassTypeUser);
	Ok = retVal->Open(dataBase);
	return retVal;
}

SHVDataFactory* SHVDataEngine_impl::CreateFactory(const SHVString& database, const SHVDataSchema* schema)
{
SHVBool ok;
SHVSQLiteWrapper* sqlite = CreateConnection(ok, database);
	if (ok)
		return new SHVDataFactory_impl(*this, sqlite, database, schema);
	else
		return NULL;
}

