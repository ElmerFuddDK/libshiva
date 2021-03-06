#ifndef __SHIVA_MYSQL_RESULTSET_H
#define __SHIVA_MYSQL_RESULTSET_H

#include "../../include/utils/shvstring.h"
#include "../../include/utils/shvrefobject.h"
#include "../../include/utils/shvtime.h"
#include "shvmysqlconnection.h"

//-=========================================================================================================
/// SHVMySQLResultSet class - Row iterator
/**
 * This class is used to retrieve the resulting rows from the query
 */

class SHVMySQLResultSet: public SHVRefObject
{
public:
	
	enum Types
	{
		TypeLong,
        TypeInt64,
		TypeBool,
		TypeDouble,
        TypeString,
		TypeDateTime,
		TypeRaw
	};
		
	// GetValue
	virtual SHVBool GetLong(long& val, int columnIdx) const = 0;
	virtual SHVBool GetInt64(SHVInt64Val& val, int columnIdx) const = 0;
	virtual SHVBool GetDouble(double& val, int columnIdx) const = 0;
	virtual SHVBool GetStringUTF8(SHVStringUTF8& text, int columnIdx) const = 0;
	virtual SHVBool GetDateTime(SHVTime& val, int columnIdx) const = 0;
	virtual SHVBool GetBool(SHVBool& val, int columnIdx) const = 0;
	virtual const char* GetRawData(int columnIdx) const = 0;
	inline SHVBool GetString(SHVString& val, int columnIdx) const;

	// GetColumnName
	virtual SHVBool GetColumnNameUTF8(SHVStringUTF8& name, int columnIdx) const = 0;
	virtual SHVBool GetColumnName8(SHVString8& name, int columnIdx) const = 0;
	virtual SHVBool GetColumnName16(SHVString16& name, int columnIdx) const = 0;
	inline SHVBool GetColumnName(SHVString& name, int columnIdx) const;
	virtual int GetColumnCount() const = 0;
	
	virtual Types GetColumnType(int columnIdx) const = 0;
	virtual int GetNativeType(int columnIdx) const = 0;
	virtual int GetColumnLen(int columnIdx) const = 0;
	virtual SHVBool NextRow() = 0;
	virtual SHVBool NextResult() = 0;
	virtual void Reset() = 0;
	virtual SHVBool IsOk() const = 0;
protected:
	virtual SHVMySQLConnection* GetConnection() = 0;
	inline void* GetProvider();
};

typedef SHVRefObjectContainer<SHVMySQLResultSet> SHVMySQLResultSetRef;

// ============================================= implementation ============================================= //
/*************************************
 * GetProvider
 *************************************/
void* SHVMySQLResultSet::GetProvider()
{
  return GetConnection()->GetProvider();
}

/*************************************
 * GetValue
 *************************************/
SHVBool SHVMySQLResultSet::GetString(SHVString& val, int columnIdx) const
{
#if __SHVSTRINGDEFAULT == utf8
	return GetStringUTF8(val, columnIdx);
#else
SHVStringUTF8 res;
	SHVBool retVal = GetStringUTF8(res, columnIdx);
	if (retVal)
		val = res.ToStrT();
	return retVal;
#endif
}

/*************************************
 * GetColumnName
 *************************************/
SHVBool SHVMySQLResultSet::GetColumnName(SHVString& name, int columnIdx) const
{
#if __SHVSTRINGDEFAULT == 8
	return GetColumnName8(name, columnIdx);
#elif __SHVSTRINGDEFAULT == 16
	return GetColumnName16(name, columnIdx);
#elif __SHVSTRINGDEFAULT == utf8
	return GetColumnNameUTF8(name, columnIdx);
#endif
}

#endif
