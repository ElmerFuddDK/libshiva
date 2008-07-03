#ifndef __SHIVA_SOCKETSERVERIMPL_SOCKETSERVERTHREAD_H
#define __SHIVA_SOCKETSERVERIMPL_SOCKETSERVERTHREAD_H

#include "shvsocketimpl.h"
#include "../../../include/framework/shveventqueuelist.h"
#include "../../../include/threadutils/shvthread.h"
#include "../../../include/threadutils/shvmutex.h"
#include "../../../include/framework/shveventdata.h"


class SHVSocketServerImpl;
///\cond INTERNAL
//=========================================================================================================
// SHVSocketServerThread class - The socket handling thread
//=========================================================================================================

class SHVSocketServerThread : public SHVEventQueue
{
public:

	// constructor
	SHVSocketServerThread(SHVSocketServerImpl* server);
	~SHVSocketServerThread();


	// thread handling
	SHVBool StartThread(SHVModuleList& modules);
	void StopThread();
	void WaitForTermination();

	// EventQueue functions
	virtual void EnqueueEvent(SHVEvent* event, SHVEventSubscriberBase* subscriber);
	virtual void SignalDispatcher();
	virtual SHVThreadBase::ThreadID GetThreadID();

	virtual SHVBool LockEvent();
	virtual void UnlockEvent();


private:

	void ThreadFunc();

	SHVSocketServerImpl* SocketServer;
	SHVThread<SHVSocketServerThread> SocketThread;
	SHVModuleList* Modules;
	bool KillSignal;
	bool QueueSignalled;
	
	SHVEventQueueList EventList;
	
#ifndef __SHIVA_WINCE
	int PipeSignal[2];
#endif
};
///\endcond

#endif
