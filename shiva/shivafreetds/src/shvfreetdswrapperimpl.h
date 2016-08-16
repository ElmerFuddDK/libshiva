#ifndef __SHIVA_FREETDS_WRAPPER_IMPL_H
#define __SHIVA_FREETDS_WRAPPER_IMPL_H

#include "shiva/include/freetds/shvfreetdswrapper.h"
#include "shiva/include/utils/shvstring.h"
#include "shiva/include/utils/shvstringutf8.h"
#include "shiva/include/utils/shvvector.h"
#include "shiva/include/utils/shvvectorref.h"
#include "shiva/include/threadutils/shvmutex.h"
#include "shvfreetdsconnectionimpl.h"


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
	
	virtual SHVFreeTDSConnection* CreateConnection(SHVEventSubscriberBase* subs);

/*	virtual SHVBool OpenUTF8(const SHVStringUTF8C& fileName, int option = 6);
	virtual SHVBool OpenInMemory();
	virtual SHVBool SetBusyTimeout(int ms);
	virtual SHVBool Close();
	virtual int GetRecentChanges();
	virtual int GetTotalChanges();

	virtual SHVSQLiteStatement* PrepareUTF8(SHVBool& ok, const SHVStringUTF8C& sql, SHVStringSQLite& notparsed);
	virtual SHVSQLiteStatementRef ExecuteUTF8(SHVBool& ok, const SHVStringUTF8C& sql, SHVStringSQLite& notparsed);
	virtual SHVStringUTF8C GetErrorMsgUTF8(); */
	
	bool FindConnectionByProcInternal(DBPROCESS* dbproc, SHVFreeTDSConnectionImplRef& conn);
	
private:
friend class SHVFreeTDSConnectionImpl;

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
typedef SHVRefObjectContainer<SHVFreeTDSWrapperImpl> SHVFreeTDSWrapperImplRef;
#endif
