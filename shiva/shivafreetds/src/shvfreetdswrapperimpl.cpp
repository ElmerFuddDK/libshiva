#include "stdafx.h"
#include "shiva/include/platformspc.h"
#include "shiva/include/framework/shvmodulelist.h"
#include "shiva/include/threadutils/shvmutexlocker.h"

#include "shvfreetdswrapperimpl.h"

///\cond INTERNAL
#define SHIVATDSWRAPPERS_GROWSIZE 5
static SHVMutexBase shiva_tds_wrappers_lock;
static SHVVectorBase shiva_tds_wrappers;
static int shiva_tds_error_handler(DBPROCESS* dbproc, int severity, int dberr, int oserr, char* dberrstr, char* oserrstr);
static int shiva_tds_message_handler(DBPROCESS* dbproc, DBINT msgno, int msgstate, int severity, char* msgtext, char* srvname, char *procname, int line);
static bool shiva_tds_wrapper_findbyproc(DBPROCESS* dbproc, SHVFreeTDSWrapperImplRef& wrapper, SHVFreeTDSConnectionImplRef& conn);
static bool shiva_tds_wrapper_add(SHVFreeTDSWrapperImpl* wrapper);
static void shiva_tds_wrapper_rem(SHVFreeTDSWrapperImpl* wrapper);
///\endcond


// =========================================================================================================
//  SHVFreeTDSWrapperImpl class - Implement the shiva C++ interface for FreeTDS
// =========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVFreeTDSWrapperImpl::SHVFreeTDSWrapperImpl(SHVModuleList* modules) : Modules(modules)
{
	Initialized = shiva_tds_wrapper_add(this);
	
	Hostport = 1433;
	PropsValid = false;
	
	// Default property types - MUST match the enum in SHVFreeTDSWrapper::Properties
	Props.Add(new StringPair(_S("hostname"),NULL));
	Props.Add(new StringPair(_S("hostport"),SHVStringC::LongToString(Hostport)));
	Props.Add(new StringPair(_S("username"),NULL));
	Props.Add(new StringPair(_S("password"),NULL));
	Props.Add(new StringPair(_S("database"),NULL));
	Props.Add(new StringPair(_S("appname"),_S("ShivaFreeTDS")));
	
	// This will fail if we have added new properties to the enum, but forgot here
	SHVASSERT(Props.CalculateCount() == PropCount);
	
	SetProperty(_S("charset"),_S("utf8"));
}

/*************************************
 * Destructor
 *************************************/
SHVFreeTDSWrapperImpl::~SHVFreeTDSWrapperImpl()
{
	DisconnectAll();
	Props.Clear();
	if (Initialized)
		shiva_tds_wrapper_rem(this);
}

/*************************************
 * SetProperty
 *************************************/
SHVFreeTDSWrapper& SHVFreeTDSWrapperImpl::SetProperty(SHVFreeTDSWrapper::Properties prop, const SHVStringC value)
{
SHVMutexLocker l(PropertyLock);
	if (prop ==SHVFreeTDSWrapper::PropHostport)
		Hostport = (short)value.ToLong();
	
	if (prop < 0 || prop >= Props.CalculateCount())
	{
		SHVASSERT(false);
	}
	else
	{
		Props[prop]->SetValue(value);
		ValidateProps();
		Login = NULL;
	}
	
	return *this;
}
SHVFreeTDSWrapper& SHVFreeTDSWrapperImpl::SetProperty(const SHVStringC prop, const SHVStringC value)
{
SHVMutexLocker l(PropertyLock);
size_t count = Props.CalculateCount();
size_t i;

	for (i=0; i<count; i++)
	{
		if (Props[i]->Property == prop)
		{
			Props[i]->SetValue(value);
			if (i == PropHostport)
				Hostport = (short)value.ToLong();
			if (i < PropCount)
				ValidateProps();
			break;
		}
	}
	
	if (i==count)
		Props.Add(new SHVFreeTDSWrapperImpl::StringPair(prop,value));
	
	Login = NULL;

	return *this;
}

/*************************************
 * GetProperty
 *************************************/
const SHVStringCRef SHVFreeTDSWrapperImpl::GetProperty(SHVFreeTDSWrapper::Properties prop)
{
SHVMutexLocker l(PropertyLock);

	if (prop >= 0 || prop < Props.CalculateCount())
	{
		return Props[prop]->Value;
	}
	else
	{
		SHVASSERT(false);
	}
	
	return NULL;
}
const SHVStringCRef SHVFreeTDSWrapperImpl::GetProperty(const SHVStringC prop)
{
SHVMutexLocker l(PropertyLock);
size_t count = Props.CalculateCount();

	for (size_t i=0; i<count; i++)
	{
		if (Props[i]->Property == prop)
			return Props[i]->Value;
	}
	
	return NULL;
}

