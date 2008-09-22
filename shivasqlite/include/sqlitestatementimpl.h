#ifndef __SHIVA_SQLITE_SQLITESTATEMENT_IMPL_H
#define __SHIVA_SQLITE_SQLITESTATEMENT_IMPL_H

#include "../../include/sqlite/sqlitewrapper.h"
#include "../../include/sqlite/sqlitestatement.h"
#include "../../include/threadutils/shvmutexlocker.h"

// forward declares
class SQLiteWrapper;
//-=========================================================================================================
///  SHVSQLiteStatementImpl class - Implements the shiva C++ interface for a SQLite statement
/**
 */
class SHVSQLiteStatementImpl: public SHVSQLiteStatement
{
public:
	SHVSQLiteStatementImpl(sqlite3_stmt* statement, SHVSQLiteWrapper* owner, int rowcount);
	virtual ~SHVSQLiteStatementImpl();

	virtual SHVBool GetLong(long& val, int columnIdx) const;
	virtual SHVBool GetInt64(SHVInt64Val& val, int columnIdx) const;
	virtual SHVBool GetDouble(double& val, int columnIdx) const;
	virtual SHVBool GetBlob(const void*& blob, int& len, int columnIdx) const;
	virtual SHVBool GetStringUTF8(SHVStringSQLite& text, int& len, int columnIdx) const;
	virtual SHVBool GetColumnNameUTF8(SHVStringSQLite& name, int columnIdx) const;
	virtual SHVBool GetColumnName8(SHVString8& name, int columnIdx) const;
	virtual SHVBool GetColumnName16(SHVString16& name, int columnIdx) const;

	virtual SHVBool GetColumnAffinity(short& type, int columnIdx) const;
	virtual SHVBool GetColumnTypeUTF8(SHVStringSQLite& colType, int columnIdx) const;
	virtual SHVBool GetColumnType8(SHVString8& colType, int columnIdx) const;
	virtual SHVBool GetColumnType16(SHVString16& colType, int columnIdx) const;

	virtual int GetColumnCount() const;
	virtual int GetParameterCount() const;

	virtual SHVBool SetParameterLongUTF8(const SHVStringUTF8C& name, long val);
	virtual SHVBool SetParameterInt64UTF8(const SHVStringUTF8C& name, SHVInt64Val val);
	virtual SHVBool SetParameterDoubleUTF8(const SHVStringUTF8C& name, double val);
	virtual SHVBool SetParameterStringUTF8(const SHVStringUTF8C& name, const SHVStringUTF8C& val);
	virtual SHVBool SetParameterNullUTF8(const SHVStringUTF8C& name);

	virtual SHVBool NextResult();
	virtual SHVBool Reset();
private:
	sqlite3_stmt* Statement;
	SHVSQLiteWrapperRef Owner;

	bool IsValid(int columnIdx) const;
#ifdef DEBUG
	SHVThreadBase::ThreadID OwnerThread;
#endif
};


#endif