#ifndef __SHIVA_FRAMEWORK_SHVMAINTHREADEVENTQUEUE_H
#define __SHIVA_FRAMEWORK_SHVMAINTHREADEVENTQUEUE_H

#include "../framework/shveventqueuelist.h"
#include "shvmodulelistimpl.h"


// forward declare
class SHVMainThreadEventDispatcher;


//-=========================================================================================================
/// SHVMainThreadEventQueue class - Interface for the main event queue
/**
 * Inherit from this interface if you want to implement a main thread event queue.
 * These queues are also responsible for setting up platformspecific stuff, like creating
 * a main window on window based platforms.\n
 * It is crucial that you implement the SHVMainThreadEventQueue::EventInternalStop event in the
 * SHVMainThreadEventQueue::OnEvent derived function, and set the SHVMainThreadEventQueue::Running flag
 * to false. This event will be emitted to this class automatically by the module list.\n
 * Any errors you want to be returned can be set by setting the SHVMainThreadEventQueue::RunReturnVal value.
 */

class SHVAPI SHVMainThreadEventQueue : public SHVEventQueue, public SHVEventTarget
{
public:


	// defines
	enum Events {
		EventInternalStop = -1	///< is emitted by the module list when the event loop needs to stop to this class
	};


	// constructor
	SHVMainThreadEventQueue(SHVMainThreadEventDispatcher* dispatcher);
	~SHVMainThreadEventQueue();


	// the module list
	inline SHVModuleListImpl& GetModuleList();
	

	// start the main thread event queue, and runs the application
	virtual SHVBool Run();


	// from SHVEventQueue
	virtual void EnqueueEvent(SHVEvent* event, SHVEventSubscriberBase* subscriber);
	virtual void SignalDispatcher();
	virtual SHVThreadBase::ThreadID GetThreadID();

	virtual SHVBool LockEvent();
	virtual void UnlockEvent();


	// from SHVEventTarget
	virtual void OnEvent(SHVEvent* event);


protected:
	
	// Event specific variables
	SHVEventQueueList EventList;
	SHVThreadBase::ThreadID ThreadID;
	SHVMainThreadEventDispatcher* Dispatcher;
	
	SHVBool RunReturnVal;
	SHVBool Running;
	
	///\cond INTERNAL
	friend class SHVMainThreadEventDispatcher;
	friend class SHVModuleListImpl;
	SHVModuleListImpl Modules;
	///\endcond
};

// ============================================= implementation ============================================= //

/** \fn void SHVMainThreadEventQueue::InitializeEventLoop()
 \brief Is called from Run in order to initialize the event loop
 *
 * When implementing a main thread event queue you can put code here to
 * initialize data used by the event loop and startup process.\n
 * On GUI based platforms this could be the main GUI window.\n
 * If an error should occur you can report this by setting the RunReturnVal
 * member to false or an error code.
 */

/** \fn void SHVMainThreadEventQueue::RunEventLoop()
 \brief Is called from Run after the module list has been started
 *
 * This is where you put your code to run the event loop. After the
 * Running boolean has been set to false you will need to exit when
 * returning to the loop.
 */

/*************************************
 * GetModuleList
 *************************************/
/// Obtains the module list for initializing modules before starting
SHVModuleListImpl& SHVMainThreadEventQueue::GetModuleList()
{
	return Modules;
}

#endif
