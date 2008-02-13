#ifndef __SHIVA_SQLITE_SQLITESTATEMENT_H
#define __SHIVA_SQLITE_SQLITESTATEMENT_H

#include "../../include/utils/shvstring.h"
#include "../../include/utils/shvstringutf8.h"
#include "../../include/threadutils/shvmutex.h"

//-=========================================================================================================
///  SHVSQLiteStatement class - The shiva C++ interface for SQLite
/**
 * Use the Prepare function on SQLiteWrapper to execute a SQL statement in the SQLite
 * engine. 
 */
class SHVSQLiteStatement
{
public:
	virtual ~SHVSQLiteStatement() {}

	// GetValue
	virtual SHVBool GetValue(long& val, int columnIdx) = 0;
	virtual SHVBool GetValue(double& val, int columnIdx) = 0;
	virtual SHVBool GetValue(const void*& blob, int& len, int columnIdx) = 0;
	virtual SHVBool GetValueUTF8(SHVStringUTF8C& text, int& len, int columnIdx) = 0;
	inline SHVBool GetValue(SHVString& text, int& len, int columnIdx);

	// GetColumnName
	virtual SHVBool GetColumnNameUTF8(SHVStringUTF8C& name, int columnIdx) = 0;
	virtual SHVBool GetColumnName8(SHVString8& name, int columnIdx) = 0;
	virtual SHVBool GetColumnName16(SHVString16& name, int columnIdx) = 0;
	inline SHVBool GetColumnName(SHVString& name, int columnIdx);

	// GetColumnType
	virtual SHVBool GetColumnType(short& type, int columnIdx) = 0;

	// GetColumnCount
	virtual int GetColumnCount() = 0;

	// SetParameter
	virtual SHVBool SetParameterUTF8(const SHVStringUTF8C& name, long val) = 0;
	inline SHVBool SetParameter(const SHVStringC& name, long val);

	virtual SHVBool SetParameterUTF8(const SHVStringUTF8C& name, double val) = 0;
	inline SHVBool SetParameter(const SHVStringC& name, double val);

	virtual SHVBool SetParameterUTF8(const SHVStringUTF8C& name, const SHVStringUTF8C& val) = 0;
	inline SHVBool SetParameter(const SHVStringC& name, const SHVStringC& val);

	virtual SHVBool SetParameterNullUTF8(const SHVStringUTF8C& name) = 0;
	inline SHVBool SetParameterNull(SHVStringC& name);

	// Iteration methods
	virtual SHVBool NextResult() = 0;
	virtual SHVBool Reset() = 0;	
};
// ============================================ implementation ============================================ //
/*************************************
 * GetValue
 *************************************/
SHVBool SHVSQLiteStatement::GetValue(SHVString& text, int& len, int columnIdx)
{
	SHVStringUTF8C res(NULL);
	SHVBool retVal = GetValueUTF8(res, len, columnIdx);
	if (retVal)
		text = res.ToStrT();
	return retVal;
}

/*************************************
 * GetColumnName
 *************************************/
SHVBool SHVSQLiteStatement::GetColumnName(SHVString& name, int columnIdx)
{
#ifdef UNICODE
	return GetColumnName16(name, columnIdx);
#else
	return GetColumnName8(name, columnIdx);
#endif
}

/*************************************
 * SetParameter
 *************************************/
SHVBool SHVSQLiteStatement::SetParameter(const SHVStringC& name, long val)
{
	return SetParameterUTF8(name.ToStrUTF8(), val);
}

SHVBool SHVSQLiteStatement::SetParameter(const SHVStringC& name, double val)
{
	return SetParameterUTF8(name.ToStrUTF8(), val);
}

SHVBool SHVSQLiteStatement::SetParameter(const SHVStringC& name, const SHVStringC& val)
{
	return SetParameterUTF8(name.ToStrUTF8(), val.ToStrUTF8());
}

SHVBool SHVSQLiteStatement::SetParameterNull(SHVStringC& name)
{
	return SetParameterNullUTF8(name.ToStrUTF8());
}

#endif