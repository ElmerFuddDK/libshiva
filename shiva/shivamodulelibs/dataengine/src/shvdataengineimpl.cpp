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
#include "../include/shvdatasessionsqlite.h"
#include "../include/shvdataengineimpl.h"
#include "../include/shvdatafactoryimpl.h"
#include "../../../include/utils/shvdir.h"

#ifdef SHIVASTATICMODULELIB

#include "../../../shivasqlite/include/sqlitewrapperimpl.h"

#endif

/*************************************
 * Constructor
 *************************************/
SHVDataEngineImpl::SHVDataEngineImpl(SHVModuleList& modules): SHVDataEngine(modules)
{
SHVString database = modules.GetConfig().Find(__DATAENGINE_DEFAULT_DATABASE, _S("database.db"))->ToString();
SHVString datapath = modules.GetConfig().Find(__DATAENGINE_DATAPATH, Modules.GetConfig().Find(SHVModuleList::DefaultCfgAppPath)->ToString())->ToString();
SHVBool ok;
SHVString driverPath;

	if (database == _S(":memory:"))
		datapath = _S("file:memory:?cached=shared");
	else
		datapath += SHVDir::Delimiter() + database;

#ifndef SHIVASTATICMODULELIB

	driverPath = SQLiteDll.CreateLibFileName(_S("shivasqlite"),modules.GetConfig().Find(SHVModuleList::DefaultCfgAppPath)->ToString());

	ok = SQLiteDll.Load(driverPath);
	if (!ok)
	{
		Modules.AddStartupError(SHVStringC::Format(_S("Could not load %s"), driverPath.GetSafeBuffer()));
	}
	else
	{
		Factory = new SHVDataFactoryImpl(*this, datapath);
	}
#else
	Factory = new SHVDataFactoryImpl(*this, datapath);
#endif
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
SHVBool SHVDataEngineImpl::RegisterTable(const SHVDataStructC* dataStruct, SHVDataSession* useSession, bool strict)
{
	return Factory->RegisterTable(dataStruct, useSession, strict);
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
 * DropAlias
 *************************************/
SHVBool SHVDataEngineImpl::DropAlias(const SHVString8C& table, const SHVString8C& alias, SHVDataSession* useSession)
{
	return Factory->DropAlias(table, alias, useSession);
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
 * ReloadStruct
 *************************************/
const SHVDataStructC* SHVDataEngineImpl::ReloadStruct(const SHVString8C& table, SHVDataSession* useSession)
{
	return Factory->ReloadStruct(table, useSession);
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
 * CopyStruct
 *************************************/
SHVDataStruct* SHVDataEngineImpl::CopyStruct(const SHVDataStructC *struc)
{
	return Factory->CopyStruct(struc);
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
//SHVSQLiteWrapperRef retVal = (SHVSQLiteWrapper*) SQLiteDll.CreateObjectInt(&Modules, SHVDll::ClassTypeUser);
SHVSQLiteWrapperRef retVal;

#ifdef SHIVASTATICMODULELIB
	retVal = new SHVSQLiteWrapperImpl();
#else
	retVal = (SHVSQLiteWrapper*) SQLiteDll.CreateObjectInt(&Modules, SHVDll::ClassTypeUser);
#endif

	Ok = retVal->Open(dataBase);
	if (Ok)
	{
	SHVStringSQLite rest("");
	// Lets setup a default memory database
		if (dataBase.Find(_S(":memory:")) == -1)
			retVal->ExecuteUTF8(Ok, "attach database ':memory:' as memdb", rest);
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
	Factory->SubscribeRowChange(sub);
}

/*************************************
 * RowChanged
 *************************************/
void SHVDataEngineImpl::RowChanged(SHVDataRow* row)
{
	EmitEvent(new SHVEventDataRowChanged(row, this, SHVDataFactory::EventRowChanged, SHVInt(), Factory));
}

/*************************************
 * BeginningTransaction
 *************************************/
void SHVDataEngineImpl::BeginningTransaction()
{
	EmitEvent(new SHVEvent(this, SHVDataEngine::EventBeginningTransaction));
}

/*************************************
 * FinishedTransaction
 *************************************/
void SHVDataEngineImpl::FinishedTransaction(bool committed)
{
	EmitEvent(new SHVEvent(this, SHVDataEngine::EventFinishedTransaction,SHVInt(committed ? 1 : 0)));
}
