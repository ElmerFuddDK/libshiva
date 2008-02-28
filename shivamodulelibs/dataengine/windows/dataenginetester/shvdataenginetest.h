#ifndef __SHIVE_DATAENGINE_TEST_H
#define __SHIVE_DATAENGINE_TEST_H

#include "shvtestmodule.h"
#include "../../include/shvdataengine.h"


class SHVDataEngineTest: public SHVTestModule
{
public:
	inline SHVDataEngineTest(SHVModuleList& modules): SHVTestModule(modules, "DataEngineTest") {}
protected:
	virtual void PerformTest(SHVTestResult* result);
	virtual SHVBool Register();
	virtual void PostRegister();
private:
	SHVBool TestInsert(SHVTestResult* result);
	SHVBool TestQueryTable(SHVTestResult* result);
	SHVBool TestQuery(SHVTestResult* result);
	SHVBool TestSpeed(SHVTestResult* result);
	SHVDataEngine* DataEngine;
};

#endif