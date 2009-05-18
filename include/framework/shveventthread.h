#ifndef __SHIVA_FRAMEWORK_EVENTTHREAD_H
#define __SHIVA_FRAMEWORK_EVENTTHREAD_H

#include "shvmodulelist.h"
#include "shveventqueuelist.h"

#include "../threadutils/shvmutexbase.h"
#include "../threadutils/shvthread.h"


//-=========================================================================================================
/// SHVEventThread class - Implementation of a simple event thread
/**
 * Implements an event thread. It can be derived for special usage.\n
 * It is generally encouraged to always use an event thread when implementing worker threads
 * in SHIVA, since events are integrated into the entire framework. That way you don't have to
 * worry about shutting down the thread.
 */

class SHVAPI SHVEventThread : public SHVEventQueue
{
public:


	enum Events {
		EventStateStarting,
		EventStateStopping
	};


	// constructor
	SHVEventThread(SHVEventSubscriberBase* stateSubscriber = NULL);
	~SHVEventThread();


	// thread handling
	void StartThread(SHVModuleList& modules);
	void StopThread();
	void WaitForTermination();


	// EventQueue functions
	virtual void EnqueueEvent(SHVEvent* event, SHVEventSubscriberBase* subscriber);
	virtual void SignalDispatcher();
	virtual SHVThreadBase::ThreadID GetThreadID();

	virtual SHVBool LockEvent();
	virtual void UnlockEvent();


protected:

	void SetSignalTimeout(int interval=SHVMutexBase::Infinite);

	// stubbed functions for inheritance
	virtual void ThreadStarting();
	virtual void ThreadStopping();
	virtual void PreEventDispatch();
	virtual void PostEventDispatch();

private:
	///\cond INTERNAL

	SHVModuleList* Modules;

	SHVMutexBase ThreadSignal;
	SHVThread<SHVEventThread> EventThread;
	bool KillSignal;
	int TimeoutInterval;

	SHVEventQueueList EventList;
	SHVEventSubscriberBaseRef StateSubscriber;

	void ThreadFunc();

	///\endcond
};

#endif
