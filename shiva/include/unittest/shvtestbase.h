#ifndef __SHIVA_UNITTEST_TESTBASE_H
#define __SHIVA_UNITTEST_TESTBASE_H

#include "../framework/shvmodulelist.h"
#include "../utils/shvstring.h"
#include "../utils/shvrefobject.h"
#include "shvtestlogger.h"

#define __MODULESYMBOL_UNITTESTS "UnitTests"


//-=========================================================================================================
/// SHVTestBase class
/**
 * Interface for implementing a unit test.\n
 * Inherit from either src/shvtest or src/shvtestqueued.
 */

class SHVTestBase : public SHVRefObject
{
public:

	typedef bool (*PerformFunc)(SHVModuleList& modules, SHVTestBase* self, int flag);
	
	enum { FlagAll = 0x7FFFFFFF };

	struct Action
	{
		int Flag;
		const SHVChar* ActionID;
		const SHVTChar* Name;
		const SHVTChar* Description;
		PerformFunc Func;
	};


	virtual const SHVString8C GetGroup() const = 0;
	virtual const SHVString8C GetID() const = 0;
	virtual const SHVStringC GetTitle() const = 0;
	virtual const Action* GetActions() const = 0;


	virtual SHVBool Perform(SHVModuleList& modules, int actionFlags, SHVEventSubscriberBase* result) = 0;

	virtual bool IsPerforming() = 0;
	virtual void DisplayResult(SHVTestLogger* logger) = 0;


	// For use within the tests to add log to the end result
	virtual void AddHeader(const SHVStringC str) = 0;
	virtual void AddHeader(const SHVTChar* s, ...) = 0;
	virtual void AddLine(const SHVStringC str) = 0;
	virtual void AddLine(const SHVTChar* s, ...) = 0;
	virtual SHVStringBuffer Success(SHVModuleList& modules, bool ok) = 0;


	inline static SHVTestBase* FromTestEvent(SHVEvent* event);


protected:
	inline SHVTestBase() {}
};


// ============================================= implementation ============================================= //

SHVTestBase* SHVTestBase::FromTestEvent(SHVEvent* event)
{
	return (SHVTestBase*)event->GetObject();
}

// ============================================== documentation ============================================= //

/// \class SHVTestBase shvtestbase.h "shiva/include/unittest/shvtestbase.h"

// GetGroup
/** \fn const SHVString8C SHVTestBase::GetGroup() const
 \brief Must return the group for the test, for command line test, eg "threadutils"
 */

// GetID
/** \fn const SHVString8C SHVTestBase::GetID() const
 \brief Must return the ID for the test, for command line test, eg "thread"
 */

// GetTitle
/** \fn const SHVStringC SHVTestBase::GetTitle() const
 \brief Must return the title of this test, eg "Thread test"
 */

// GetActions
/** \fn const SHVTestBase::Action* SHVTestBase::GetActions() const
 \brief Return a list of actions that define this test
 *
 * Must return a static list of actions that defines the actions this test can perform, eg:
\code
const SHVTestBase::Action* SHVMyTest::GetActions() const
{
const SHVTestBase::Action actions[] = {
	{ 1, "test1", _S("My first test"), _S("This is my first test function"), &SHVMyTest::MyTest1 },
	{ 2, "test2", _S("My second test"), _S("Yet another test function"), &SHVMyTest::MyTest2 },
	{ 4, "test3", _S("My third test"), _S("This is getting old now"), &SHVMyTest::MyTest3 },
	{ 0, NULL, NULL, NULL, NULL } }; // Termination
	return actions;
}
\endcode
 */

// Perform
/** \fn SHVBool SHVTestBase::Perform(SHVModuleList& modules, int actionFlags, SHVEventSubscriberBase* result)
 \brief Attempt to start tests
 \param modules Module list for the application
 \param actionFlags Test actions to perform
 \param result Event subscriber to emit the result to
 *
 * Must perform actions listed from GetActions. Inherit from either SHVTest
 * or SHVTestQueued since they implement this functionality.
 */

// IsPerforming
/** \fn bool SHVTestBase::IsPerforming()
 \brief Returns true if the test is busy performing
 */

/** \struct SHVTestBase::Action shvtestbase.h "shiva/include/unittest/shvtestbase.h"
 \brief Contains info about a specific test to perform
 *
 \see SHVTestBase::GetActions
 */

#endif
