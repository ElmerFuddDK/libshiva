#ifndef __SHIVE_DATAENGINE_TEST_H
#define __SHIVE_DATAENGINE_TEST_H

#include "shvtestmodule.h"
#include "../../include/shvdataengine.h"


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
	SHVBool TestInsert(SHVTestResult* result);
	SHVBool TestUpdate(SHVTestResult* result);
	SHVBool TestQueryTable(SHVTestResult* result);
	SHVBool TestQuery(SHVTestResult* result);
	SHVBool TestSpeed(SHVTestResult* result);
	SHVBool TestSpeed2(SHVTestResult* result);
	SHVDataEngine* DataEngine;
	SHVTestResult* Result;
	bool trace;
};

#endif