#ifndef __SHIVA_SQLITE_SQLITESTATEMENT_H
#define __SHIVA_SQLITE_SQLITESTATEMENT_H

#include "../utils/shvstring.h"
#include "../utils/shvstringutf8.h"
#include "../utils/shvrefobject.h"
#include "../threadutils/shvmutex.h"
#include "shvstringsqlite.h"

//-=========================================================================================================
///  SHVSQLiteStatement class - The shiva C++ interface for SQLite
/**
 * Use the Prepare function on SQLiteWrapper to execute a SQL statement in the SQLite
 * engine. 
 */
class SHVSQLiteStatement: public SHVRefObject
{
protected:
	virtual ~SHVSQLiteStatement() {}
public:

	// GetValue
	inline  SHVBool GetInt(int& val, int columnIdx) const;
	virtual SHVBool GetLong(long& val, int columnIdx) const = 0;
	virtual SHVBool GetInt64(SHVInt64Val& val, int columnIdx) const = 0;
	virtual SHVBool GetDouble(double& val, int columnIdx) const = 0;
	virtual SHVBool GetBlob(const void*& blob, int& len, int columnIdx) const = 0;
	virtual SHVBool GetStringUTF8(SHVStringSQLite& text, int& len, int columnIdx) const = 0;
	inline SHVBool GetString(SHVString& text, int& len, int columnIdx) const;

	// GetColumnName
	virtual SHVBool GetColumnNameUTF8(SHVStringSQLite& name, int columnIdx) const = 0;
	virtual SHVBool GetColumnName8(SHVString8& name, int columnIdx) const = 0;
	virtual SHVBool GetColumnName16(SHVString16& name, int columnIdx) const = 0;
	inline SHVBool GetColumnName(SHVString& name, int columnIdx) const;

	// GetColumnType
	virtual SHVBool GetColumnAffinity(short& type, int columnIdx) const = 0;
	virtual SHVBool GetColumnTypeUTF8(SHVStringSQLite& colType, int columnIdx) const = 0;
	virtual SHVBool GetColumnType8(SHVString8& colType, int columnIdx) const = 0;
	virtual SHVBool GetColumnType16(SHVString16& colType, int columnIdx) const = 0;
	inline SHVBool GetColumnType(SHVString& colType, int columnIdx) const;

	// GetColumnCount
	virtual int GetColumnCount() const = 0;

	// SetParameter
	virtual void ClearAllParameters() = 0;
	inline SHVBool SetParameterIntUTF8(const SHVStringUTF8C& name, int val);
	inline SHVBool SetParameterInt(const SHVStringC& name, int val);

	virtual SHVBool SetParameterLongUTF8(const SHVStringUTF8C& name, long val) = 0;
	inline SHVBool SetParameterLong(const SHVStringC& name, long val);

	virtual SHVBool SetParameterInt64UTF8(const SHVStringUTF8C& name, SHVInt64Val val) = 0;
	inline SHVBool SetParameterInt64(const SHVStringC& name, SHVInt64Val val);

	virtual SHVBool SetParameterDoubleUTF8(const SHVStringUTF8C& name, double val) = 0;
	inline SHVBool SetParameterDouble(const SHVStringC& name, double val);

	virtual SHVBool SetParameterStringUTF8(const SHVStringUTF8C& name, const SHVStringUTF8C& val) = 0;
	inline SHVBool SetParameterString(const SHVStringC& name, const SHVStringC& val);

	virtual SHVBool SetParameterNullUTF8(const SHVStringUTF8C& name) = 0;
	inline SHVBool SetParameterNull(SHVStringC& name);

	// Iteration methods
	virtual SHVBool NextResult() = 0;
	virtual SHVBool Reset() = 0;
};
typedef SHVRefObjectContainer<SHVSQLiteStatement> SHVSQLiteStatementRef;

// ============================================ implementation ============================================ //
SHVBool SHVSQLiteStatement::GetInt(int& val, int columnIdx) const
{
long lval;
SHVBool retVal = GetLong(lval, columnIdx);
	val = (int) lval;
	return retVal;
}

/*************************************
 * GetValue
 *************************************/
SHVBool SHVSQLiteStatement::GetString(SHVString& text, int& len, int columnIdx) const
{
	SHVStringSQLite res(NULL);
	SHVBool retVal = GetStringUTF8(res, len, columnIdx);
	if (retVal)
		text = res.ToStrT();
	return retVal;
}

/*************************************
 * GetColumnName
 *************************************/
SHVBool SHVSQLiteStatement::GetColumnName(SHVString& name, int columnIdx) const
{
#ifdef UNICODE
	return GetColumnName16(name, columnIdx);
#else
	return GetColumnName8(name, columnIdx);
#endif
}

/*************************************
 * GetColumnType
 *************************************/
SHVBool SHVSQLiteStatement::GetColumnType(SHVString& colType, int columnIdx) const
{
#ifdef UNICODE
	return GetColumnType16(colType, columnIdx);
#else
	return GetColumnType8(colType, columnIdx);
#endif
}

/*************************************
 * SetParameter
 *************************************/
inline SHVBool SHVSQLiteStatement::SetParameterIntUTF8(const SHVStringUTF8C& name, int val)
{
	return SetParameterLongUTF8(name, val);
}

inline SHVBool SHVSQLiteStatement::SetParameterInt(const SHVStringC& name, int val)
{
	return SetParameterLong(name, val);
}

SHVBool SHVSQLiteStatement::SetParameterLong(const SHVStringC& name, long val)
{
	return SetParameterLongUTF8(name.ToStrUTF8(), val);
}

SHVBool SHVSQLiteStatement::SetParameterInt64(const SHVStringC& name, SHVInt64Val val)
{
	return SetParameterInt64UTF8(name.ToStrUTF8(), val);
}

SHVBool SHVSQLiteStatement::SetParameterDouble(const SHVStringC& name, double val)
{
	return SetParameterDoubleUTF8(name.ToStrUTF8(), val);
}

SHVBool SHVSQLiteStatement::SetParameterString(const SHVStringC& name, const SHVStringC& val)
{
	return SetParameterStringUTF8(name.ToStrUTF8(), val.ToStrUTF8());
}

SHVBool SHVSQLiteStatement::SetParameterNull(SHVStringC& name)
{
	return SetParameterNullUTF8(name.ToStrUTF8());
}

#endif