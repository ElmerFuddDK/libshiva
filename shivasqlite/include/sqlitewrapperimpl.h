#ifndef __SHIVA_SQLITE_WRAPPER_IMPL_H
#define __SHIVA_SQLITE_WRAPPER_IMPL_H

#include "../src/sqlite/sqlite3.h"
#include "../../include/sqlite/sqlitewrapper.h"
#include "../../include/sqlite/sqlitestatement.h"
#include "../../include/threadutils/shvmutex.h"

//-=========================================================================================================
///  SHVSQLiteWrapperImpl class - Implement the shiva C++ interface for SQLite
/**
 */
	
class SHVSQLiteWrapperImpl: public SHVSQLiteWrapper
{
public:
	SHVSQLiteWrapperImpl(void);
	virtual ~SHVSQLiteWrapperImpl(void);

	virtual SHVBool OpenUTF8(const SHVStringUTF8C& fileName, int option = 6);
	virtual SHVBool OpenInMemory();
	virtual SHVBool Close();
	virtual SHVSQLiteStatement* PrepareUTF8(SHVBool& ok, const SHVStringUTF8C& sql, SHVStringSQLite& notparsed);
	virtual SHVSQLiteStatement* ExecuteUTF8(SHVBool& ok, const SHVStringUTF8C& sql, SHVStringSQLite& notparsed);
	virtual SHVStringUTF8C GetErrorMsgUTF8(); 
	virtual SHVMutex& GetMutex();
private:
	sqlite3* Sqlite;
	SHVMutex Lock;
};
#endif