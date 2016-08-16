#ifndef __SHIVA_FREETDS_WRAPPER_H
#define __SHIVA_FREETDS_WRAPPER_H

#include "shiva/include/utils/shvstringc.h"
#include "shiva/include/utils/shvptrcontainer.h"
#include "shiva/include/framework/shvevent.h"
#include "shvfreetdsconnection.h"


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
		ErrQuery
	};
	
	virtual SHVFreeTDSWrapper& SetProperty(Properties prop, const SHVStringC value) = 0;
	virtual SHVFreeTDSWrapper& SetProperty(const SHVStringC prop, const SHVStringC value) = 0;

	virtual const SHVStringCRef GetProperty(SHVFreeTDSWrapper::Properties prop) = 0;
	virtual const SHVStringCRef GetProperty(const SHVStringC prop) = 0;
	
	virtual bool PropertiesValid() = 0;

	virtual SHVFreeTDSConnection* CreateConnection(SHVEventSubscriberBase* subs) = 0;
	

protected:
	inline SHVFreeTDSWrapper() {}
};
typedef SHVRefObjectContainer<SHVFreeTDSWrapper> SHVFreeTDSWrapperRef;
#endif
