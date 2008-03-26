#include "stdafx.h"
#include "../../include/platformspc.h"

#include "../include/sqlitewrapperimpl.h"
#include "../include/sqlitestatementimpl.h"

SHVSQLiteWrapperImpl::SHVSQLiteWrapperImpl(void)
{
	Sqlite = NULL;
}

SHVSQLiteWrapperImpl::~SHVSQLiteWrapperImpl(void)
{
SHVMutexLocker lock(Lock);
	if (Sqlite)
	{
		sqlite3_close(Sqlite);
		Sqlite = NULL;
	}
}

SHVBool SHVSQLiteWrapperImpl::OpenUTF8(const SHVStringUTF8C& fileName, int option)
{
SHVMutexLocker lock(Lock);
	if (Sqlite)
	{
		sqlite3_close(Sqlite);
	}
	return SHVBool(sqlite3_open_v2(fileName.GetSafeBuffer(), &Sqlite, option, NULL));
}

SHVBool SHVSQLiteWrapperImpl::OpenInMemory()
{
SHVMutexLocker lock(Lock);
	if (Sqlite)
	{
		sqlite3_close(Sqlite);
	}
	return SHVBool(sqlite3_open(":memory", &Sqlite));
}

SHVBool SHVSQLiteWrapperImpl::Close()
{
SHVMutexLocker lock(Lock);
	if (Sqlite)
	{
		short res = (short) sqlite3_close(Sqlite);
		Sqlite = NULL;
		return SHVBool(res);
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);
}

SHVSQLiteStatement* SHVSQLiteWrapperImpl::PrepareUTF8(SHVBool& ok, const SHVStringUTF8C& sql, SHVStringSQLite& notparsed)
{
SHVMutexLocker lock(Lock);
sqlite3_stmt* sqlite_statement;
int res = SHVSQLiteWrapper::SQLite_ERROR;;
const char* rest = NULL;
SHVSQLiteStatement* retVal = NULL;
	if (Sqlite)
	{
		ok = SHVBool(sqlite3_prepare_v2(Sqlite, sql.GetSafeBuffer(), -1, &sqlite_statement, &rest));
		notparsed = rest;
		retVal = new SHVSQLiteStatementImpl(sqlite_statement, this, sqlite3_data_count(sqlite_statement));
	}
	return retVal;
} 

SHVSQLiteStatement* SHVSQLiteWrapperImpl::ExecuteUTF8(SHVBool& ok, const SHVStringUTF8C& sql, SHVStringSQLite& notparsed)
{
SHVMutexLocker lock(Lock);
sqlite3_stmt* sqlite_statement;
int res = SHVSQLiteWrapper::SQLite_ERROR;;
SHVSQLiteStatement* retVal = NULL;
const char* rest = NULL;
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

SHVStringUTF8C SHVSQLiteWrapperImpl::GetErrorMsgUTF8()
{
SHVMutexLocker lock(Lock);
	if (Sqlite)
		return sqlite3_errmsg(Sqlite);
	else
		return "Database is not open";
}

SHVMutex& SHVSQLiteWrapperImpl::GetMutex()
{
	return Lock;
}