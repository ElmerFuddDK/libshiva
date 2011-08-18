#ifndef __SHIVA_FRAMEWORK_SHVMAINEVENTQUEUE_H
#define __SHIVA_FRAMEWORK_SHVMAINEVENTQUEUE_H

#include "../framework/shveventqueuelist.h"


//-=========================================================================================================
/// SHVMainEventQueue class - Interface for the main event queue
/**
 * Abstract interface for handling the main event queue for a module list.
 \see SHVMainThreadEventQueue
 */

class SHVAPI SHVMainEventQueue : public SHVEventQueue, public SHVEventTarget
{
public:


	// defines
	enum Events {
		EventInternalStop = -1	///< is emitted by the module list when the event loop needs to stop to this class
	};


	// from SHVEventQueue
	virtual void EnqueueEvent(SHVEvent* event, SHVEventSubscriberBase* subscriber) = 0;
	virtual void SignalDispatcher() = 0;
	virtual SHVThreadBase::ThreadID GetThreadID() = 0;

	virtual SHVBool LockEvent() = 0;
	virtual void UnlockEvent() = 0;


	// from SHVEventTarget
	virtual void OnEvent(SHVEvent* event) = 0;


protected:
	inline SHVMainEventQueue() {}
};



// ============================================== documentation ============================================= //

/// \class SHVMainEventQueue shvmaineventqueue.h "shiva/include/framework/shvmaineventqueue.h"

#endif
