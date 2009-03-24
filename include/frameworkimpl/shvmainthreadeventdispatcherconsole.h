#ifndef __SHIVA_FRAMEWORK_SHVMAINTHREADEVENTDISPATCHERCONSOLE_H
#define __SHIVA_FRAMEWORK_SHVMAINTHREADEVENTDISPATCHERCONSOLE_H

#include "../framework/shvmainthreadeventdispatcher.h"
#include "../threadutils/shvmutexbase.h"
#include "../utils/shvbuffercstream.h"

#include "../framework/shveventstdin.h"
#ifdef __SHIVA_WIN32
#include "../threadutils/shvthread.h"
#endif

//-=========================================================================================================
/// SHVMainThreadEventDispatcherConsole class - Dispatcher for console user IO
/**
 * This class implements a generic dispatcher that can be used for generic console
 * user IO.
 */

class SHVAPI SHVMainThreadEventDispatcherConsole : public SHVMainThreadEventDispatcher
{
public:


	// constructor
	SHVMainThreadEventDispatcherConsole();
	~SHVMainThreadEventDispatcherConsole();


	// from SHVMainThreadEventDispatcher
	virtual void SetupDefaults(SHVModuleList& modules);

	virtual void SignalDispatcher();

	virtual SHVBool InitializeEventLoop();
	virtual void RunEventLoop();
	virtual void StopEventLoop(SHVBool errors);


protected:

	// from SHVEventTarget
	virtual void OnEvent(SHVEvent* event);
	
	void HandleStdin();

private:
	///\cond INTERNAL
	
	enum { StdinBufSize = 256 };
	
#ifdef __SHIVA_WIN32
	SHVModuleList* ModuleList;
	SHVMutexBase Signal;
	SHVEventSubscriberRef selfSubs;
	SHVThread<SHVMainThreadEventDispatcherConsole> StdinThread;
	void StdinFunc();
#else
	SHVMutex Mutex;
	bool QueueSignalled;
	int PipeSignal[2];
#endif
	
	SHVBufferCStream StdinStream;
	size_t StdinPos;
	///\endcond
};

#endif
