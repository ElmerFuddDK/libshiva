#ifndef __SHIVA_MODULES_SUBPROCESSTESTER_H
#define __SHIVA_MODULES_SUBPROCESSTESTER_H

#include "shiva/include/framework/shvmodule.h"
#include "shiva/include/modules/subprocess/shvsubprocessserver.h"
#include "shiva/include/threadutils/shvthread.h"

//-=========================================================================================================
/// SHVSubProcessTester class
/**
 */

class SHVSubProcessTester : public SHVModule
{
public:

	SHVSubProcessTester(SHVModuleList& modules);

	virtual SHVBool Register();
	virtual void Unregister();

	virtual void OnEvent(SHVEvent* event);


private:

	void ThreadProcess();

	SHVSubProcessServer* SubProcessServer;
	SHVSubProcessPtr SubProcess;

	SHVThread<SHVSubProcessTester> Thread;
	SHVSubProcessPtr ThreadSubProcess;
};

#endif
