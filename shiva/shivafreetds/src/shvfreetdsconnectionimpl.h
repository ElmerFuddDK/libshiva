#ifndef __SHIVA_FREETDS_CONNECTION_IMPL_H
#define __SHIVA_FREETDS_CONNECTION_IMPL_H

#include "shiva/include/framework/shveventsubscriberbase.h"
#include "shiva/include/freetds/shvfreetdsconnection.h"
#include "shvfreetdscommon.h"

class SHVFreeTDSWrapperImpl;


//-=========================================================================================================
///  SHVFreeTDSConnectionImpl class - Implements the connection via FreeTDS
/**
 */
	
class SHVFreeTDSConnectionImpl : public SHVFreeTDSConnection
{
public:
	
	SHVFreeTDSConnectionImpl(SHVFreeTDSWrapperImpl* owner, SHVEventSubscriberBase* subs);
	virtual ~SHVFreeTDSConnectionImpl();

	virtual SHVBool GetError();

	virtual SHVBool Connect();
	virtual void Disconnect();
	virtual bool IsConnected();

	// Performing a query
	virtual SHVBool ExecQuery(const SHVStringUTF8C query);
	
	// Iteration methods
	virtual SHVBool NextResult();
	virtual SHVBool NextRow();
	virtual SHVBool Reset();
	
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
	///\endcond

private:
friend class SHVFreeTDSWrapperImpl;

	void DisconnectInternal();
	void CleanupCols();
	SHVTime DbDateToTime(const DBDATETIME* dateTime) const;

	SHVFreeTDSWrapperImpl* Owner;
	SHVEventSubscriberBaseRef Subscriber;
	SHVBool OK;
	
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
	
	///\endcond
};
typedef SHVRefObjectContainer<SHVFreeTDSConnectionImpl> SHVFreeTDSConnectionImplRef;

#endif
