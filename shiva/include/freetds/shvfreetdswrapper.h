#ifndef __SHIVA_FREETDS_WRAPPER_H
#define __SHIVA_FREETDS_WRAPPER_H

#include "shiva/include/utils/shvstringc.h"
#include "shiva/include/utils/shvptrcontainer.h"
#include "shiva/include/framework/shvevent.h"
#include "shvfreetdsconnection.h"
#include "shvfreetdstransaction.h"


//-=========================================================================================================
///  SHVFreeTDSWrapper class - C++ interface for FreeTDS
/**
 */
	
class SHVFreeTDSWrapper : public SHVRefObject
{
public:
	
	enum Properties {
		PropHostname = 0,
		PropHostport,
		PropUsername,
		PropPassword,
		PropDatabase,
		PropAppName,
		PropCount // Holds the number of properties - not really a property
	};
	
	enum Errors {
		ErrNone = SHVBool::True,
		ErrGeneric,
		ErrInitFail,
		ErrConnecting,
		ErrSelectingDb,
		ErrNotConnected,
		ErrQuery,
		ErrQueryRetriable,
		ErrInterrupted
	};
	
	enum IsolationLevels {
		IsolationLevelKeepExisting = 0,
		IsolationLevelReadUncommitted = 1,
		IsolationLevelReadCommitted = 2,
		IsolationLevelRepeatableRead = 3,
		IsolationLevelSerializable = 4,
		IsolationLevelSnapshot = 5
	};
	
	virtual SHVFreeTDSWrapper& SetProperty(Properties prop, const SHVStringC value) = 0;
	virtual SHVFreeTDSWrapper& SetProperty(const SHVStringC prop, const SHVStringC value) = 0;

	virtual const SHVStringCRef GetProperty(SHVFreeTDSWrapper::Properties prop) = 0;
	virtual const SHVStringCRef GetProperty(const SHVStringC prop) = 0;
	
	virtual bool PropertiesValid() = 0;

	virtual SHVFreeTDSConnection* CreateConnection() = 0;
	virtual SHVFreeTDSTransaction* CreateTransaction(SHVFreeTDSConnection* existingConnection, IsolationLevels lvl = IsolationLevelRepeatableRead, SHVInt maxRetries = SHVInt()) = 0;
	
	virtual void InterruptAll() = 0; // Can be used to cancel all current queries


protected:
	inline SHVFreeTDSWrapper() {}
};
typedef SHVRefObjectContainer<SHVFreeTDSWrapper> SHVFreeTDSWrapperRef;
#endif
