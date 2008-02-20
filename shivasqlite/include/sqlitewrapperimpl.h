#ifndef __SHIVA_SQLITE_WRAPPER_IMPL_H
#define __SHIVA_SQLITE_WRAPPER_IMPL_H

#include "../sqlitewrapper.h"
#include "../../src/sqlite/sqlite3.h"
#include "../sqlitestatement.h"
#include "../../../include/threadutils/shvmutex.h"

//-=========================================================================================================
///  SHVSQLiteWrapper_Impl class - Implement the shiva C++ interface for SQLite
/**
 */
	
class SHVSQLiteWrapper_Impl: public SHVSQLiteWrapper
{
public:
	SHVSQLiteWrapper_Impl(void);
	virtual ~SHVSQLiteWrapper_Impl(void);

	virtual SHVBool OpenUTF8(const SHVStringUTF8C& fileName, int option = 6);
	virtual SHVBool OpenInMemory();
	virtual SHVBool Close();
	virtual SHVSQLiteStatement* PrepareUTF8(SHVBool& ok, const SHVStringUTF8C& sql, SHVStringUTF8& notparsed);
	virtual SHVStringUTF8C GetErrorMsgUTF8(); 
	virtual SHVMutex& GetMutex();
private:
	sqlite3* Sqlite;
	SHVMutex Lock;
};
#endif