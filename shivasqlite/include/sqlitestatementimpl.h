#ifndef __SHIVA_SQLITE_SQLITESTATEMENT_IMPL_H
#define __SHIVA_SQLITE_SQLITESTATEMENT_IMPL_H

#include "../sqlitewrapper.h"
#include "../sqlitestatement.h"
#include "../../../include/threadutils/shvmutexlocker.h"

// forward declares
class SQLiteWrapper;
//-=========================================================================================================
///  SHVSQLiteStatement_impl class - Implements the shiva C++ interface for a SQLite statement
/**
 */
class SHVSQLiteStatement_impl: public SHVSQLiteStatement
{
public:
	SHVSQLiteStatement_impl(sqlite3_stmt* statement, SHVSQLiteWrapper* owner);
	virtual ~SHVSQLiteStatement_impl();

	virtual SHVBool GetValue(long& val, int columnIdx);
	virtual SHVBool GetValue(double& val, int columnIdx);
	virtual SHVBool GetValue(const void*& blob, int& len, int columnIdx);
	virtual SHVBool GetValueUTF8(SHVStringUTF8C& text, int& len, int columnIdx);
	virtual SHVBool GetColumnNameUTF8(SHVStringUTF8C& name, int columnIdx);
	virtual SHVBool GetColumnName8(SHVString8& name, int columnIdx);
	virtual SHVBool GetColumnName16(SHVString16& name, int columnIdx);

	virtual SHVBool GetColumnType(short& type, int columnIdx);
	virtual int GetColumnCount();

	virtual SHVBool SetParameterUTF8(const SHVStringUTF8C& name, long val);
	virtual SHVBool SetParameterUTF8(const SHVStringUTF8C& name, double val);
	virtual SHVBool SetParameterUTF8(const SHVStringUTF8C& name, const SHVStringUTF8C& val);
	virtual SHVBool SetParameterNullUTF8(const SHVStringUTF8C& name);

	virtual SHVBool NextResult();
	virtual SHVBool Reset();
private:
	sqlite3_stmt* Statement;
	SHVMutex Lock;
	SHVSQLiteWrapperRef Owner;
};


#endif