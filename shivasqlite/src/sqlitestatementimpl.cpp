#include "StdAfx.h"
#include "../../../include/platformspc.h"

#include "../SQLite/sqlite3.h"
#include "../../include/sqliteimpl/sqlitewrapper_impl.h"
#include "../../include/sqliteimpl/sqlitestatement_impl.h"
#include "../../../include/threadutils/shvmutexlocker.h"

/*************************************
 * Constructor
 *************************************/
SHVSQLiteStatement_impl::SHVSQLiteStatement_impl(sqlite3_stmt* statement, SHVSQLiteWrapper* owner, int rowcount)
{
	Statement = statement;	
	Owner = owner;
	RowCount = rowcount;
}

SHVSQLiteStatement_impl::~SHVSQLiteStatement_impl()
{
SHVMutexLocker lock(Owner->GetMutex());
	if (Statement)
		sqlite3_finalize(Statement);
}

/*************************************
 * GetLong
 *************************************/
SHVBool SHVSQLiteStatement_impl::GetLong(long& val, int columnIdx) const
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

/*************************************
 * GetDouble
 *************************************/
SHVBool SHVSQLiteStatement_impl::GetDouble(double& val, int columnIdx) const
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

/*************************************
 * GetBlob
 *************************************/
SHVBool SHVSQLiteStatement_impl::GetBlob(const void*& blob, int& len, int columnIdx) const
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

/*************************************
 * GetValueUTF8
 *************************************/
SHVBool SHVSQLiteStatement_impl::GetStringUTF8(SHVStringSQLite& text, int& len, int columnIdx) const
{
SHVMutexLocker lock(Lock);
	if (columnIdx < sqlite3_column_count(Statement) && columnIdx >= 0)
	{
		len = sqlite3_column_bytes(Statement, columnIdx);
		text = SHVStringSQLite((const SHVChar*) sqlite3_column_text(Statement, columnIdx));
		return SHVBool(SHVSQLiteWrapper::SQLite_OK);
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);
}

/*************************************
 * GetColumnNameUTF8
 *************************************/
SHVBool SHVSQLiteStatement_impl::GetColumnNameUTF8(SHVStringSQLite& name, int columnIdx) const
{
SHVMutexLocker lock(Lock);
	if (columnIdx < sqlite3_column_count(Statement) && columnIdx >= 0)
	{
		name = SHVStringSQLite(sqlite3_column_name(Statement, columnIdx));
		return SHVBool(SHVSQLiteWrapper::SQLite_OK);
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);
}

/*************************************
 * GetColumnName8
 *************************************/
SHVBool SHVSQLiteStatement_impl::GetColumnName8(SHVString8& name, int columnIdx) const
{
	SHVStringSQLite res(NULL);
	SHVBool retVal = GetColumnNameUTF8(res, columnIdx);
	if (retVal)
		name = res.ToStr8();
	return retVal;
}

/*************************************
 * GetColumnName16
 *************************************/
SHVBool SHVSQLiteStatement_impl::GetColumnName16(SHVString16& name, int columnIdx) const
{
	SHVStringSQLite res(NULL);
	SHVBool retVal = GetColumnNameUTF8(res, columnIdx);
	if (retVal)
		name = res.ToStr16();
	return retVal;
}


/*************************************
 * GetColumnType
 *************************************/
SHVBool SHVSQLiteStatement_impl::GetColumnAffinity(short& type, int columnIdx) const
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

/*************************************
 * GetColumnTypeUTF8
 *************************************/
SHVBool SHVSQLiteStatement_impl::GetColumnTypeUTF8(SHVStringSQLite& colType, int columnIdx) const
{
SHVMutexLocker lock(Lock);
	if (columnIdx < sqlite3_column_count(Statement) && columnIdx >= 0)
	{
		colType = sqlite3_column_decltype(Statement, columnIdx);
		return SHVBool(SHVSQLiteWrapper::SQLite_OK);
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);
}

/*************************************
 * GetColumnType8
 *************************************/
SHVBool SHVSQLiteStatement_impl::GetColumnType8(SHVString8& colType, int columnIdx) const
{
SHVStringSQLite res(NULL);
	SHVBool retVal = GetColumnTypeUTF8(res, columnIdx);
	if (retVal)
		colType = res.ToStr8();
	return retVal;
}

/*************************************
 * GetColumnType16
 *************************************/
SHVBool SHVSQLiteStatement_impl::GetColumnType16(SHVString16& colType, int columnIdx) const
{
SHVStringSQLite res(NULL);
	SHVBool retVal = GetColumnTypeUTF8(res, columnIdx);
	if (retVal)
		colType = res.ToStr16();
	return retVal;
}

/*************************************
 * GetColumnCount
 *************************************/
int SHVSQLiteStatement_impl::GetColumnCount() const
{
SHVMutexLocker lock(Lock);
	return sqlite3_column_count(Statement);
}

/*************************************
 * GetRowCount
 *************************************/
int SHVSQLiteStatement_impl::GetRowCount() const
{
SHVMutexLocker lock(Lock);
	return RowCount;
}

/*************************************
 * SetParameterLongUTF8
 *************************************/
SHVBool SHVSQLiteStatement_impl::SetParameterLongUTF8(const SHVStringUTF8C& name, long val)
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

/*************************************
 * SetParameterDoubleUTF8
 *************************************/
SHVBool SHVSQLiteStatement_impl::SetParameterDoubleUTF8(const SHVStringUTF8C& name, double val)
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

/*************************************
 * SetParameterStringUTF8
 *************************************/
SHVBool SHVSQLiteStatement_impl::SetParameterStringUTF8(const SHVStringUTF8C& name, const SHVStringUTF8C& val)
{
SHVMutexLocker lock(Lock);
	int pIdx = sqlite3_bind_parameter_index(Statement, name.GetSafeBuffer());
	if (pIdx)
	{
		return SHVBool(sqlite3_bind_text(Statement, pIdx, val.GetSafeBuffer(), (int) SHVString8C::StrLen(val.GetSafeBuffer()), SQLITE_TRANSIENT));
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);
}

/*************************************
 * SetParameterNullUTF8
 *************************************/
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

/*************************************
 * NextResult
 *************************************/
SHVBool SHVSQLiteStatement_impl::NextResult()
{
SHVMutexLocker lock(Lock);
	return SHVBool(sqlite3_step(Statement));
}

/*************************************
 * Reset
 *************************************/
SHVBool SHVSQLiteStatement_impl::Reset()
{
SHVMutexLocker lock(Lock);
	return SHVBool(sqlite3_reset(Statement));
}	


