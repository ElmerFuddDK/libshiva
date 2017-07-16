#ifndef __SHIVA_FREETDS_WRAPPER_IMPL_H
#define __SHIVA_FREETDS_WRAPPER_IMPL_H

#include "shiva/include/freetds/shvfreetdswrapper.h"
#include "shiva/include/utils/shvstring.h"
#include "shiva/include/utils/shvvector.h"
#include "shiva/include/utils/shvvectorref.h"
#include "shiva/include/threadutils/shvmutex.h"
#include "shvfreetdsconnectionimpl.h"
#include "shvfreetdstransactionimpl.h"
///\cond INTERNAL
class SHVFreeTDSWrapperImpl;
typedef SHVRefObjectContainer<SHVFreeTDSWrapperImpl> SHVFreeTDSWrapperImplRef;
bool shiva_tds_wrapper_findbyproc(DBPROCESS* dbproc, SHVFreeTDSWrapperImplRef& wrapper, SHVFreeTDSConnectionImplRef& conn);
///\endcond


//-=========================================================================================================
///  SHVFreeTDSWrapperImpl class - Implement the shiva C++ interface for FreeTDS
/**
 */
	
class SHVFreeTDSWrapperImpl : public SHVFreeTDSWrapper
{
public:
	
	SHVFreeTDSWrapperImpl(SHVModuleList* modules);
	virtual ~SHVFreeTDSWrapperImpl();

	virtual SHVFreeTDSWrapper& SetProperty(SHVFreeTDSWrapper::Properties prop, const SHVStringC value);
	virtual SHVFreeTDSWrapper& SetProperty(const SHVStringC prop, const SHVStringC value);

	virtual const SHVStringCRef GetProperty(SHVFreeTDSWrapper::Properties prop);
	virtual const SHVStringCRef GetProperty(const SHVStringC prop);

	virtual bool PropertiesValid();
	
	virtual SHVFreeTDSConnection* CreateConnection();
	virtual SHVFreeTDSTransaction* CreateTransaction(SHVFreeTDSConnection* existingConnection, IsolationLevels lvl = IsolationLevelRepeatableRead, SHVInt maxRetries = SHVInt());
	
	virtual void InterruptAll();
	
	
private:
friend class SHVFreeTDSConnectionImpl;
friend class SHVFreeTDSTransactionImpl;
///\cond INTERNAL
friend bool shiva_tds_wrapper_findbyproc(DBPROCESS* dbproc, SHVFreeTDSWrapperImplRef& wrapper, SHVFreeTDSConnectionImplRef& conn);

	// Used internally, never use this from the outside!
	bool FindConnectionByProcInternal(DBPROCESS* dbproc, SHVFreeTDSConnectionImplRef& conn);
	// validates the pointer still exists as active connection and returns reference (Thread safe)
	bool ValidateConnectionIsActive(SHVFreeTDSConnectionImpl* ptr, SHVFreeTDSConnectionImplRef& conn);
///\endcond

	void DisconnectAll();

	void ValidateProps();
	SHVFreeTDSLoginRef GetLoginStruct();
	SHVBool ConnectInternal(SHVFreeTDSConnectionImpl* conn);
	bool RemoveActiveConnection(SHVFreeTDSConnectionImpl* conn);
	
	virtual const SHVStringUTF8CRef GetDbProperty(SHVFreeTDSWrapper::Properties prop);
	virtual const SHVStringUTF8CRef GetDbProperty(const SHVStringC prop);
	
	SHVModuleList* Modules;
	
	bool Initialized;
	
	short Hostport;
	SHVString Empty;

	SHVMutex PropertyLock;
	
	bool PropsValid;
	
	SHVFreeTDSLoginRef Login;
	
	struct StringPair {
		SHVString Property;
		SHVStringCRef Value;
		SHVStringUTF8CRef DbValue;
		inline StringPair(const SHVStringC prop, const SHVStringC value) : Property(prop), Value(value.GetSafeBuffer()), DbValue(value.ToStrUTF8().GetSafeBuffer()) {}
		inline void SetValue(const SHVStringC value) { Value = value.GetSafeBuffer(); DbValue = value.ToStrUTF8().GetSafeBuffer(); }
	};
	
	SHVVector<StringPair> Props;
	
	SHVVectorRef<SHVFreeTDSConnectionImpl> ActiveConnections;
};
#endif
