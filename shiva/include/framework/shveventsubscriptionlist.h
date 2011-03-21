#ifndef __SHIVA_FRAMEWORK_EVENTSUBSCRIPTIONLIST_H
#define __SHIVA_FRAMEWORK_EVENTSUBSCRIPTIONLIST_H

#include "shveventstring.h"
#include "shvmodulelist.h"

#include "../utils/shvlist.h"
#include "../utils/shvptrcontainer.h"


//-=========================================================================================================
/// SHVEventSubscriptionList class - Contains a list of subscriptions for a specific event
/**
 * Is used when a subscription is associated to an event.
 */

class SHVAPI SHVEventSubscriptionList
{
public:


	// constructor
	SHVEventSubscriptionList(SHVEvent* event);


	// Subscription functions
	void EventSubscribe(SHVEventSubscriberBase* subscriber);


	// Event functions
	SHVEvent* GetEvent();
	bool operator==(const SHVEvent* event);


	// Emitting events
	bool EmitEvent(SHVModuleList& modules, SHVEvent* event);


private:
	///\cond INTERNAL
	SHVList<SHVEventSubscriberBaseRef,SHVEventSubscriberBase*> EventSubscribers;
	SHVEventRef Event;
	///\endcond
};
typedef SHVPtrContainer<SHVEventSubscriptionList> SHVEventSubscriptionListPtr;

#endif
