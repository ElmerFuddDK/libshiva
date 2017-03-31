#ifndef __SHIVA_FREETDS_TRANSACTION_H
#define __SHIVA_FREETDS_TRANSACTION_H

#include "shvfreetdsresultset.h"


//-=========================================================================================================
///  SHVFreeTDSTransaction class - Perform transactions against the SQL server
/**
 * This class makes it easy to perform transactions against the
 * SQL server. You can add queries to it, and when you call
 * PerformTransaction it will send it all to the SQL server
 * encapsulated in a BEGIN/COMMIT TRANSACTION. All retriable
 * errors are automatically retried, and the results will be
 * cached. This way when you get a succesful return you will
 * know the result is performed correctly.\n
 * This object is not thread safe and must only be used by the
 * creating thread.
 */
	
class SHVFreeTDSTransaction : public SHVFreeTDSResultset
{
	
public:

	virtual SHVBool IsOK() = 0;

	virtual void Disconnect() = 0;
	virtual bool IsConnected() = 0;
	virtual void Interrupt() = 0;
	
	// Performing the transaction
	virtual SHVBool AddQuery(const SHVStringUTF8C query) = 0;
	virtual SHVBool PerformTransaction() = 0;
	
	// Iterate through added queries
	virtual SHVStringUTF8C GetQuery(size_t index) = 0;
	virtual size_t GetQueryCount() = 0;


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
typedef SHVRefObjectContainer<SHVFreeTDSTransaction> SHVFreeTDSTransactionRef;


#endif
