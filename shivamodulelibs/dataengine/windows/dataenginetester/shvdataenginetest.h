#ifndef __SHIVE_DATAENGINE_TEST_H
#define __SHIVE_DATAENGINE_TEST_H

#include "shvtestmodule.h"
#include "../../../../include/modules/dataengine/shvdataengine.h"
#include "../../../../include/threadutils/shvthread.h"


class SHVDataEngineTest: public SHVTestModule
{
public:
	inline SHVDataEngineTest(SHVModuleList& modules): SHVTestModule(modules, "DataEngineTest"), trace(false) {}
	void ThreadDone();
protected:
	virtual void PerformTest(SHVTestResult* result);
	virtual SHVBool Register();
	virtual void PostRegister();
	virtual void OnEvent(SHVEvent* event);
private:
	SHVBool TestInsert(SHVTestResult* result);
	SHVBool TestUpdate(SHVTestResult* result);
	SHVBool TestQueryTable(SHVTestResult* result);
	SHVBool TestQuery(SHVTestResult* result);
	SHVBool TestSpeed(SHVTestResult* result);
	SHVBool TestSpeed2(SHVTestResult* result);
	SHVBool TestConcurrency(SHVTestResult* result);
	SHVBool AddPerson(SHVDataRowList* list, int pk_person, const SHVStringC& firstName, const SHVStringC& middleName, const SHVStringC& lastName);
	SHVDataEngine* DataEngine;
	SHVTestResult* Result;
	bool trace;
	SHVMutexBase WaitLock;
	int WaitRefCount;
};

#endif