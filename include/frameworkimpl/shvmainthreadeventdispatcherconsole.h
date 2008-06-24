#ifndef __SHIVA_FRAMEWORK_SHVMAINTHREADEVENTDISPATCHERCONSOLE_H
#define __SHIVA_FRAMEWORK_SHVMAINTHREADEVENTDISPATCHERCONSOLE_H

#include "../framework/shvmainthreadeventdispatcher.h"
#include "../threadutils/shvmutexbase.h"
#include "../utils/shvbuffercstream.h"

#include "../framework/shveventstdin.h"

//-=========================================================================================================
/// SHVMainThreadEventDispatcherConsole class - Dispatcher for console user IO
/**
 * This class implements a generic dispatcher that can be used for generic console
 * user IO.
 */

class SHVMainThreadEventDispatcherConsole : public SHVMainThreadEventDispatcher
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
	
	SHVMutex Mutex;
	bool QueueSignalled;
	int PipeSignal[2];
	
	SHVBufferCStream StdinStream;
	size_t StdinPos;
	///\endcond
};

#endif
