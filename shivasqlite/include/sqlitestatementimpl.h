#ifndef __SQLITESTATEMENT_IMPL_H
#define __SQLITESTATEMENT_IMPL_H

#include "../sqlitewrapper.h"
#include "../sqlitestatement.h"


class RFTSQLiteStatement_impl: public RFTSQLiteStatement
{
private:
	sqlite3_stmt* m_Statement;
public:
	RFTSQLiteStatement_impl(sqlite3_stmt* statement);
	virtual ~RFTSQLiteStatement_impl();

	virtual short GetValue(long& val, int columnIdx);
	virtual short GetValue(double& val, int columnIdx);
	virtual short GetValue(const void*& blob, int& len, int columnIdx);
	virtual short GetValue(const char*& text, int& len, int columnIdx);
	virtual short GetColumnName(const char*& name, int columnIdx);
	virtual short GetColumnType(short& type, int columnIdx);
	virtual int GetColumnCount();

	virtual short NextResult();
	virtual short Reset();
};
#endif