#ifndef __SHIVA_FREETDS_CONNECTION_H
#define __SHIVA_FREETDS_CONNECTION_H

#include "shvfreetdsresultset.h"


//-=========================================================================================================
///  SHVFreeTDSConnection class - Connection via FreeTDS
/**
 * This object is not thread safe and must only be used by the
 * creating thread.\n
 * However, Interrupt is safe and necessary to use from other
 * threads to interrupt a runnning query.
 */
	
class SHVFreeTDSConnection : public SHVFreeTDSResultset
{
	
public:

	virtual SHVBool IsOK() = 0;

	virtual SHVBool Connect() = 0;
	virtual void Disconnect() = 0;
	virtual bool IsConnected() = 0;
	virtual void Interrupt() = 0;
	virtual SHVBool Reset() = 0;
	
	// Performing a query
	virtual SHVBool ExecQuery(const SHVStringUTF8C query, bool clearMessagesAndSqlError = true) = 0;
	virtual SHVBool ExecQueryPartial(const SHVStringUTF8C query, SHVStringUTF8C* rest = NULL, bool clearMessagesAndSqlError = true) = 0;
	
	
	// From SHVFreeTDSResultset
	
	// Iteration methods
	virtual SHVBool NextResult() = 0;
	virtual SHVBool NextRow() = 0;
	
	// Messages from the SQL server
	virtual bool HasTDSMessage() = 0;
	virtual bool PopTDSMessage(TDSMessage& msg) = 0;
	virtual void ClearTDSMessages() = 0;
	
	// GetValue
	virtual SHVBool GetLong(long& val, int columnIdx) const = 0;
	virtual SHVBool GetInt64(SHVInt64Val& val, int columnIdx) const = 0;
	virtual SHVBool GetDouble(double& val, int columnIdx) const = 0;
	virtual SHVBool GetStringUTF8(SHVStringUTF8& text, int columnIdx) const = 0;
	virtual SHVBool GetTime(SHVTime& val, int columnIdx) const = 0;
	virtual SHVBool GetUUID(SHVUUID::ID& val, int columnIdx) const = 0;

	// GetColumnName
	virtual SHVBool GetColumnNameUTF8(SHVStringUTF8& name, int columnIdx) const = 0;
	virtual SHVBool GetColumnName8(SHVString8& name, int columnIdx) const = 0;
	virtual SHVBool GetColumnName16(SHVString16& name, int columnIdx) const = 0;

	// GetColumnType
	virtual SHVBool GetColumnTypeUTF8(SHVStringUTF8& colType, int columnIdx) const = 0;
	virtual SHVBool GetColumnType8(SHVString8& colType, int columnIdx) const = 0;
	virtual SHVBool GetColumnType16(SHVString16& colType, int columnIdx) const = 0;

	// GetColumnCount
	virtual int GetColumnCount() const = 0;

};
typedef SHVRefObjectContainer<SHVFreeTDSConnection> SHVFreeTDSConnectionRef;

#endif
