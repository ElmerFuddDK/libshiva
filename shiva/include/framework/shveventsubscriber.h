#ifndef __SHIVA_FRAMEWORK_EVENTSUBSCRIBER_H
#define __SHIVA_FRAMEWORK_EVENTSUBSCRIBER_H

#include "shveventsubscriberbase.h"
#include "shvmodulelist.h"


//-=========================================================================================================
/// SHVEventSubscriber class - Default event subscriber
/**
 * Implements a subscriber for direct and queued subscriptions. The behavior if the subscriber depends on
 * whether it is constructed with a queue or not. If there is no queue given on construction it will
 * perform the events directly on emission, otherwise it will enqueue it.\n
 * if the directIfSameThread flag is enabled, the subscriber will only enqueue it if the event is emitted
 * from another thread. Thus ensuring the event is always performed from the same thread, but performed
 * immediately if possible.
 */

class SHVAPI SHVEventSubscriber : public SHVEventSubscriberBase
{
public:


	// constructor
	SHVEventSubscriber(SHVEventTarget* target, SHVEventQueue* queue = NULL, bool directIfSameThread = false);


	// inherited from SHVEventSubscriberBase
	virtual SHVEventQueue* Emit(SHVModuleList& modules, SHVEvent* event);
	virtual void Perform(SHVEvent* event);


protected:
	SHVEventTarget* Target;
	SHVEventQueue* Queue;
	bool DirectIfSameThread;
};
typedef SHVRefObjectContainer<SHVEventSubscriber> SHVEventSubscriberRef;



//-=========================================================================================================
/// SHVEventSubscriberFunc class - Template wrapper for event function calls
/**
 * Instead of calling SendEvent on a given event target it will call a function provided on the
 * constructor. Otherwise it functions exactly as SHVEventSubscriber
 */

template <class T>
class SHVEventSubscriberFunc : public SHVEventSubscriberBase
{
public:


	// defines
	typedef void (T::*OnEvent)(SHVEvent* event);


	// constructor
	SHVEventSubscriberFunc(T* target, OnEvent func, SHVEventQueue* queue = NULL, bool directIfSameThread = false);


	// inherited from SHVEventSubscriberBase
	virtual SHVEventQueue* Emit(SHVModuleList& modules, SHVEvent* event);
	virtual void Perform(SHVEvent* event);


protected:
	T* Target;
	OnEvent Func;
	SHVEventQueue* Queue;
	bool DirectIfSameThread;
};


// ============================================= implementation ============================================= //

/*************************************
 * Constructor
 *************************************/
template <class T>
SHVEventSubscriberFunc<T>::SHVEventSubscriberFunc(T* target, OnEvent func, SHVEventQueue* queue, bool directIfSameThread)
	: Target(target), Func(func), Queue(queue), DirectIfSameThread(directIfSameThread)
{
}

/*************************************
 * Emit
 *************************************/
template <class T>
SHVEventQueue* SHVEventSubscriberFunc<T>::Emit(SHVModuleList& modules, SHVEvent* event)
{
	SHVUNUSED_PARAM(modules);

	if ( Queue && (!DirectIfSameThread || SHVThreadBase::GetCurrentThreadID() != Queue->GetThreadID()) )
		Queue->EnqueueEvent(event,this);
	else
		Perform(event);

	return Queue;
}

/*************************************
 * Perform
 *************************************/
template <class T>
void SHVEventSubscriberFunc<T>::Perform(SHVEvent* event)
{
SHVEventRef e = event; // ensure the event reference is validated
	(Target->*Func)(event);
}

#endif
