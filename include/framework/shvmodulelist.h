#ifndef __SHIVA_FRAMEWORK_MODULELIST_H
#define __SHIVA_FRAMEWORK_MODULELIST_H

#include "shvmodulebase.h"
#include "shvconfig.h"



// ============================================= Global events ============================================== //

#define __EVENT_GLOBAL_DELAYSHUTDOWN         "DelayShutdown"	///< emit this to delay closing or unregistering -
																///< ID is delay in millisecs or 0 for infinite
#define __EVENT_GLOBAL_DELAYSHUTDOWNFINISHED "DelayShutdownFinished" ///< send this to cancel delay shutdown


class SHVModuleListIterator;
class SHVEventQueueList;
typedef SHVPtrContainer<SHVModuleListIterator> SHVModuleListIteratorPtr;

//-=========================================================================================================
/// SHVModuleList class - interface for the module list
/**
 * Contains the interface for the module list. This is the glue og the system, since it contains all modules
 * and controls the state of them.\n
 * All events from the module list itself is emitted with caller as itself.\n
 * It is used for the following tasks:
 \arg Module containment, resolving, and state maintenance (register/unregister)
 \arg Global events - emitting and registering
 \arg General event system handling - Locking when emitting, contains a list of global
 *    event queues, as well as the default event queue.
 */

class SHVAPI SHVModuleList : public SHVEventQueue, public SHVEventTarget
{
public:


	// defines
	enum Events {
		EventPreRegister,		///< is emitted before the register round
		EventEndRegister,		///< is emitted after registering modules
		EventPreUnregister,		///< is emitted during PreUnregister
		EventClosing			///< is emitted when closing - use this to delay shutdown
	};
	enum Configs {
		CfgDefault = 0,
		CfgGUI = 1,
		CfgUser = 10
	}; 
	enum DefaultConfigOptions {
		DefaultCfgArgc,				///< On supported platforms this is argument count
		DefaultCfgArgv				///< On supported platforms this is argument list (char**)
	};


	// Module stuff
	virtual SHVBool AddModule(SHVModuleBase* module) = 0;
	virtual SHVModuleBase* ResolveModule(const SHVChar* name) = 0;
	virtual SHVModuleListIteratorPtr GetIterator(const SHVChar* group) = 0;


	// config stuff
	virtual SHVConfig& GetConfig(int cfgID = CfgDefault) = 0;


	// Tag stuff
	virtual SHVInt CreateTag() = 0;


	// Global event stuff
	virtual void EventSubscribe(SHVEvent* event, SHVEventSubscriberBase* subscriber) = 0;
	inline void EventSubscribe(SHVInt id, SHVEventSubscriberBase* subscriber);
	inline void EventSubscribe(const SHVChar* name, SHVEventSubscriberBase* subscriber);
	virtual SHVBool EmitEvent(SHVEvent* event) = 0;


	// Startup error handling
	virtual SHVBool AddStartupError(const SHVStringC& error) = 0;
	virtual SHVStringBuffer GetStartupErrors() = 0;


	// properties
	virtual SHVBool IsRegistered() = 0;
	virtual SHVBool IsUnregistering() = 0;


	// from SHVEventQueue
	virtual void EnqueueEvent(SHVEvent* event, SHVEventSubscriberBase* subscriber) = 0;
	virtual void SignalDispatcher() = 0;
	virtual SHVThreadBase::ThreadID GetThreadID() = 0;
	virtual SHVBool LockEvent() = 0;
	virtual void UnlockEvent() = 0;


	// Starts the shutdown process
	virtual void CloseApp() = 0;


	// Version check against libshiva
	static SHVBool CheckVersion(int major, int minor, int release);


protected:
friend class SHVEventQueue;
friend class SHVEventQueueList;

	// for event control
	virtual SHVBool EventActiveInQueue() = 0;
	virtual void EventDeactivatedInQueue() = 0;


	// from event target
	virtual void OnEvent(SHVEvent* event) = 0;
};



//-=========================================================================================================
/// SHVModuleListIterator class - interface for iterating the module list
/**
 * This class is returned from the GetIterator function on the module list. It is used for
 * running through the list for modules within a given group.
 */

