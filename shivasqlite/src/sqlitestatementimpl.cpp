#include "StdAfx.h"
#include "../SQLite/sqlite3.h"
#include "sqlitewrapper_impl.h"
#include "sqlitestatement_impl.h"

RFTSQLiteStatement_impl::RFTSQLiteStatement_impl(sqlite3_stmt* statement)
{
	m_Statement = statement;
}

RFTSQLiteStatement_impl::~RFTSQLiteStatement_impl()
{
	sqlite3_finalize(m_Statement);
}


short RFTSQLiteStatement_impl::GetValue(long& val, int columnIdx)
{
	if (columnIdx < sqlite3_column_count(m_Statement) && columnIdx >= 0)
	{
		val = sqlite3_column_int(m_Statement, columnIdx);
		return RFTSQLiteWrapper::SQLite_OK;
	}
	else
		return RFTSQLiteWrapper::SQLite_ERROR;	
}

short RFTSQLiteStatement_impl::GetValue(double& val, int columnIdx)
{
	if (columnIdx < sqlite3_column_count(m_Statement) && columnIdx >= 0)
	{
		val = sqlite3_column_double(m_Statement, columnIdx);
		return RFTSQLiteWrapper::SQLite_OK;
	}
	else
		return RFTSQLiteWrapper::SQLite_ERROR;	
}

short RFTSQLiteStatement_impl::GetValue(const void*& blob, int& len, int columnIdx)
{
	if (columnIdx < sqlite3_column_count(m_Statement) && columnIdx >= 0)
	{
		len = sqlite3_column_count(m_Statement);
		blob = sqlite3_column_blob(m_Statement, columnIdx);
		return RFTSQLiteWrapper::SQLite_OK;
	}
	else
		return RFTSQLiteWrapper::SQLite_ERROR;		
}

short RFTSQLiteStatement_impl::GetValue(const char*& text, int& len, int columnIdx)
{
	if (columnIdx < sqlite3_column_count(m_Statement) && columnIdx >= 0)
	{
		len = sqlite3_column_bytes(m_Statement, columnIdx);
		text = (const char*) sqlite3_column_text(m_Statement, columnIdx);
		return RFTSQLiteWrapper::SQLite_OK;
	}
	else
		return RFTSQLiteWrapper::SQLite_ERROR;		
}

short RFTSQLiteStatement_impl::GetColumnName(const char*& name, int columnIdx)
{
	if (columnIdx < sqlite3_column_count(m_Statement) && columnIdx >= 0)
	{
		name = sqlite3_column_name(m_Statement, columnIdx);
		return RFTSQLiteWrapper::SQLite_OK;
	}
	else
		return RFTSQLiteWrapper::SQLite_ERROR;			
}


short RFTSQLiteStatement_impl::GetColumnType(short& type, int columnIdx)
{
	if (columnIdx < sqlite3_column_count(m_Statement) && columnIdx >= 0)
	{
		type = sqlite3_column_type(m_Statement, columnIdx);
		return RFTSQLiteWrapper::SQLite_OK;
	}
	else
		return RFTSQLiteWrapper::SQLite_ERROR;			
}

int RFTSQLiteStatement_impl::GetColumnCount()
{
	return sqlite3_column_count(m_Statement);
}

short RFTSQLiteStatement_impl::NextResult()
{
	return sqlite3_step(m_Statement);
}

short RFTSQLiteStatement_impl::Reset()
{
	return sqlite3_reset(m_Statement);
}	


