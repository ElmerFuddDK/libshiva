#ifndef __SHIVA_SQLITE_SQLITESTATEMENT_H
#define __SHIVA_SQLITE_SQLITESTATEMENT_H

#include "../../include/utils/shvstring.h"
#include "../../include/utils/shvstringutf8.h"
#include "../../include/utils/shvrefobject.h"
#include "../../include/threadutils/shvmutex.h"

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
	virtual SHVBool GetValue(long& val, int columnIdx) const = 0;
	virtual SHVBool GetValue(double& val, int columnIdx) const = 0;
	virtual SHVBool GetValue(const void*& blob, int& len, int columnIdx) const = 0;
	virtual SHVBool GetValueUTF8(SHVStringUTF8C& text, int& len, int columnIdx) const = 0;
	inline SHVBool GetValue(SHVString& text, int& len, int columnIdx) const;

	// GetColumnName
	virtual SHVBool GetColumnNameUTF8(SHVStringUTF8C& name, int columnIdx) const = 0;
	virtual SHVBool GetColumnName8(SHVString8& name, int columnIdx) const = 0;
	virtual SHVBool GetColumnName16(SHVString16& name, int columnIdx) const = 0;
	inline SHVBool GetColumnName(SHVString& name, int columnIdx) const;

	// GetColumnType
	virtual SHVBool GetColumnType(short& type, int columnIdx) const = 0;
	virtual SHVBool GetColumnTypeUTF8(SHVStringUTF8C& colType, int columnIdx) const = 0;
	virtual SHVBool GetColumnType8(SHVString8& colType, int columnIdx) const = 0;
	virtual SHVBool GetColumnType16(SHVString16& colType, int columnIdx) const = 0;
	inline SHVBool GetColumnType(SHVString& colType, int columnIdx) const;

	// GetColumnCount
	virtual int GetColumnCount() const = 0;

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
typedef SHVRefObjectContainer<SHVSQLiteStatement> SHVSQLiteStatementRef;

// ============================================ implementation ============================================ //
/*************************************
 * GetValue
 *************************************/
SHVBool SHVSQLiteStatement::GetValue(SHVString& text, int& len, int columnIdx) const
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