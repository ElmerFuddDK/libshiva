#ifndef __SHIVA_FRAMEWORK_EVENTSUBSCRIBERDYNAMIC_H
#define __SHIVA_FRAMEWORK_EVENTSUBSCRIBERDYNAMIC_H

#include "shveventsubscriberbase.h"
#include "shveventtargetdynamic.h"
#include "../utils/shvvectorref.h"

class SHVModuleList;


//-=========================================================================================================
/// SHVEventSubscriberDynamic class - Subscriber class for dynamic targets
/**
 * Implements a subscriber to be used when dealing with dynamic event targets. This
 * subscriber acts as a host for dynamically allocated and deallocated event targets.\n
 * This subscriber does NOT keep a reference to the dynamic target.
 */

class SHVAPI SHVEventSubscriberDynamic : public SHVEventSubscriberBase
{
public:


	// constructor
	SHVEventSubscriberDynamic(SHVEventQueue* queue = NULL, bool directIfSameThread = false);
	virtual ~SHVEventSubscriberDynamic();


	// Subsciption for dynamic event targets
	virtual void Subscribe(SHVEventTargetDynamic* subscriber);
	virtual void Unsubscribe(SHVEventTargetDynamic* subscriber);

	// Query if the target is still in the subscriber
	virtual SHVBool ContainsSubscriber(SHVEventTargetDynamic* subscriber);
	

	// inherited from SHVEventSubscriberBase
	virtual SHVEventQueue* Emit(SHVModuleList& modules, SHVEvent* event);
	virtual void Perform(SHVEvent* event);


protected:
	///\cond INTERNAL
	enum { SubscriberGrowSize = 10 };
	SHVVectorBase Subscribers;
	SHVEventQueue* Queue;
	bool DirectIfSameThread;
	///\endcond
};
typedef SHVRefObjectContainer<SHVEventSubscriberDynamic> SHVEventSubscriberDynamicRef;

#endif
