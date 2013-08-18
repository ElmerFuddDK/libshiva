#ifndef __SHIVA_FRAMEWORK_EVENTQUEUELIST_H
#define __SHIVA_FRAMEWORK_EVENTQUEUELIST_H

#include "shvmodulelist.h"

#include "../threadutils/shvmutex.h"


//-=========================================================================================================
/// SHVEventQueueList class - contains the logic for handling event queues
/**
 * This class will handle the basic logic for enqueueing and distributing events in an event queue.
 */

class SHVAPI SHVEventQueueList
{
public:


	// constructor
	SHVEventQueueList();
	virtual ~SHVEventQueueList();


	// Event handling
	void EnqueueEvent(SHVModuleList& modules, SHVEvent* event, SHVEventSubscriberBase* subscriber);
	void DispatchEvents(SHVModuleList& modules);
	void Clear(SHVModuleList& modules);

	bool PendingEvents();


	bool LockEvent();
	void UnlockEvent();


protected:
	///\cond INTERNAL

	struct EventEntry {
		SHVEventRef Event;
		SHVEventSubscriberBaseRef Subscriber;

		inline EventEntry(SHVEvent* event, SHVEventSubscriberBase* subscriber) : Event(event), Subscriber(subscriber) {}
	};
	typedef SHVPtrContainer<EventEntry> EventEntryPtr;

	SHVList<EventEntryPtr,EventEntry*> EventList;

	SHVMutex EventLock;
	bool NeedDispatching;
	bool IsDispatching;

	///\endcond
	virtual void PerformEvent(SHVModuleList& modules, EventEntryPtr& entry);
	inline SHVBool EventActiveInQueue(SHVModuleList& modules) { return modules.EventActiveInQueue(); }
	inline void EventDeactivatedInQueue(SHVModuleList& modules) { modules.EventDeactivatedInQueue(); }
};

#endif
