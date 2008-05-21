#include "stdafx.h"
#include "../../include/platformspc.h"

#include "../include/sqlitewrapperimpl.h"
#include "../include/sqlitestatementimpl.h"

/*************************************
 * Constructor
 *************************************/
SHVSQLiteWrapperImpl::SHVSQLiteWrapperImpl(void)
{
	OwnerThread = SHVThreadBase::GetCurrentThreadID();
	Sqlite = NULL;
}

/*************************************
 * Destructor
 *************************************/
SHVSQLiteWrapperImpl::~SHVSQLiteWrapperImpl(void)
{
	SHVTHREADCHECK(OwnerThread);
	if (Sqlite)
	{
		sqlite3_close(Sqlite);
		Sqlite = NULL;
	}
}

/*************************************
 * OpenUTF8
 *************************************/
SHVBool SHVSQLiteWrapperImpl::OpenUTF8(const SHVStringUTF8C& fileName, int option)
{
	SHVTHREADCHECK(OwnerThread);
	if (Sqlite)
	{
		sqlite3_close(Sqlite);
	}
	return SHVBool(sqlite3_open_v2(fileName.GetSafeBuffer(), &Sqlite, option, NULL));
}

/*************************************
 * OpenInMemory
 *************************************/
SHVBool SHVSQLiteWrapperImpl::OpenInMemory()
{
	SHVTHREADCHECK(OwnerThread);
	if (Sqlite)
	{
		sqlite3_close(Sqlite);
	}
	return SHVBool(sqlite3_open(":memory", &Sqlite));
}

/*************************************
 * Close
 *************************************/
SHVBool SHVSQLiteWrapperImpl::Close()
{
	SHVTHREADCHECK(OwnerThread);
	if (Sqlite)
	{
		short res = (short) sqlite3_close(Sqlite);
		Sqlite = NULL;
		return SHVBool(res);
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);
}

/*************************************
 * PrepareUTF8
 *************************************/
SHVSQLiteStatement* SHVSQLiteWrapperImpl::PrepareUTF8(SHVBool& ok, const SHVStringUTF8C& sql, SHVStringSQLite& notparsed)
{
sqlite3_stmt* sqlite_statement;
int res = SHVSQLiteWrapper::SQLite_ERROR;;
const char* rest = NULL;
SHVSQLiteStatement* retVal = NULL;
	
	SHVTHREADCHECK(OwnerThread);
	if (Sqlite)
	{
		ok = SHVBool(sqlite3_prepare_v2(Sqlite, sql.GetSafeBuffer(), -1, &sqlite_statement, &rest));
		notparsed = rest;
		retVal = new SHVSQLiteStatementImpl(sqlite_statement, this, sqlite3_data_count(sqlite_statement));
	}
	return retVal;
} 

/*************************************
 * ExecuteUTF8
 *************************************/
SHVSQLiteStatementRef SHVSQLiteWrapperImpl::ExecuteUTF8(SHVBool& ok, const SHVStringUTF8C& sql, SHVStringSQLite& notparsed)
{
SHVTHREADCHECK(OwnerThread);
sqlite3_stmt* sqlite_statement;
int res = SHVSQLiteWrapper::SQLite_ERROR;;
SHVSQLiteStatement* retVal = NULL;
const char* rest = NULL;
	
	SHVTHREADCHECK(OwnerThread);
	if (Sqlite)
	{
		ok = SHVBool(sqlite3_prepare_v2(Sqlite, sql.GetSafeBuffer(), -1, &sqlite_statement, &rest));
		if (ok)
		{
			retVal = new SHVSQLiteStatementImpl(sqlite_statement, this, sqlite3_data_count(sqlite_statement));
			ok = SHVBool(retVal->NextResult().GetError());
		}
		notparsed = rest;
	}
	return retVal;
} 

/*************************************
 * GetErrorMsgUTF8
 *************************************/
SHVStringUTF8C SHVSQLiteWrapperImpl::GetErrorMsgUTF8()
{
	SHVTHREADCHECK(OwnerThread);
	if (Sqlite)
		return sqlite3_errmsg(Sqlite);
	else
		return "Database is not open";
}

