#include "StdAfx.h"
#include "sqlitewrapper_impl.h"
#include "sqlitestatement_impl.h"

RFTSQLiteWrapper_Impl::RFTSQLiteWrapper_Impl(void)
{
	m_Sqlite = NULL;
}

RFTSQLiteWrapper_Impl::~RFTSQLiteWrapper_Impl(void)
{
	if (m_Sqlite)
	{
		sqlite3_close(m_Sqlite);
		m_Sqlite = NULL;
	}
}

short RFTSQLiteWrapper_Impl::Open(const char* fileName, int option)
{
	if (m_Sqlite)
	{
		sqlite3_close(m_Sqlite);
	}
	return (short) sqlite3_open_v2(fileName, &m_Sqlite, option, NULL);
}

short RFTSQLiteWrapper_Impl::OpenInMemory()
{
	if (m_Sqlite)
	{
		sqlite3_close(m_Sqlite);
	}
	return (short) sqlite3_open(":memory", &m_Sqlite);
}

short RFTSQLiteWrapper_Impl::Close()
{
	if (m_Sqlite)
	{
		short res = (short) sqlite3_close(m_Sqlite);
		m_Sqlite = NULL;
		return res;
	}
	else
		return RFTSQLiteWrapper::SQLite_ERROR;
}

short RFTSQLiteWrapper_Impl::Prepare(RFTSQLiteStatement*& statement, const char* sql, const char*& notparsed)
{
	sqlite3_stmt* sqllite_statement;
	short res = sqlite3_prepare_v2(m_Sqlite, sql, -1, &sqllite_statement, &notparsed);
	statement = new RFTSQLiteStatement_impl(sqllite_statement);
	return res;
} 

const char* RFTSQLiteWrapper_Impl::GetErrorMsg()
{
	return sqlite3_errmsg(m_Sqlite);
}