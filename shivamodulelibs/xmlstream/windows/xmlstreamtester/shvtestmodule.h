#ifndef __SHIVA_FRAMEWORKTESTERS_TESTMODULE_H
#define __SHIVA_FRAMEWORKTESTERS_TESTMODULE_H

#include "shvtestserver.h"


//-=========================================================================================================
/// SHVTestModule class - abstract for implementing test modules
/**
 * Inherit from this class if you want to make a test module.
 */

class SHVTestModule : public SHVModule
{
public:

	enum Events {
		EventInternalPerformTest = 0xFF
	};


	// constructor
	SHVTestModule(SHVModuleList& modules, const SHVChar* name);


	virtual SHVBool Register();


protected:

	SHVTestServer* TestServer;

	virtual void PerformTest(SHVTestResult* result) = 0;

private:
	///\cond INTERNAL
	void OnInitiateTest(SHVEvent* event);
	void OnPerformTest(SHVEvent* event);
	///\endcond
};

#endif
