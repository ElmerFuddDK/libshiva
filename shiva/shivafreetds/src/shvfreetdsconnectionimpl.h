#ifndef __SHIVA_FREETDS_CONNECTION_IMPL_H
#define __SHIVA_FREETDS_CONNECTION_IMPL_H

#include "shiva/include/framework/shveventsubscriberbase.h"
#include "shiva/include/freetds/shvfreetdsconnection.h"
#include "shvfreetdscommon.h"

class SHVFreeTDSWrapperImpl;
class SHVFreeTDSTransactionImpl;


//-=========================================================================================================
///  SHVFreeTDSConnectionImpl class - Implements the connection via FreeTDS
/**
 */
	
class SHVFreeTDSConnectionImpl : public SHVFreeTDSConnection
{
public:
	
	SHVFreeTDSConnectionImpl(SHVFreeTDSWrapperImpl* owner);
	virtual ~SHVFreeTDSConnectionImpl();

	virtual SHVBool IsOK();

	virtual SHVBool Connect();
	virtual void Disconnect();
	virtual bool IsConnected();
	virtual void Interrupt();
	virtual SHVBool Reset();

	// Performing a query
	virtual SHVBool ExecQuery(const SHVStringUTF8C query, bool clearMessagesAndSqlError);
	virtual SHVBool ExecQueryPartial(const SHVStringUTF8C query, SHVStringUTF8C* rest, bool clearMessagesAndSqlError);
	
	// Iteration methods
	virtual SHVBool NextResult();
	virtual SHVBool NextRow();
	
	virtual bool HasTDSMessage();
	virtual bool PopTDSMessage(TDSMessage& msg);
	virtual void ClearTDSMessages();
	
	// GetValue
	virtual SHVBool GetLong(long& val, int columnIdx) const;
	virtual SHVBool GetInt64(SHVInt64Val& val, int columnIdx) const;
	virtual SHVBool GetDouble(double& val, int columnIdx) const;
	virtual SHVBool GetStringUTF8(SHVStringUTF8& text, int columnIdx) const;
	virtual SHVBool GetTime(SHVTime& time, int columnIdx) const;
	virtual SHVBool GetUUID(SHVUUID::ID& uuid, int columnIdx) const;

	// GetColumnName
	virtual SHVBool GetColumnNameUTF8(SHVStringUTF8& name, int columnIdx) const;
	virtual SHVBool GetColumnName8(SHVString8& name, int columnIdx) const;
	virtual SHVBool GetColumnName16(SHVString16& name, int columnIdx) const;

	// GetColumnType
	virtual SHVBool GetColumnTypeUTF8(SHVStringUTF8& colType, int columnIdx) const;
	virtual SHVBool GetColumnType8(SHVString8& colType, int columnIdx) const;
	virtual SHVBool GetColumnType16(SHVString16& colType, int columnIdx) const;

	// GetColumnCount
	virtual int GetColumnCount() const;
	
	///\cond INTERNAL
	void HandleTdsErrorInternal(int severity, int dberr, int oserr, char* dberrstr, char* oserrstr);
	void HandleTdsMessageInternal(DBINT msgno, int msgstate, int severity, char* msgtext, char* srvname, char* procname, int line);
	void SetInterrupted();
	///\endcond

private:
friend class SHVFreeTDSWrapperImpl;
friend class SHVFreeTDSTransactionImpl;

	void DisconnectInternal();
	void CleanupCols();
	int CalculateRetryDelay(int tries);
	SHVBool SplitQuery(const SHVString8C query, SHVString8 &queryPart, SHVString8C &rest);

	size_t ConnectionThreadID;
	SHVFreeTDSWrapperImpl* Owner;
	SHVBool OK;
	SHVInt RandomSeed;
	
	///\cond INTERNAL
	SHVFreeTDSLoginRef Login;
	DBPROCESS* DbProc;
	
	struct TDSColumn {
		char* Name;
		int Type;
		enum ColTypes {
			TypeInt,
			TypeInt64,
			TypeDouble,
			TypeTime,
			TypeUUID,
			TypeString
		} InternalType;
		union {
			int* Int;
			SHVInt64Val* Int64;
			double* Double;
			DBDATETIME* Datetime;
			char* String;
			void* Buffer;
		} Data;
		int Size;
		int Status;
	};
	TDSColumn* Columns;
	int ColCount;
	
	SHVList<TDSMessage> SqlMessages;
	TDSErrorLevels SqlErrorLevel;

	RETCODE AllocateColumnBuffer(int i, TDSColumn* col);
	TDSColumn* ReleaseColumnsInternal();
	
	SHVTime DbDateToTime(const DBDATETIME* dateTime) const;
	SHVBool GetLongInternal(long& val, const TDSColumn& col) const;
	SHVBool GetInt64Internal(SHVInt64Val& val, const TDSColumn& col) const;
	SHVBool GetDoubleInternal(double& val, const TDSColumn& col) const;
	SHVBool GetStringUTF8Internal(SHVStringUTF8& text, const TDSColumn& col) const;
	SHVBool GetTimeInternal(SHVTime& val, const TDSColumn& col) const;
	SHVBool GetUUIDInternal(SHVUUID::ID& val, const TDSColumn& col) const;
	SHVBool GetColumnTypeUTF8Internal(SHVStringUTF8& colType, int internalType) const;
	SHVBool GetColumnType8Internal(SHVString8& colType, int internalType) const;
	SHVBool GetColumnType16Internal(SHVString16& colType, int internalType) const;
	///\endcond
};
typedef SHVRefObjectContainer<SHVFreeTDSConnectionImpl> SHVFreeTDSConnectionImplRef;

#endif
