#include "stdafx.h"
#include "../../platformspc.h"
#include "../../framework/shveventsubscriber.h"

#include "shvtestqueued.h"

//=========================================================================================================
// SHVTestQueued class
//=========================================================================================================
/// \class SHVTestQueued shvtestqueued.h "shiva/include/unittest/src/shvtestqueued.h"

/*************************************
 * Constructor
 *************************************/
SHVTestQueued::SHVTestQueued() : SHVTest()
{
}

/*************************************
 * Perform
 *************************************/
/// Perform tests
/**
 \param modules Module list for the application
 \param actionFlags Test actions to perform
 \param result Event subscriber to emit the result to
 *
 * This function will queue up action events on the event queue
 * returned by GetQueue(). Otherwise the performing functions
 * will be called as normal.
 \see SHVTestBase::Perform
 */
SHVBool SHVTestQueued::Perform(SHVModuleList& modules, int actionFlags, SHVEventSubscriberBase* result)
{
	LogLock.Lock();
	if (InitializePerform())
	{
	SHVEventQueue* q;
		ResultSubscriber = result; // will mark us as "performing"
		if (PerformSubscriber.IsNull())
			PerformSubscriber = new SHVEventSubscriberFunc<SHVTestQueued>(this,&SHVTestQueued::OnPerformActionEvent,GetQueue());
		LogLock.Unlock();

		for (const Action* actions = GetActions(); actions && actions->Flag; actions++)
		{
		int i=0;
			if ((actionFlags&actions->Flag) && actions->Func)
			{
			SHVEventAction* event = new SHVEventAction(NULL,SHVInt(++i));
				SHVEventAction::Get(event).modules = &modules;
				SHVEventAction::Get(event).action = actions;
				PerformingActions.AddTail(i);
				q = PerformSubscriber->Emit(modules,event);
			}
		}

		if (q)
			q->SignalDispatcher();
		else
			EmitPerformedEvent(modules,result);

		return SHVBool::True;
	}
	else
	{
		LogLock.Unlock();
	}

	return SHVBool::False;
}

/*************************************
 * IsPerforming
 *************************************/
bool SHVTestQueued::IsPerforming()
{
	return (!ResultSubscriber.IsNull());
}

///\cond INTERNAL
/*************************************
 * OnPerformActionEvent
 *************************************/
void SHVTestQueued::OnPerformActionEvent(SHVEvent* event)
{
PerformActionData& data = SHVEventAction::Get(event);
SHVListPos pos = PerformingActions.Find(event->GetID());

	PerformAction(*data.modules,data.action);
	PerformingActions.RemoveAt(pos);

	if (PerformingActions.GetCount() == 0)
	{
	SHVEventSubscriberBaseRef subs = ResultSubscriber;
		ResultSubscriber = NULL;
		EmitPerformedEvent(*data.modules,subs);
	}
}
///\endcond

/// \struct SHVTestQueued::PerformActionData shvtestqueued.h "shiva/include/unittest/src/shvtestqueued.h"
