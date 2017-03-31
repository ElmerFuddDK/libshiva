#ifndef __SHIVA_FREETDS_TRANSACTION_IMPL_H
#define __SHIVA_FREETDS_TRANSACTION_IMPL_H

#include "shiva/include/framework/shveventsubscriberbase.h"
#include "shiva/include/freetds/shvfreetdstransaction.h"
#include "shvfreetdsconnectionimpl.h"
#include "shiva/include/utils/shvvector.h"
#include "shiva/include/utils/shvlist.h"


//-=========================================================================================================
///  SHVFreeTDSTransactionImpl class
/**
 */
	
class SHVFreeTDSTransactionImpl : public SHVFreeTDSTransaction
{
public:
	
	SHVFreeTDSTransactionImpl(SHVFreeTDSConnectionImpl* connection, bool ownerOfConnection, int isolationLvl, SHVInt maxRetries);
	virtual ~SHVFreeTDSTransactionImpl();

	virtual SHVBool IsOK();

	virtual void Disconnect();
	virtual bool IsConnected();
	virtual void Interrupt();

	// Performing the transaction
	virtual SHVBool AddQuery(const SHVStringUTF8C query);
	virtual SHVBool PerformTransaction();
	
	// Iterate through added queries
	virtual SHVStringUTF8C GetQuery(size_t index);
	virtual size_t GetQueryCount();
	
	
	// From SHVFreeTDSResultset
	
	// Iteration methods
	virtual SHVBool NextResult();
	virtual SHVBool NextRow();
	
	// Messages from the SQL server
	virtual bool HasTDSMessage();
	virtual bool PopTDSMessage(TDSMessage& msg);
	virtual void ClearTDSMessages();
	
	// GetValue
	virtual SHVBool GetLong(long& val, int columnIdx) const;
	virtual SHVBool GetInt64(SHVInt64Val& val, int columnIdx) const;
	virtual SHVBool GetDouble(double& val, int columnIdx) const;
	virtual SHVBool GetStringUTF8(SHVStringUTF8& text, int columnIdx) const;
	virtual SHVBool GetTime(SHVTime& val, int columnIdx) const;
	virtual SHVBool GetUUID(SHVUUID::ID& val, int columnIdx) const;

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
	
private:
friend class SHVFreeTDSWrapperImpl;

	///\cond INTERNAL
	const char* GetIsolationLevelString();
	///\endcond

	SHVFreeTDSConnectionImplRef Connection;
	SHVFreeTDSWrapperImpl* Owner;
	bool OwnerOfConnection;
	int IsolationLevel;
	SHVInt MaxRetries;
	SHVBool OK;
	bool Performed;
	int Retries;
	
	SHVVector<SHVString8> Queries; // Is really UTF8 strings ... use .GetBufferConst to convert
	
	///\cond INTERNAL
	struct Result {
		SHVVector<SHVStringUTF8> ColNames;
		SHVVector<int> ColInternalTypes;
		SHVList<SHVFreeTDSConnectionImpl::TDSColumn*> Rows;
		Result(SHVFreeTDSConnectionImpl::TDSColumn* cols, int colCount);
		~Result();
	};
	SHVVector<Result> Resultset;
	size_t CurrentResult;
	SHVListPos CurrentRowPos;
	SHVFreeTDSConnectionImpl::TDSColumn* CurrentRow;
	int CurrentColCount;

	SHVList<SHVFreeTDSConnectionImpl::TDSMessage> SqlMessages;
	TDSErrorLevels SqlErrorLevel;
	///\endcond
};
typedef SHVRefObjectContainer<SHVFreeTDSTransactionImpl> SHVFreeTDSTransactionImplRef;

#endif
