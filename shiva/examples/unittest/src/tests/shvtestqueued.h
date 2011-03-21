#ifndef __SHIVA_TESTS_TESTQUEUED_H
#define __SHIVA_TESTS_TESTQUEUED_H

#include "shvtestbase.h"
#include "shiva/include/framework/shveventdata.h"
#include "shiva/include/utils/shvhashtable.h"


//-=========================================================================================================
/// SHVTestQueued class
/**
 * Inherit from this class to create a queued unit test.\n
 * A queued unit test will have it's actions performed in an event queue, most likely
 * a different thread than the main thread.\n
 * You will need to override at least 3 functions : GetID, GetTitle and GetActions.
 */

class SHVTestQueued : public SHVTestBase
{
public:

	struct PerformActionData
	{
		SHVModuleList* modules;
		const Action* action;
		inline PerformActionData() : modules(NULL), action(NULL) {}
	};
	typedef SHVEventData<PerformActionData> SHVEventAction;


	// From SHVTestBase
	virtual const SHVString8C GetGroup() const = 0;
	virtual const SHVString8C GetID() const = 0;
	virtual const SHVStringC GetTitle() const = 0;
	virtual const Action* GetActions() const = 0;

	virtual SHVBool Perform(SHVModuleList& modules, int actionFlags, SHVEventSubscriberBase* result);

	virtual bool IsPerforming();

	// For retrieving the queue
	virtual SHVEventQueue* GetQueue() = 0;


protected:

	SHVTestQueued();

private:

	void OnPerformActionEvent(SHVEvent* event);

	SHVEventSubscriberBaseRef ResultSubscriber;
	SHVEventSubscriberBaseRef PerformSubscriber;

	SHVList<int> PerformingActions;
};

#endif
