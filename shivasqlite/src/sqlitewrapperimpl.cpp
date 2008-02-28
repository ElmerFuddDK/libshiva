#include "StdAfx.h"
#include "../../../include/platformspc.h"

#include "../../include/sqliteimpl/sqlitewrapper_impl.h"
#include "../../include/sqliteimpl/sqlitestatement_impl.h"

SHVSQLiteWrapper_Impl::SHVSQLiteWrapper_Impl(void)
{
	Sqlite = NULL;
}

SHVSQLiteWrapper_Impl::~SHVSQLiteWrapper_Impl(void)
{
SHVMutexLocker lock(Lock);
	if (Sqlite)
	{
		sqlite3_close(Sqlite);
		Sqlite = NULL;
	}
}

SHVBool SHVSQLiteWrapper_Impl::OpenUTF8(const SHVStringUTF8C& fileName, int option)
{
SHVMutexLocker lock(Lock);
	if (Sqlite)
	{
		sqlite3_close(Sqlite);
	}
	return SHVBool(sqlite3_open_v2(fileName.GetSafeBuffer(), &Sqlite, option, NULL));
}

SHVBool SHVSQLiteWrapper_Impl::OpenInMemory()
{
SHVMutexLocker lock(Lock);
	if (Sqlite)
	{
		sqlite3_close(Sqlite);
	}
	return SHVBool(sqlite3_open(":memory", &Sqlite));
}

SHVBool SHVSQLiteWrapper_Impl::Close()
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

SHVSQLiteStatement* SHVSQLiteWrapper_Impl::PrepareUTF8(SHVBool& ok, const SHVStringUTF8C& sql, SHVStringSQLite& notparsed)
{
SHVMutexLocker lock(Lock);
sqlite3_stmt* sqllite_statement;
int res = SHVSQLiteWrapper::SQLite_ERROR;;
const char* rest = NULL;
SHVSQLiteStatement* retVal = NULL;
	if (Sqlite)
	{
		ok = SHVBool(sqlite3_prepare_v2(Sqlite, sql.GetSafeBuffer(), -1, &sqllite_statement, &rest));
		notparsed = rest;
		retVal = new SHVSQLiteStatement_impl(sqllite_statement, this);
	}
	return retVal;
} 

SHVSQLiteStatement* SHVSQLiteWrapper_Impl::ExecuteUTF8(SHVBool& ok, const SHVStringUTF8C& sql, SHVStringSQLite& notparsed)
{
SHVMutexLocker lock(Lock);
sqlite3_stmt* sqllite_statement;
int res = SHVSQLiteWrapper::SQLite_ERROR;;
SHVSQLiteStatement* retVal = NULL;
const char* rest = NULL;
	if (Sqlite)
	{
		ok = SHVBool(sqlite3_prepare_v2(Sqlite, sql.GetSafeBuffer(), -1, &sqllite_statement, &rest));
		if (ok)
		{
			retVal = new SHVSQLiteStatement_impl(sqllite_statement, this);
			ok = SHVBool(retVal->NextResult().GetError());
		}
		notparsed = rest;
	}
	return retVal;
} 

SHVStringUTF8C SHVSQLiteWrapper_Impl::GetErrorMsgUTF8()
{
SHVMutexLocker lock(Lock);
	if (Sqlite)
		return sqlite3_errmsg(Sqlite);
	else
		return "Database is not open";
}

SHVMutex& SHVSQLiteWrapper_Impl::GetMutex()
{
	return Lock;
}