/*************************************
 * PropertiesValid
 *************************************/
bool SHVFreeTDSWrapperImpl::PropertiesValid()
{
	return PropsValid;
}

/*************************************
 * CreateConnection
 *************************************/
SHVFreeTDSConnection* SHVFreeTDSWrapperImpl::CreateConnection(SHVEventSubscriberBase* subs)
{
	return new SHVFreeTDSConnectionImpl(this, subs);
}

/*************************************
 * FindConnectionByProcInternal
 *************************************/
bool SHVFreeTDSWrapperImpl::FindConnectionByProcInternal(DBPROCESS* dbproc, SHVFreeTDSConnectionImplRef& conn)
{
bool retVal = false;
SHVMutexLocker l(PropertyLock);
size_t i,count;
	count = ActiveConnections.CalculateCount();
	for (i=0; i<count && !retVal; i++)
	{
		if (ActiveConnections[i] && ActiveConnections[i]->DbProc == dbproc)
		{
			conn = ActiveConnections[i];
			retVal = true;
		}
	}
	return retVal;
}

/*************************************
 * DisconnectAll
 *************************************/
void SHVFreeTDSWrapperImpl::DisconnectAll()
{
size_t count = ActiveConnections.CalculateCount();

	SHVASSERT(Modules->IsUnregistering());

	for (size_t i=0; i<count; i++)
	{
		if (ActiveConnections[i])
		{
			ActiveConnections[i]->DisconnectInternal();
			ActiveConnections[i]->Owner = NULL;
		}
	}
	ActiveConnections.Clear();
}

/*************************************
 * ValidateProps
 *************************************/
void SHVFreeTDSWrapperImpl::ValidateProps()
{
	PropsValid = !Props[PropHostname]->Value.IsEmpty()
				&& Hostport > 0
				&& !Props[PropHostname]->Value.IsEmpty()
			;
}

SHVFreeTDSLoginRef SHVFreeTDSWrapperImpl::GetLoginStruct()
{
SHVMutexLocker l(PropertyLock);

	if (Initialized && PropsValid && Login.IsNull())
	{
		Login = new SHVFreeTDSLogin(dblogin());
		
		if (Login->Login)
		{
		SHVStringUTF8CRef val;
			DBSETLAPP (Login->Login, Props[PropAppName]->DbValue.GetSafeBuffer());
			DBSETLUSER(Login->Login, Props[PropUsername]->DbValue.GetSafeBuffer());
			DBSETLPWD (Login->Login, Props[PropPassword]->DbValue.GetSafeBuffer());
			
			val = GetDbProperty(_S("charset"));
			if (!val.IsEmpty())
				DBSETLCHARSET(Login->Login, val.GetSafeBuffer());
			val = GetDbProperty(_S("language"));
			if (!val.IsEmpty())
				DBSETLNATLANG(Login->Login, val.GetSafeBuffer());
		}
		else
		{
			SHVASSERT(false); // Failed to allocate
			Login = NULL;
		}
	}
	return Login;
}

/*************************************
 * ConnectInternal
 *************************************/
SHVBool SHVFreeTDSWrapperImpl::ConnectInternal(SHVFreeTDSConnectionImpl* conn)
{
SHVBool retVal(ErrNone);

	if (Initialized &&  Modules->IsRegistered())
	{
		conn->DbProc = dbopen(Login->Login,GetDbProperty(SHVFreeTDSWrapper::PropHostname).GetSafeBuffer());
		if (!conn->DbProc)
		{
			retVal.SetError(ErrConnecting);
		}
		else if (dbuse(conn->DbProc, GetDbProperty(SHVFreeTDSWrapper::PropDatabase).GetSafeBuffer()) == FAIL)
		{
			retVal.SetError(ErrSelectingDb);
		}
	}
	else
	{
		retVal.SetError(ErrGeneric);
	}

	if (retVal)
	{
	SHVMutexLocker l(PropertyLock);
	size_t i,freePos,count;
		freePos = count = ActiveConnections.CalculateCount();
		for (i=0; i<count; i++)
		{
			if (ActiveConnections[i] == conn)
				break;
			else if (freePos == count && ActiveConnections[i] == NULL)
				freePos = i;
		}
		if (i == count)
		{
			if (freePos < count)
				ActiveConnections.Replace(freePos,conn);
			else
				ActiveConnections.Add(conn);
		}
	}
	
	return retVal;
}

/*************************************
 * RemoveActiveConnection
 *************************************/
bool SHVFreeTDSWrapperImpl::RemoveActiveConnection(SHVFreeTDSConnectionImpl *conn)
{
SHVMutexLocker l(PropertyLock);
size_t i,count = ActiveConnections.CalculateCount();
	for (i=0; i<count; i++)
	{
		if (ActiveConnections[i] == conn)
		{
			ActiveConnections.Replace(i,NULL);
			if (i+1 == count)
				ActiveConnections.Truncate();
			break;
		}
	}
	return true;
}

