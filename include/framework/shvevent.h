#ifndef __SHIVA_FRAMEWORK_EVENT_H
#define __SHIVA_FRAMEWORK_EVENT_H

#include "../utils/shvrefobject.h"
#include "../threadutils/shvthreadbase.h"


class SHVEventTarget;
class SHVEventSubscriberBase;
class SHVModuleList;

//-=========================================================================================================
/// SHVEvent class - Contains implementation of a basic event
/**
 * This is the class that is sent on events in SHIVA. It is the base implementation, and contains the
 * basic funtionality for an event, such as type, ID and SubID.
 */

class SHVAPI SHVEvent : public SHVRefObject
{
public:

	enum Types {
		TypeBase = 0,
		TypeString
	};


	SHVEvent(SHVEventTarget* caller, SHVInt id, SHVInt subid = SHVInt(), SHVRefObject* obj = NULL);


	virtual Types GetType() const;

	// properties
	inline SHVInt GetID() const;
	inline SHVInt GetSubID() const;
	inline SHVRefObject* GetObject();
	inline SHVEventTarget* GetCaller();

	virtual void* GetData();


	// comparison
	inline static bool Equals(const SHVEvent* event, SHVInt id);
	inline static bool Equals(const SHVEvent* event, SHVInt id, SHVInt subid);


protected:
	///\cond INTERNAL
	SHVEventTarget* Caller;
	SHVInt ID, SubID;
	SHVRefObjectContainer<SHVRefObject> Object;
	///\endcond
};
typedef SHVRefObjectContainer<SHVEvent> SHVEventRef;


//-=========================================================================================================
/// SHVEventTarget class - Base class for event targets
/**
 * This is the class that is sent on events in SHIVA. It is the base implementation, and contains the
 * basic funtionality for an event, such as type, ID and SubID
 */

class SHVAPI SHVEventTarget
{
public:
	inline void SendEvent(SHVEvent* event);

protected:
	virtual void OnEvent(SHVEvent* event) = 0;
};


//-=========================================================================================================
/// SHVEventQueue class - Interface class for event queues
/**
 * This class it the interface for event queues.
 */

class SHVAPI SHVEventQueue
{
public:
	virtual void EnqueueEvent(SHVEvent* event, SHVEventSubscriberBase* subscriber) = 0;
	virtual void SignalDispatcher() = 0;
	virtual SHVThreadBase::ThreadID GetThreadID() = 0;

	virtual SHVBool LockEvent() = 0;
	virtual void UnlockEvent() = 0;


protected:
	SHVBool EventActiveInQueue(SHVModuleList& modules);
	void EventDeactivatedInQueue(SHVModuleList& modules);
};



// ======================================== implementation - SHVEvent ======================================= //

/*************************************
 * GetID
 *************************************/
/// Returns the ID associated with the event
/**
 * This proerty is the event identifier for base events. For
 * other event types it can be used for other things.
 */
SHVInt SHVEvent::GetID() const
{
	return ID;
}

/*************************************
 * GetSubID
 *************************************/
/// Returns the SubID associated with the event
/**
 * This property is the second level identifier
 * other event types it can be used for other things.
 */
SHVInt SHVEvent::GetSubID() const
{
	return SubID;
}

/*************************************
 * GetObject
 *************************************/
/// Returns the internal object of the event
SHVRefObject* SHVEvent::GetObject()
{
	return Object;
}

/*************************************
 * GetCaller
 *************************************/
/// Returns the caller of the event
SHVEventTarget* SHVEvent::GetCaller()
{
	return Caller;
}

/*************************************
 * Equals
 *************************************/
/// Compares an integer based id with the event
/**
 \param event Event to compare with
 \param id Identifier of event
 \return True if event is a base event and has given id
 *
 * Compares an event with an id, and returns true if the event
 * has the id specified, and is of the base type.\n
 * This is meant to be used when events are sent from a pre-
 * defined caller:
\code
void SHVMyModule::OnEvent(SHVEvent* event)
{
	if (event->GetCaller() == CallerModule) // CallerModule is a pointer to another module
	{
		if (SHVEvent::Equals(event, SHVCallerModule::EventTest))
		{
			printf("Received a test event from CallerModule\n");
		}
	}
}
\endcode
 */
bool SHVEvent::Equals(const SHVEvent* event, SHVInt id)
{
	return ( event->GetType() == SHVEvent::TypeBase && event->GetID() == id );
}

/*************************************
 * Equals
 *************************************/
/// Compares an integer based id and subid with the event
/**
 \param event Event to compare with
 \param id Identifier of event
 \param subid Second level identifier of event
 \return True if event is a base event and has given id/subid
 *
 \see SHVEvent::Equals(const SHVEvent* event, SHVInt id)
 */
bool SHVEvent::Equals(const SHVEvent* event, SHVInt id, SHVInt subid)
{
	return ( event->GetType() == SHVEvent::TypeBase && event->GetID() == id && event->GetSubID() == subid );
}



// ===================================== implementation - SHVEventTarget ==================================== //

/*************************************
 * SendEvent
 *************************************/
void SHVEventTarget::SendEvent(SHVEvent* event)
{
SHVEventRef e(event);
	OnEvent(event);
}


// ============================================== documentation ============================================= //

/** \fn void SHVEventTarget::SendEvent(SHVEvent* event)
 \brief Send an event to an event target
 */
/** \fn void virtual SHVEventTarget::OnEvent(SHVEvent* event)
 \brief Function for performing events
 */


/** \fn void SHVEventQueue::EnqueueEvent(SHVEvent* event, SHVEventSubscriberBase* subscriber)
 \brief Adds an event to the event queue
 */
/** \fn void SHVEventQueue::SignalDispatcher()
 \brief Signal the queue to start dispatching
 */
/** \fn SHVThreadBase::ThreadID SHVEventQueue::GetThreadID()
 \brief Get the thread id for the queue
 */
/** \fn SHVBool SHVEventQueue::LockEvent()
 \brief Locks the event queue
 *
 * You can use this function if you need to add multiple events to
 * the queue. It will avoid multiple lock/unlocks within the queue.\n
 * It will return false if the event queue is inactive, and can't
 * receive events. This will be the case when the module list isn't
 * registered.\n
 * This function is meant for "hit and run" usage. Never use it in
 * cases where other locking/blocking functions are called:
\code
// the wrong way
	if (queue1.LockEvent() && queue2.LockEvent()) // this is a big NONO - 2 locks at a time
	{
		queue1.EnqueueEvent(event1, subscriber);
		queue1.EnqueueEvent(event2, subscriber);
		queue2.EnqueueEvent(event1, subscriber);
		queue2.EnqueueEvent(event2, subscriber);
		queue1.UnlockEvent();
		queue2.UnlockEvent();
	}

// the right way
	if (queue1.LockEvent())
	{
		queue1.EnqueueEvent(event1, subscriber);
		queue1.EnqueueEvent(event2, subscriber);
		queue1.UnlockEvent();
	}
	if (queue2.LockEvent())
	{
		queue2.EnqueueEvent(event1, subscriber);
		queue2.EnqueueEvent(event2, subscriber);
		queue2.UnlockEvent();
	}
\endcode
 */
/** \fn void SHVEventQueue::UnlockEvent()
 \brief Unlocks the event queue again
 \see SHVEventQueue::LockEvent
*/

#endif
