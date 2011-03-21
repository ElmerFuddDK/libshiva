#ifndef __SHIVE_DATAENGINE_TEST_H
#define __SHIVE_DATAENGINE_TEST_H

#include "shvtestmodule.h"
#include "../../../../include/modules/dataengine/shvdataengine.h"
#include "../../../../include/threadutils/shvthread.h"


class SHVDataEngineTest: public SHVTestModule
{
public:
	inline SHVDataEngineTest(SHVModuleList& modules): SHVTestModule(modules, "DataEngineTest"), trace(false) {}
protected:
	virtual void PerformTest(SHVTestResult* result);
	virtual SHVBool Register();
	virtual void PostRegister();
	virtual void OnEvent(SHVEvent* event);
private:
	SHVDataEngine* DataEngine;
	SHVTestResult* Result;
	bool trace;
	SHVMutexBase WaitLock;
	int WaitRefCount;
};

#endif
