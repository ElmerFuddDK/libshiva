#ifndef __SHIVA_UNITTEST_SRC_TESTQUEUED_H
#define __SHIVA_UNITTEST_SRC_TESTQUEUED_H

#include "shvtest.h"
#include "../../framework/shveventdata.h"
#include "../../utils/shvhashtable.h"


//-=========================================================================================================
/// SHVTestQueued class
/**
 * Inherit from this class to create a queued unit test.\n
 * A queued unit test will have it's actions performed in an event queue, most likely
 * a different thread than the main thread.\n
 * You will need to override at least 4 functions : GetID, GetGroup, GetTitle and GetActions.\n
 * Please look in the unittest example project for examples. All threadutils tests are queued.
 */

class SHVTestQueued : public SHVTest
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
	///\cond INTERNAL
	void OnPerformActionEvent(SHVEvent* event);

	SHVEventSubscriberBaseRef ResultSubscriber;
	SHVEventSubscriberBaseRef PerformSubscriber;

	SHVList<int> PerformingActions;
	///\endcond
};

#endif
