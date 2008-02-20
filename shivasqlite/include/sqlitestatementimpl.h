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

	virtual SHVBool GetValue(long& val, int columnIdx) const;
	virtual SHVBool GetValue(double& val, int columnIdx) const;
	virtual SHVBool GetValue(const void*& blob, int& len, int columnIdx) const;
	virtual SHVBool GetValueUTF8(SHVStringUTF8C& text, int& len, int columnIdx) const;
	virtual SHVBool GetColumnNameUTF8(SHVStringUTF8C& name, int columnIdx) const;
	virtual SHVBool GetColumnName8(SHVString8& name, int columnIdx) const;
	virtual SHVBool GetColumnName16(SHVString16& name, int columnIdx) const;

	virtual SHVBool GetColumnType(short& type, int columnIdx) const;
	virtual SHVBool GetColumnTypeUTF8(SHVStringUTF8C& colType, int columnIdx) const;
	virtual SHVBool GetColumnType8(SHVString8& colType, int columnIdx) const;
	virtual SHVBool GetColumnType16(SHVString16& colType, int columnIdx) const;

	virtual int GetColumnCount() const;

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