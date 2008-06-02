#include "stdafx.h"
#include "../../include/platformspc.h"

#include "sqlite/sqlite3.h"
#include "../include/sqlitewrapperimpl.h"
#include "../include/sqlitestatementimpl.h"
#include "../../include/threadutils/shvmutexlocker.h"

/*************************************
 * Constructor
 *************************************/
SHVSQLiteStatementImpl::SHVSQLiteStatementImpl(sqlite3_stmt* statement, SHVSQLiteWrapper* owner, int rowcount)
{
#ifdef DEBUG
	OwnerThread = SHVThreadBase::GetCurrentThreadID();
#endif
	Statement = statement;	
	Owner = owner;
}

SHVSQLiteStatementImpl::~SHVSQLiteStatementImpl()
{
	SHVTHREADCHECK(OwnerThread);
	if (Statement)
		sqlite3_finalize(Statement);
}

/*************************************
 * GetLong
 *************************************/
SHVBool SHVSQLiteStatementImpl::GetLong(long& val, int columnIdx) const
{
	SHVTHREADCHECK(OwnerThread);
	if (IsValid(columnIdx))
	{
		val = sqlite3_column_int(Statement, columnIdx);
		return SHVBool(SHVSQLiteWrapper::SQLite_OK);
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);	
}

/*************************************
 * GetLong
 *************************************/
SHVBool SHVSQLiteStatementImpl::GetInt64(SHVInt64Val& val, int columnIdx) const
{
	SHVTHREADCHECK(OwnerThread);
	if (IsValid(columnIdx))
	{
		val = sqlite3_column_int64(Statement, columnIdx);
		return SHVBool(SHVSQLiteWrapper::SQLite_OK);
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);	
}

/*************************************
 * GetDouble
 *************************************/
SHVBool SHVSQLiteStatementImpl::GetDouble(double& val, int columnIdx) const
{
	SHVTHREADCHECK(OwnerThread);
	if (IsValid(columnIdx))
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
SHVBool SHVSQLiteStatementImpl::GetBlob(const void*& blob, int& len, int columnIdx) const
{
	SHVTHREADCHECK(OwnerThread);
	if (IsValid(columnIdx))
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
SHVBool SHVSQLiteStatementImpl::GetStringUTF8(SHVStringSQLite& text, int& len, int columnIdx) const
{
	SHVTHREADCHECK(OwnerThread);
	if (IsValid(columnIdx))
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
SHVBool SHVSQLiteStatementImpl::GetColumnNameUTF8(SHVStringSQLite& name, int columnIdx) const
{
	SHVTHREADCHECK(OwnerThread);
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
SHVBool SHVSQLiteStatementImpl::GetColumnName8(SHVString8& name, int columnIdx) const
{
SHVStringSQLite res(NULL);
SHVBool retVal;
	SHVTHREADCHECK(OwnerThread);
	retVal = GetColumnNameUTF8(res, columnIdx);
	if (retVal)
		name = res.ToStr8();
	return retVal;
}

/*************************************
 * GetColumnName16
 *************************************/
SHVBool SHVSQLiteStatementImpl::GetColumnName16(SHVString16& name, int columnIdx) const
{
SHVStringSQLite res(NULL);
SHVBool retVal;
	SHVTHREADCHECK(OwnerThread);
	retVal = GetColumnNameUTF8(res, columnIdx);
	if (retVal)
		name = res.ToStr16();
	return retVal;
}


/*************************************
 * GetColumnType
 *************************************/
SHVBool SHVSQLiteStatementImpl::GetColumnAffinity(short& type, int columnIdx) const
{
	SHVTHREADCHECK(OwnerThread);
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
SHVBool SHVSQLiteStatementImpl::GetColumnTypeUTF8(SHVStringSQLite& colType, int columnIdx) const
{
	SHVTHREADCHECK(OwnerThread);
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
SHVBool SHVSQLiteStatementImpl::GetColumnType8(SHVString8& colType, int columnIdx) const
{
SHVStringSQLite res(NULL);
SHVBool retVal;
	SHVTHREADCHECK(OwnerThread);
	retVal = GetColumnTypeUTF8(res, columnIdx);
	if (retVal)
		colType = res.ToStr8();
	return retVal;
}

/*************************************
 * GetColumnType16
 *************************************/
SHVBool SHVSQLiteStatementImpl::GetColumnType16(SHVString16& colType, int columnIdx) const
{
SHVStringSQLite res(NULL);
SHVBool retVal;
	SHVTHREADCHECK(OwnerThread);
	retVal = GetColumnTypeUTF8(res, columnIdx);
	if (retVal)
		colType = res.ToStr16();
	return retVal;
}

/*************************************
 * GetColumnCount
 *************************************/
int SHVSQLiteStatementImpl::GetColumnCount() const
{
	SHVTHREADCHECK(OwnerThread);
	return sqlite3_column_count(Statement);
}

/*************************************
 * SetParameterLongUTF8
 *************************************/
SHVBool SHVSQLiteStatementImpl::SetParameterLongUTF8(const SHVStringUTF8C& name, long val)
{
int pIdx;
	SHVTHREADCHECK(OwnerThread);
	pIdx = sqlite3_bind_parameter_index(Statement, name.GetSafeBuffer());
	if (pIdx)
	{
		return SHVBool(sqlite3_bind_int(Statement, pIdx, val));
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);
}

/*************************************
 * SetParameterInt64UTF8
 *************************************/
SHVBool SHVSQLiteStatementImpl::SetParameterInt64UTF8(const SHVStringUTF8C& name, SHVInt64Val val)
{
int pIdx;
	SHVTHREADCHECK(OwnerThread);
	pIdx = sqlite3_bind_parameter_index(Statement, name.GetSafeBuffer());
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
SHVBool SHVSQLiteStatementImpl::SetParameterDoubleUTF8(const SHVStringUTF8C& name, double val)
{
int pIdx; 
	SHVTHREADCHECK(OwnerThread);	
	pIdx = sqlite3_bind_parameter_index(Statement, name.GetSafeBuffer());
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
SHVBool SHVSQLiteStatementImpl::SetParameterStringUTF8(const SHVStringUTF8C& name, const SHVStringUTF8C& val)
{
int pIdx;
	pIdx = sqlite3_bind_parameter_index(Statement, name.GetSafeBuffer());
	SHVTHREADCHECK(OwnerThread);	
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
SHVBool SHVSQLiteStatementImpl::SetParameterNullUTF8(const SHVStringUTF8C& name)
{
int pIdx;
	pIdx = sqlite3_bind_parameter_index(Statement, name.GetSafeBuffer());
	SHVTHREADCHECK(OwnerThread);	
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
SHVBool SHVSQLiteStatementImpl::NextResult()
{
	SHVTHREADCHECK(OwnerThread);	
	return SHVBool(sqlite3_step(Statement));
}

/*************************************
 * Reset
 *************************************/
SHVBool SHVSQLiteStatementImpl::Reset()
{
	SHVTHREADCHECK(OwnerThread);	
	return SHVBool(sqlite3_reset(Statement));
}	

/*************************************
 * IsValid
 *************************************/
bool SHVSQLiteStatementImpl::IsValid(int columnIdx) const
{
	return columnIdx < sqlite3_column_count(Statement) && 
		columnIdx >= 0 && 
		sqlite3_column_type(Statement, columnIdx) != SQLITE_NULL;
}

