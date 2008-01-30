#ifndef __SHIVA_FRAMEWORK_EVENTSUBSCRIBERBASE_H
#define __SHIVA_FRAMEWORK_EVENTSUBSCRIBERBASE_H

#include "shveventstring.h"

class SHVModuleList;


//-=========================================================================================================
/// SHVEventSubscriberBase class - Subscriber interface
/**
 * Base interface for event subscribers.
 */

class SHVAPI SHVEventSubscriberBase : public SHVRefObject
{
public:

	virtual SHVEventQueue* Emit(SHVModuleList& modules, SHVEvent* event) = 0;
	virtual void Perform(SHVEvent* event) = 0;

};
typedef SHVRefObjectContainer<SHVEventSubscriberBase> SHVEventSubscriberBaseRef;




// ============================================== documentation ============================================= //


/** \fn SHVEventQueue* SHVEventSubscriberBase::Emit(SHVModuleList& modules, SHVEvent* event)
 \brief Is called when dispatching an event - enqueues or performs the event
 \param modules reference to the module list
 \param event Event object to enqueue or perform
 \return the event queue the event was enqueued in or NULL
 *
 * This function is called when an event is emitted. it can do 2 things:
 \arg Enqueue an event into an SHVEventQueue object. This can be either
 *    the module list or another queue. It must return the queue afterwards
 *    in order to get it dispatched after emitting.
 \arg Perform an event. The event will be performed immediately and then
 *    return NULL, telling the emitter that there has been no events
 *    enqueued by this subscriber.
 *
 \note Never call SHVEventQueue::SignalDispatcher from here, since it is done
 *     by the emitter.
 */

/** \fn void SHVEventSubscriberBase::Perform(SHVEvent* event)
 \brief Will perform the event
 \param event Event object to enqueue or perform
 *
 * Will call the subscribing object itself.
 */

#endif
