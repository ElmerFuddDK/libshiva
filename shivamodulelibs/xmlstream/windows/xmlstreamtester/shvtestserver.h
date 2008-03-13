#ifndef __SHIVA_FRAMEWORKTESTERS_TESTSERVER_H
#define __SHIVA_FRAMEWORKTESTERS_TESTSERVER_H

#include "../../../../include/framework/shvmodule.h"
#include "../../../../include/shvlogger.h"
#include "../../../../include/utils/shvlist.h"


class SHVTestResult;
//-=========================================================================================================
/// SHVTestServer class - Implements the test framework - use from test modules
/**
 * Implements the test framework - resolve this in test modules.
 */

class SHVTestServer : public SHVModule
{
public:

	enum Events {
		EventStartTest,	///< is emitted when testing begins
		EventTestDone	///< is emitted when all tests are done
	};


	// constructor
	SHVTestServer(SHVModuleList& modules, SHVLogger& logger);


	virtual void PostRegister();


	void InitiateTest();


	// State handling
	bool TestPending(SHVModule* caller);
	void TestDone(SHVTestResult* result);


	void AddLogLine(SHVModuleBase* caller, const SHVStringC& log);


protected:


	enum InternalEvents {
		EventInternalAddLogLine = 0xFF,
		EventInternalTestDone
	};

	virtual void OnEvent(SHVEvent* event);


private:
	///\cond INTERNAL
	void CheckTestDone();

	SHVLogger& Logger;
	SHVList<SHVModule*> PendingTests;

	bool InitiateTestPending;
	bool PerformingTest;
	///\endcond
};



//-=========================================================================================================
/// SHVTestResult class - Contains the result of a test
/**
 * This class is to be used when finishing a test.
 */
class SHVTestResult : public SHVRefObject
{
friend class SHVTestServer;
public:


	// constructor
	SHVTestResult(SHVModule* caller, const SHVStringC& title);


	// log handling
	void AddLog(const SHVStringC& log);
	void AddLog(const SHVTChar* s, ...);


	operator bool();
	SHVBool operator=(SHVBool res);
	SHVBool GetResult();


private:
	///\cond INTERNAL
	SHVModule* Caller;
	SHVString Title;
	SHVList<SHVString,const SHVStringC&> Log;
	SHVBool Result;
	///\endcond
};
typedef SHVRefObjectContainer<SHVTestResult> SHVTestResultRef;

#endif
