#ifndef __SHIVA_FRAMEWORK_EVENTEMITTER_H
#define __SHIVA_FRAMEWORK_EVENTEMITTER_H

#include "shveventstring.h"
#include "shveventsubscriptionlist.h"
#include "shvmodulelist.h"

#include "../utils/shvlist.h"


//-=========================================================================================================
/// SHVEventEmitter class - Used for event registering and emitting events
/**
 * This class is used for registering event subscribers for specific events, as well as emitting them.
 */

class SHVAPI SHVEventEmitter
{
public:


	// constructor
	SHVEventEmitter();


	// Subscription functions
	void EventSubscribe(SHVEvent* event, SHVEventSubscriberBase* subscriber);


	// Emitting events
	bool EmitEvent(SHVModuleList& modules, SHVEvent* event);


	// Clear the subscriptions
	void Clear();

private:
	///\cond INTERNAL
	SHVList<SHVEventSubscriptionListPtr,SHVEventSubscriptionList*> EventSubscriptions;

	SHVEventSubscriptionList* Find(SHVEvent* event);
	///\endcond
};

#endif
