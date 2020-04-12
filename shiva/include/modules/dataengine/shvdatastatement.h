#ifndef __SHIVA_DATASTATEMENT_H
#define __SHIVA_DATASTATEMENT_H

#include "../../shvtypes.h"
#include "../../utils/shvrefobject.h"
#include "../../utils/shvstring.h"


//-=========================================================================================================
/// SHVDataStatement class - Raw access to a precompiled sql statement
/**
 * Interface for a generic shiva data precompiled statement. The statement is
 * divided into results, 1 for each sub-statement. You will need to iterate through
 * them all in order to perform the whole query:
\code
void RunStatement(SHVDataStatement* statement)
{
	statement->Reset();
	statement->ClearAllParameters();
	// set paramaters here

	while (statement->NextResult())
	{
		// Here you can query the columns, as each result is unique
		while (statement->NextRow())
		{
			// Here you can query data from the rows.
		}
	}
}
\endcode
 */
class SHVDataStatement: public SHVRefObject
{
public:

	// IsOk
	virtual SHVBool IsOk() const = 0;

	// GetValue
	inline  SHVBool GetInt(int& val, int columnIdx) const;
	virtual SHVBool GetLong(long& val, int columnIdx) const = 0;
	virtual SHVBool GetInt64(SHVInt64Val& val, int columnIdx) const = 0;
	virtual SHVBool GetDouble(double& val, int columnIdx) const = 0;
	virtual SHVBool GetStringUTF8C(SHVStringUTF8C& text, int& len, int columnIdx) const = 0;
	inline SHVBool GetString(SHVString& text, int& len, int columnIdx) const;

	// GetColumnName
	virtual SHVBool GetColumnNameUTF8C(SHVStringUTF8C& name, int columnIdx) const = 0;
	virtual SHVBool GetColumnNameUTF8(SHVStringUTF8& name, int columnIdx) const = 0;
	virtual SHVBool GetColumnName8(SHVString8& name, int columnIdx) const = 0;
	virtual SHVBool GetColumnName16(SHVString16& name, int columnIdx) const = 0;
	inline SHVBool GetColumnName(SHVString& name, int columnIdx) const;

	// GetColumnType
	virtual SHVBool GetColumnAffinity(short& type, int columnIdx) const = 0;
	virtual SHVBool GetColumnTypeUTF8C(SHVStringUTF8C& colType, int columnIdx) const = 0;
	virtual SHVBool GetColumnTypeUTF8(SHVStringUTF8& colType, int columnIdx) const = 0;
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

	virtual SHVBool GetParameterNameUTF8C(SHVStringUTF8C& name, int columnIdx) const = 0;
	virtual SHVBool GetParameterNameUTF8(SHVStringUTF8& name, int columnIdx) const = 0;
	virtual SHVBool GetParameterName8(SHVString8& name, int columnIdx) const = 0;
	virtual SHVBool GetParameterName16(SHVString16& name, int columnIdx) const = 0;
	inline SHVBool GetParameterName(SHVString& name, int columnIdx) const;

	virtual int GetParameterCount() const = 0;

	// Iteration methods
	virtual SHVBool NextResult() = 0;
	virtual SHVBool NextRow() = 0;
	virtual SHVBool Reset() = 0;
};
typedef SHVRefObjectContainer<SHVDataStatement> SHVDataStatementRef;


// ============================================ implementation ============================================ //
SHVBool SHVDataStatement::GetInt(int& val, int columnIdx) const
{
long lval;
SHVBool retVal = GetLong(lval, columnIdx);
	val = (int) lval;
	return retVal;
}

/*************************************
 * GetValue
 *************************************/
SHVBool SHVDataStatement::GetString(SHVString& text, int& len, int columnIdx) const
{
SHVStringUTF8C res(NULL);
	SHVBool retVal = GetStringUTF8C(res, len, columnIdx);
	if (retVal)
		text = res.ToStrT();
	return retVal;
}

/*************************************
 * GetColumnName
 *************************************/
SHVBool SHVDataStatement::GetColumnName(SHVString& name, int columnIdx) const
{
#if __SHVSTRINGDEFAULT == 8
	return GetColumnName8(name, columnIdx);
#elif __SHVSTRINGDEFAULT == 16
	return GetColumnName16(name, columnIdx);
#elif __SHVSTRINGDEFAULT == utf8
	return GetColumnNameUTF8(name, columnIdx);
#endif
}

/*************************************
 * GetColumnType
 *************************************/
SHVBool SHVDataStatement::GetColumnType(SHVString& colType, int columnIdx) const
{
#if __SHVSTRINGDEFAULT == 8
	return GetColumnType8(colType, columnIdx);
#elif __SHVSTRINGDEFAULT == 16
	return GetColumnType16(colType, columnIdx);
#elif __SHVSTRINGDEFAULT == utf8
	return GetColumnTypeUTF8(colType, columnIdx);
#endif
}

/*************************************
 * SetParameter
 *************************************/
inline SHVBool SHVDataStatement::SetParameterIntUTF8(const SHVStringUTF8C& name, int val)
{
	return SetParameterLongUTF8(name, val);
}

inline SHVBool SHVDataStatement::SetParameterInt(const SHVStringC& name, int val)
{
	return SetParameterLong(name, val);
}

SHVBool SHVDataStatement::SetParameterLong(const SHVStringC& name, long val)
{
	return SetParameterLongUTF8(name.ToStrUTF8(), val);
}

SHVBool SHVDataStatement::SetParameterInt64(const SHVStringC& name, SHVInt64Val val)
{
	return SetParameterInt64UTF8(name.ToStrUTF8(), val);
}

SHVBool SHVDataStatement::SetParameterDouble(const SHVStringC& name, double val)
{
	return SetParameterDoubleUTF8(name.ToStrUTF8(), val);
}

SHVBool SHVDataStatement::SetParameterString(const SHVStringC& name, const SHVStringC& val)
{
	return SetParameterStringUTF8(name.ToStrUTF8(), val.ToStrUTF8());
}

SHVBool SHVDataStatement::SetParameterNull(SHVStringC& name)
{
	return SetParameterNullUTF8(name.ToStrUTF8());
}

/*************************************
 * GetParameterName
 *************************************/
SHVBool SHVDataStatement::GetParameterName(SHVString& name, int columnIdx) const
{
#if __SHVSTRINGDEFAULT == 8
	return GetParameterName8(name, columnIdx);
#elif __SHVSTRINGDEFAULT == 16
	return GetParameterName16(name, columnIdx);
#elif __SHVSTRINGDEFAULT == utf8
	return GetParameterNameUTF8(name, columnIdx);
#endif
}


#endif