/*************************************
 * GetDbProperty
 *************************************/
const SHVStringUTF8CRef SHVFreeTDSWrapperImpl::GetDbProperty(SHVFreeTDSWrapper::Properties prop)
{
SHVMutexLocker l(PropertyLock);

	if (prop >= 0 || prop < Props.CalculateCount())
	{
		return Props[prop]->DbValue;
	}
	else
	{
		SHVASSERT(false);
	}
	
	return NULL;
}
const SHVStringUTF8CRef SHVFreeTDSWrapperImpl::GetDbProperty(const SHVStringC prop)
{
SHVMutexLocker l(PropertyLock);
size_t count = Props.CalculateCount();

	for (size_t i=0; i<count; i++)
	{
		if (Props[i]->Property == prop)
			return Props[i]->DbValue;
	}
	
	return NULL;
}


///\cond INTERNAL
static int shiva_tds_error_handler(DBPROCESS* dbproc, int severity, int dberr, int oserr, char* dberrstr, char* oserrstr)
{
SHVFreeTDSWrapperImplRef wrapper;
SHVFreeTDSConnectionImplRef conn;

	// Locate the active connection
	if (shiva_tds_wrapper_findbyproc(dbproc,wrapper,conn))
	{
		conn->HandleTdsErrorInternal(severity,dberr,oserr,dberrstr,oserrstr);
	}
	
	return INT_CANCEL;
}
 
static int shiva_tds_message_handler(DBPROCESS* dbproc, DBINT msgno, int msgstate, int severity, char* msgtext, char* srvname, char* procname, int line)
{
SHVFreeTDSWrapperImplRef wrapper;
SHVFreeTDSConnectionImplRef conn;

	// Locate the active connection
	if (shiva_tds_wrapper_findbyproc(dbproc,wrapper,conn))
	{
		conn->HandleTdsMessageInternal(msgno,msgstate,severity,msgtext,srvname,procname,line);
	}
	
	return 0;
}
static bool shiva_tds_wrapper_add(SHVFreeTDSWrapperImpl* wrapper)
{
size_t i,count;
bool initialized;

	shiva_tds_wrappers_lock.Lock();
	count = shiva_tds_wrappers.CalculateCount();
	
	if (count == 0)
	{
		initialized = (dbinit() != FAIL);
		
		if (initialized)
		{
			dberrhandle(shiva_tds_error_handler);
			dbmsghandle(shiva_tds_message_handler);
		}
	}
	else
	{
		initialized = true;
	}
	
	if (initialized)
	{
		for (i=0; i<count; i++)
		{
			if (shiva_tds_wrappers[i] == NULL)
			{
				shiva_tds_wrappers.Replace(i,wrapper);
				break;
			}
		}
		if (i == count)
			shiva_tds_wrappers.Add(wrapper,SHIVATDSWRAPPERS_GROWSIZE);
	}
	
	shiva_tds_wrappers_lock.Unlock();
	return initialized;
}
static void shiva_tds_wrapper_rem(SHVFreeTDSWrapperImpl* wrapper)
{
size_t i,count;
	shiva_tds_wrappers_lock.Lock();
	count = shiva_tds_wrappers.CalculateCount();
	for (i=0; i<count; i++)
	{
		if (shiva_tds_wrappers[i] == wrapper)
		{
			if (i+1 == count) // If it is the last, remove it directly
			{
				shiva_tds_wrappers.Replace(i,NULL);
			}
			else // Otherwise shuffle the last one to current pos and replace last with null
			{
				count--;
				shiva_tds_wrappers.Replace(i,shiva_tds_wrappers[count]);
				shiva_tds_wrappers.Replace(count,NULL);
			}
			shiva_tds_wrappers.Truncate(SHIVATDSWRAPPERS_GROWSIZE);
			break;
		}
	}
	shiva_tds_wrappers_lock.Unlock();
}
static bool shiva_tds_wrapper_findbyproc(DBPROCESS* dbproc, SHVFreeTDSWrapperImplRef& wrapper, SHVFreeTDSConnectionImplRef& conn)
{
bool retVal = false;
size_t i,count;
SHVFreeTDSWrapperImpl* wrapperIdx;
	shiva_tds_wrappers_lock.Lock();
	count = shiva_tds_wrappers.CalculateCount();
	for (i=0; i<count && !retVal; i++)
	{
		wrapperIdx = (SHVFreeTDSWrapperImpl*)shiva_tds_wrappers[i];
		if (wrapperIdx && wrapperIdx->FindConnectionByProcInternal(dbproc,conn))
		{
			wrapper = wrapperIdx;
			retVal = true;
		}
	}
	shiva_tds_wrappers_lock.Unlock();
	return retVal;
}
///\endcond
