#ifndef __SHIVA_FRAMEWORK_MODULELISTIMPL_H
#define __SHIVA_FRAMEWORK_MODULELISTIMPL_H

#include "shvconfigimpl.h"

#include "../framework/shvmodulelist.h"
#include "shvmaineventqueue.h"
#include "../framework/shveventemitter.h"
#include "../framework/shvtimer.h"

#include "../threadutils/shvmutex.h"
#include "../utils/shvlist.h"


class SHVModuleListIteratorImpl;

//-=========================================================================================================
/// SHVModuleListImpl class - implementation of module list
/**
 * Implements the module list. This is the core class of the framework.
 */

class SHVAPI SHVModuleListImpl : public SHVModuleList
{
public:


	// constructor
	SHVModuleListImpl(SHVMainEventQueue& defaultQueue);
	~SHVModuleListImpl();


	// Initialization
	SHVBool Initialize();


	// Parsing command line arguments
	static SHVBool ParseArgs(SHVConfig& cfg, const SHVStringC argString);
	static SHVBool ParseArgs(SHVConfig& cfg, int argc, char *argv[]);


	// Module management stuff
	SHVBool Start();
	void DestroyModules();


	// Module stuff
	virtual SHVBool AddModule(SHVModuleBase* module);
	virtual SHVModuleBase* ResolveModule(const SHVChar* name);
	virtual SHVModuleListIteratorPtr GetIterator(const SHVChar* group);


	// config stuff
	virtual SHVConfig& GetConfig(int cfgID = CfgDefault);


	// Tag stuff
	virtual SHVInt CreateTag();


	// Global event stuff
	virtual void EventSubscribe(SHVEvent* event, SHVEventSubscriberBase* subscriber);
	inline void EventSubscribe(SHVInt id, SHVEventSubscriberBase* subscriber);
	inline void EventSubscribe(const SHVChar* name, SHVEventSubscriberBase* subscriber);
	virtual SHVBool EmitEvent(SHVEvent* event);


	// Startup error handling
	virtual SHVBool AddStartupError(const SHVStringC& error);
	virtual SHVStringBuffer GetStartupErrors();


	// properties
	virtual SHVBool IsRegistered();
	virtual SHVBool IsUnregistering();


	// from SHVEventQueue
	virtual void EnqueueEvent(SHVEvent* event, SHVEventSubscriberBase* subscriber);
	virtual void SignalDispatcher();
	virtual SHVThreadBase::ThreadID GetThreadID();
	virtual SHVBool LockEvent();
	virtual void UnlockEvent();

	// Error value to return
	virtual void SetReturnError(SHVBool ret);
	virtual SHVBool GetReturnError();

	// Starts the shutdown process
	virtual void CloseApp();

protected:
	// for event control
	virtual SHVBool EventActiveInQueue();
	virtual void EventDeactivatedInQueue();

	virtual void OnEvent(SHVEvent* event);

private:

	SHVBool RegisterModules();
	void PreUnregisterModules();
	void UnregisterModules();

	///\cond INTERNAL
friend class SHVModuleListIteratorImpl;

	// Constants and typedefs
	enum States {
		StateInitializing,		// the state on creation
		StateInitialized,		// the state after initialize, and unregister
		StateStarting,			// the state between start and register
		StateRegistering,		// the state during register
		StateRunning,			// the state after register
		StateRunningClosing,	// the state after closeapp
		StateRunningPreUnreg,	// the state after preunregister
		StateUnregistering,		// the state during unregister
		StateUnregistered,		// the state after unregister
		StateError = 0xFF
	};
	typedef SHVList< SHVPtrContainer<SHVModuleBase> ,SHVModuleBase*> SHVInternalList;
	typedef SHVListIterator< SHVPtrContainer<SHVModuleBase> ,SHVModuleBase*> SHVInternalListIterator;

	enum {
		EventInternalRegister = -1,
		EventInternalUnregister = -2,
		EventInternalClose = -3
	};

	struct ShutdownDelay {
		SHVEventTarget* Target;
		SHVTimerInstanceRef Timer;
	};
	typedef SHVPtrContainer<ShutdownDelay> ShutdownDelayPtr;


	// modules
	SHVTimer* Timer;

	// variables
	States State;
	int TagSeed;
	SHVBool ReturnError;

	SHVEventSubscriberBaseRef InternalSubscriber;
	SHVMainEventQueue& DefaultEventQueue;
	SHVInternalList Modules;
	SHVEventEmitter Events;
	SHVConfigImpl Config;
	int EventsActive;
	SHVMutex ConfigLock;

	SHVHashTable<int, SHVPtrContainer<SHVConfigImpl> > UserConfigs;

	SHVList<SHVString,const SHVStringC> StartupErrors;

	SHVList<ShutdownDelayPtr,ShutdownDelay*> ShutdownDelays;

	SHVMutex Lock;

	void DelayShutdown(SHVEvent* event);
	void RemoveDelayedShutdowns(SHVEventTarget* target, SHVRefObject* timer);
	void DelayShutdownFinished(SHVEventTarget* target, SHVRefObject* timer);

	virtual void OnInternalEvent(SHVEvent* event);
	///\endcond
};



//-=========================================================================================================
/// SHVModuleListIteratorImpl class - implementation of module list iterator
/**
 * Contains implementation of the module list iterator interface
 */

class SHVAPI SHVModuleListIteratorImpl : public SHVModuleListIterator
{
friend class SHVModuleListImpl;
public:

	virtual operator SHVBool();
	virtual SHVBool MoveNext();
	virtual SHVModuleBase* Get();

private:
	SHVModuleListIteratorImpl(SHVModuleListImpl::SHVInternalList& list, const SHVChar* group);

	SHVModuleListImpl::SHVInternalListIterator List;
	SHVString8C Group;
};



// ============================================= implementation ============================================= //

/*************************************
 * EventSubscribe
 *************************************/
void SHVModuleListImpl::EventSubscribe(SHVInt id, SHVEventSubscriberBase* subscriber)
{
	EventSubscribe(new SHVEvent(NULL,id), subscriber);
}
void SHVModuleListImpl::EventSubscribe(const SHVChar* name, SHVEventSubscriberBase* subscriber)
{
	EventSubscribe(new SHVEventString(NULL,name), subscriber);
}
#endif
