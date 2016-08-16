#ifndef __SHIVA_FREETDS_CONNECTION_H
#define __SHIVA_FREETDS_CONNECTION_H

#include "shiva/include/shvtypes.h"
#include "shiva/include/utils/shvstring.h"
#include "shiva/include/utils/shvstringutf8.h"
#include "shiva/include/utils/shvtime.h"
#include "shiva/include/utils/shvuuid.h"
#include "shiva/include/utils/shvrefobject.h"


//-=========================================================================================================
///  SHVFreeTDSConnection class - Connection via FreeTDS
/**
 */
	
class SHVFreeTDSConnection : public SHVRefObject
{
	
public:

	virtual SHVBool GetError() = 0;

	virtual SHVBool Connect() = 0;
	virtual void Disconnect() = 0;
	virtual bool IsConnected() = 0;
	
	// Performing a query
	virtual SHVBool ExecQuery(const SHVStringUTF8C query) = 0;
	
	// Iteration methods
	virtual SHVBool NextResult() = 0;
	virtual SHVBool NextRow() = 0;
	virtual SHVBool Reset() = 0;
	
	// GetValue
	inline  SHVBool GetInt(int& val, int columnIdx) const;
	virtual SHVBool GetLong(long& val, int columnIdx) const = 0;
	virtual SHVBool GetInt64(SHVInt64Val& val, int columnIdx) const = 0;
	virtual SHVBool GetDouble(double& val, int columnIdx) const = 0;
	virtual SHVBool GetStringUTF8(SHVStringUTF8& text, int columnIdx) const = 0;
	inline SHVBool GetString(SHVString& text, int columnIdx) const;
	virtual SHVBool GetTime(SHVTime& time, int columnIdx) const = 0;
	virtual SHVBool GetUUID(SHVUUID::ID& uuid, int columnIdx) const = 0;

	// GetColumnName
	virtual SHVBool GetColumnNameUTF8(SHVStringUTF8& name, int columnIdx) const = 0;
	virtual SHVBool GetColumnName8(SHVString8& name, int columnIdx) const = 0;
	virtual SHVBool GetColumnName16(SHVString16& name, int columnIdx) const = 0;
	inline SHVBool GetColumnName(SHVString& name, int columnIdx) const;

	// GetColumnType
	virtual SHVBool GetColumnTypeUTF8(SHVStringUTF8& colType, int columnIdx) const = 0;
	virtual SHVBool GetColumnType8(SHVString8& colType, int columnIdx) const = 0;
	virtual SHVBool GetColumnType16(SHVString16& colType, int columnIdx) const = 0;
	inline SHVBool GetColumnType(SHVString& colType, int columnIdx) const;

	// GetColumnCount
	virtual int GetColumnCount() const = 0;

};
typedef SHVRefObjectContainer<SHVFreeTDSConnection> SHVFreeTDSConnectionRef;


// ============================================ implementation ============================================ //

/*************************************
 * GetValue
 *************************************/
SHVBool SHVFreeTDSConnection::GetInt(int& val, int columnIdx) const
{
long lval;
SHVBool retVal = GetLong(lval, columnIdx);
	val = (int) lval;
	return retVal;
}
SHVBool SHVFreeTDSConnection::GetString(SHVString& text, int columnIdx) const
{
SHVStringUTF8 res(NULL);
SHVBool retVal = GetStringUTF8(res, columnIdx);
	if (retVal)
		text = res.ToStrT();
	return retVal;
}

/*************************************
 * GetColumnName
 *************************************/
SHVBool SHVFreeTDSConnection::GetColumnName(SHVString& name, int columnIdx) const
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
SHVBool SHVFreeTDSConnection::GetColumnType(SHVString& colType, int columnIdx) const
{
#ifdef UNICODE
	return GetColumnType16(colType, columnIdx);
#else
	return GetColumnType8(colType, columnIdx);
#endif
}

#endif