class SHVModuleListIterator
{
public:
	virtual operator SHVBool() = 0;
	virtual SHVBool MoveNext() = 0;
	virtual SHVModuleBase* Get() = 0;
};



//-=========================================================================================================
/// SHVModuleResolver class - convenience class for resolving modules
/**
 * This class is a nice convenience template class for resolving modules. Use this in your module
 * register functions:
\code
SHVBool SHVMyModule::Register()
{
	if (!SHVModuleResolver<SHVMyOtherModule>(Modules,MyOtherModule,"MyOtherModule"))
		return false;

	return SHVModule::Register();
}
\endcode
 */

template<class T>
class SHVModuleResolver
{
public:


	// construction
	inline SHVModuleResolver(SHVModuleList& modules, T*& ptr, const SHVChar* name);


	// operator
	inline operator bool ();


private:
	///\cond INTERNAL
	bool Resolved;
	///\endcond
};



// ============================================= implementation ============================================= //

template<class T>
SHVModuleResolver<T>::SHVModuleResolver(SHVModuleList& modules, T*& ptr, const SHVChar* name)
{
	ptr = (T*)modules.ResolveModule(name);
	Resolved = (ptr ? true : false);
}

template<class T>
SHVModuleResolver<T>::operator bool ()
{
	return Resolved;
}


/*************************************
 * EventSubscribe
 *************************************/
/// Event subscription overload
/**
 \see SHVModuleList::EventSubscribe(SHVEvent* event, SHVEventSubscriberBase* subscriber)
 */
void SHVModuleList::EventSubscribe(SHVInt id, SHVEventSubscriberBase* subscriber)
{
	EventSubscribe(new SHVEvent(NULL,id), subscriber);
}
/// Event subscription overload
/**
 \see SHVModuleList::EventSubscribe(SHVEvent* event, SHVEventSubscriberBase* subscriber)
 */
void SHVModuleList::EventSubscribe(const SHVChar* name, SHVEventSubscriberBase* subscriber)
{
	EventSubscribe(new SHVEventString(NULL,name), subscriber);
}



// ============================================== documentation ============================================= //


// Module stuff
/** \fn SHVBool SHVModuleList::AddModule(SHVModuleBase* module)
 \brief Add a module to the list
 */
/** \fn SHVModuleBase* SHVModuleList::ResolveModule(const SHVChar* name)
 \brief Resolve a module from the list
 */
/** \fn SHVModuleListIteratorPtr SHVModuleList::GetIterator(const SHVChar* group)
 \brief Return an iterator for a group of modules
 */


// config stuff
/** \fn SHVConfig& SHVModuleList::GetConfig(int cfgID)
 \brief Return global or a user defined config
 */


// Tag stuff
/** \fn SHVInt SHVModuleList::CreateTag()
 \brief Returns a unique tag for identification
 */


// Global event stuff
/** \fn void SHVModuleList::EventSubscribe(SHVEvent* event, SHVEventSubscriberBase* subscriber)
 \brief Is called in order to subscribe on an event this module emits
 \param event Event object identifying the event to be emitted
 \param subscriber Pointer to subscriber to be used when emitting
 */
/** \fn SHVBool SHVModuleList::EmitEvent(SHVEvent* event)
 \brief Emit a global event
 */


// Startup error handling
/** \fn SHVBool SHVModuleList::AddStartupError(const SHVStringC& error)
 \brief Add an error during startup (register)
 \return Always returns false (convenience)
 */
/** \fn SHVStringBuffer SHVModuleList::GetStartupErrors()
 \brief Returns the startup errors in a combined buffer
 */


// properties
/** \fn SHVBool SHVModuleList::IsRegistered()
 \brief Get registered state
 */
/** \fn SHVBool SHVModuleList::IsUnregistering()
 \brief Get destroying state
 */


// for event control
/** \fn SHVBool SHVModuleList::EventActiveInQueue()
 \brief signals the module list that an event has been enqueued
 \return true if the event queue is active
 *
 * Call this function from an event queue before enqueueing.
 \see SHVEventQueue::EventActiveInQueue
 */
/** \fn void SHVModuleList::EventDeactivatedInQueue()
 \brief signals the module list that an event has been performed from a queue
 \see SHVEventQueue::EventActiveInQueue
 */


#endif
