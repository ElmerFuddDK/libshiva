#ifndef __SQLITEWRAPPER_IMPL_H
#define __SQLITEWRAPPER_IMPL_H

#include "../sqlitewrapper.h"
#include "../SQLite\sqlite3.h"
#include "../sqlitestatement.h"

class RFTSQLiteWrapper_Impl: public RFTSQLiteWrapper
{
private:
	sqlite3* m_Sqlite;
public:
	RFTSQLiteWrapper_Impl(void);
	virtual ~RFTSQLiteWrapper_Impl(void);

	virtual short Open(const char* fileName, int option);
	virtual short OpenInMemory();
	virtual short Close();
	virtual short Prepare(RFTSQLiteStatement*& statement, const char* sql, const char*& notparsed); 
	virtual const char* GetErrorMsg();
};
#endif