#ifndef __SHIVA_MODULES_UNITTEST_H
#define __SHIVA_MODULES_UNITTEST_H

#include "shiva/include/framework/shvmodule.h"
#include "shiva/include/utils/shvvectorref.h"
#include "shiva/include/utils/shvhashtable.h"
#include "forms/shvformmain.h"
#include "logger/shvtestlogger.h"
#include "tests/shvtestbase.h"


//-=========================================================================================================
/// SHVUnitTest class
/**
 */

class SHVUnitTest : public SHVModule
{
public:

	SHVUnitTest(SHVModuleList& modules);

	virtual SHVBool Register();
	virtual void PostRegister();
	virtual void Unregister();

	virtual void OnEvent(SHVEvent* event);

	void RegisterTest(SHVTestBase* test);

	bool IsPerforming();

private:

	enum MenuItems {
		MenuQuit = 10000,
		MenuPerformAll,
		MenuClearLog
	};
	
	struct TestGroup
	{
		const SHVStringC Group;
		SHVVectorRef<SHVTestBase> Tests;
		inline TestGroup(SHVTestBase* test) : Group(test->GetGroup()) { Tests.Add(test); }
	};
	typedef SHVPtrContainer<TestGroup> TestGroupPtr;
	
	struct ActionMenuItem
	{
		int Flag;
		SHVTestBase* Tester;
		TestGroup* Group;
		inline ActionMenuItem(int flag, SHVTestBase* tester) : Flag(flag), Tester(tester), Group(NULL) {}
		inline ActionMenuItem(TestGroup* group) : Flag(SHVTestBase::FlagAll), Tester(NULL), Group(group) {}
	};
	
	void PerformTestFromString(const SHVString8C str);
	void OnMenuEvent(SHVEvent* event);
	void OnTestResult(SHVEvent* event);
	void OnTestResultShow(SHVEvent* event);

	SHVGUIManager* GUIManager;
	SHVFormMainRef FormMain;
	SHVTestLoggerRef TestLogger;

	bool Performing;
	
	SHVEventSubscriberBaseRef TestResultSubs, TestResultShowSubs;
	SHVEventSubscriberBaseRef MenuSubs;
	SHVList<TestGroupPtr, TestGroup*> TestGroups;
	SHVVector<ActionMenuItem> ActionMenuItems;
};

#endif
