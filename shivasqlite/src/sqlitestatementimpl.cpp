#include "StdAfx.h"
#include "../../../include/platformspc.h"

#include "../SQLite/sqlite3.h"
#include "../../include/sqliteimpl/sqlitewrapper_impl.h"
#include "../../include/sqliteimpl/sqlitestatement_impl.h"
#include "../../../include/threadutils/shvmutexlocker.h"

SHVSQLiteStatement_impl::SHVSQLiteStatement_impl(sqlite3_stmt* statement, SHVSQLiteWrapper* owner)
{
	Statement = statement;	
	Owner = owner;
}

SHVSQLiteStatement_impl::~SHVSQLiteStatement_impl()
{
SHVMutexLocker lock(Owner->GetMutex());
	if (Statement)
		sqlite3_finalize(Statement);
}

SHVBool SHVSQLiteStatement_impl::GetValue(long& val, int columnIdx)
{
SHVMutexLocker lock(Lock);
	if (columnIdx < sqlite3_column_count(Statement) && columnIdx >= 0)
	{
		val = sqlite3_column_int(Statement, columnIdx);
		return SHVBool(SHVSQLiteWrapper::SQLite_OK);
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);	
}

SHVBool SHVSQLiteStatement_impl::GetValue(double& val, int columnIdx)
{
SHVMutexLocker lock(Lock);
	if (columnIdx < sqlite3_column_count(Statement) && columnIdx >= 0)
	{
		val = sqlite3_column_double(Statement, columnIdx);
		return SHVBool(SHVSQLiteWrapper::SQLite_OK);
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);	
}

SHVBool SHVSQLiteStatement_impl::GetValue(const void*& blob, int& len, int columnIdx)
{
SHVMutexLocker lock(Lock);
	if (columnIdx < sqlite3_column_count(Statement) && columnIdx >= 0)
	{
		len = sqlite3_column_count(Statement);
		blob = sqlite3_column_blob(Statement, columnIdx);
		return SHVBool(SHVSQLiteWrapper::SQLite_OK);
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);
}

SHVBool SHVSQLiteStatement_impl::GetValueUTF8(SHVStringUTF8C& text, int& len, int columnIdx)
{
SHVMutexLocker lock(Lock);
	if (columnIdx < sqlite3_column_count(Statement) && columnIdx >= 0)
	{
		len = sqlite3_column_bytes(Statement, columnIdx);
		text = SHVStringUTF8C((const SHVChar*) sqlite3_column_text(Statement, columnIdx));
		return SHVBool(SHVSQLiteWrapper::SQLite_OK);
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);
}

SHVBool SHVSQLiteStatement_impl::GetColumnNameUTF8(SHVStringUTF8C& name, int columnIdx)
{
SHVMutexLocker lock(Lock);
	if (columnIdx < sqlite3_column_count(Statement) && columnIdx >= 0)
	{
		name = sqlite3_column_name(Statement, columnIdx);
		return SHVBool(SHVSQLiteWrapper::SQLite_OK);
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);
}


SHVBool SHVSQLiteStatement_impl::GetColumnType(short& type, int columnIdx)
{
SHVMutexLocker lock(Lock);
	if (columnIdx < sqlite3_column_count(Statement) && columnIdx >= 0)
	{
		type = sqlite3_column_type(Statement, columnIdx);
		return SHVSQLiteWrapper::SQLite_OK;
	}
	else
		return SHVSQLiteWrapper::SQLite_ERROR;			
}

int SHVSQLiteStatement_impl::GetColumnCount()
{
SHVMutexLocker lock(Lock);
	return sqlite3_column_count(Statement);
}

SHVBool SHVSQLiteStatement_impl::SetParameterUTF8(const SHVStringUTF8C& name, long val)
{
SHVMutexLocker lock(Lock);
	int pIdx = sqlite3_bind_parameter_index(Statement, name.GetSafeBuffer());
	if (pIdx)
	{
		return SHVBool(sqlite3_bind_int64(Statement, pIdx, val));
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);
}

SHVBool SHVSQLiteStatement_impl::SetParameterUTF8(const SHVStringUTF8C& name, double val)
{
SHVMutexLocker lock(Lock);
	int pIdx = sqlite3_bind_parameter_index(Statement, name.GetSafeBuffer());
	if (pIdx)
	{
		return SHVBool(sqlite3_bind_double(Statement, pIdx, val));
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);
}

SHVBool SHVSQLiteStatement_impl::SetParameterUTF8(const SHVStringUTF8C& name, const SHVStringUTF8C& val)
{
SHVMutexLocker lock(Lock);
	int pIdx = sqlite3_bind_parameter_index(Statement, name.GetSafeBuffer());
	if (pIdx)
	{
		return SHVBool(sqlite3_bind_text(Statement, pIdx, val.GetSafeBuffer(), SHVString8C::StrLen(val.GetSafeBuffer()), SQLITE_TRANSIENT));
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);
}

SHVBool SHVSQLiteStatement_impl::SetParameterNullUTF8(const SHVStringUTF8C& name)
{
SHVMutexLocker lock(Lock);
	int pIdx = sqlite3_bind_parameter_index(Statement, name.GetSafeBuffer());
	if (pIdx)
	{
		return SHVBool(sqlite3_bind_null(Statement, pIdx));
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);
}

SHVBool SHVSQLiteStatement_impl::NextResult()
{
SHVMutexLocker lock(Lock);
	return SHVBool(sqlite3_step(Statement));
}

SHVBool SHVSQLiteStatement_impl::Reset()
{
SHVMutexLocker lock(Lock);
	return SHVBool(sqlite3_reset(Statement));
}	


