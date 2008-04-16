#ifndef __SHIVA_FRAMEWORK_SHVMAINTHREADEVENTDISPATCHERGENERIC_H
#define __SHIVA_FRAMEWORK_SHVMAINTHREADEVENTDISPATCHERGENERIC_H

#include "../framework/shvmainthreadeventdispatcher.h"
#include "../threadutils/shvmutexbase.h"

//-=========================================================================================================
/// SHVMainThreadEventDispatcherGeneric class - Generic dispatcher that uses a mutex
/**
 * This class implements a generic dispatcher that uses a mutex. This can be used for
 * console applications, services/daemons or subsystems.
 */

class SHVAPI SHVMainThreadEventDispatcherGeneric : public SHVMainThreadEventDispatcher
{
public:


	// constructor
	SHVMainThreadEventDispatcherGeneric();
	~SHVMainThreadEventDispatcherGeneric();


	// from SHVMainThreadEventDispatcher
	virtual void SetupDefaults(SHVModuleList& modules);

	virtual void SignalDispatcher();

	virtual SHVBool InitializeEventLoop();
	virtual void RunEventLoop();
	virtual void StopEventLoop(SHVBool errors);


protected:

	// from SHVEventTarget
	virtual void OnEvent(SHVEvent* event);

private:
	///\cond INTERNAL
	SHVMutexBase Signal;
	///\endcond
};

#endif
