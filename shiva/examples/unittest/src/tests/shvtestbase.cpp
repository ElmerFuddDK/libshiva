#include "stdafx.h"
#include "shiva/include/platformspc.h"
#include "shiva/include/threadutils/shvmutexlocker.h"

#include "shvtestbase.h"


//=========================================================================================================
// SHVTestBase class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVTestBase::SHVTestBase()
{
	OK = true;
}

/*************************************
 * Perform
 *************************************/
SHVBool SHVTestBase::Perform(SHVModuleList& modules, int actionFlags, SHVEventSubscriberBase* result)
{
	if (InitializePerform())
	{
		for (const Action* actions = GetActions(); actions && actions->Flag; actions++)
		{
			if ((actionFlags&actions->Flag) && actions->Func)
				PerformAction(modules,actions);
		}
		
		EmitPerformedEvent(modules,result);
		return SHVBool::True;
	}
	return SHVBool::False;
}

/*************************************
 * IsPerforming
 *************************************/
bool SHVTestBase::IsPerforming()
{
	return false;
}

/*************************************
 * DisplayResult
 *************************************/
void SHVTestBase::DisplayResult(SHVTestLogger* logger)
{
SHVMutexLocker lock(LogLock);
	AddLine(_S(""));
	AddLine(_S("Result for %s : %s"), GetTitle().GetSafeBuffer(), logger->Success(OK).GetSafeBuffer());
	for (size_t i=0; i < Log.CalculateCount(); i++)
	{
		switch (Log[i]->Type)
		{
		case LogTypeTitle:
			logger->AddTitle(Log[i]->String);
			break;
		case LogTypeHeader:
			logger->AddHeader(Log[i]->String);
			break;
		case LogTypeLine:
			logger->AddLine(Log[i]->String);
			break;
		}
	}
	Log.Clear();
}

/*************************************
 * InitializePerform
 *************************************/
bool SHVTestBase::InitializePerform()
{
SHVMutexLocker l(LogLock);

	if (!IsPerforming())
	{
		Log.Clear();
		Log.Add(new LogString(LogTypeTitle,SHVStringC::Format(_S("Performing %s"), GetTitle().GetSafeBuffer())));
		OK = true;
		return true;
	}
	return false;
}

/*************************************
 * PerformAction
 *************************************/
void SHVTestBase::PerformAction(SHVModuleList& modules, const Action* action)
{
bool result;
	AddHeader(_S("%s:"), action->Name);
	result = (*action->Func)(modules, this, action->Flag);
	OK = OK && result;
}

/*************************************
 * EmitPerformedEvent
 *************************************/
void SHVTestBase::EmitPerformedEvent(SHVModuleList& modules, SHVEventSubscriberBase* result)
{
	result->EmitNow(modules,new SHVEventString(NULL,"performed",SHVInt(),SHVInt(),this));
}

/*************************************
 * AddHeader
 *************************************/
void SHVTestBase::AddHeader(const SHVStringC str)
{
SHVMutexLocker lock(LogLock);
	Log.Add(new LogString(LogTypeHeader, SHVString(str).ReleaseBuffer()));
}

/*************************************
 * AddHeader
 *************************************/
void SHVTestBase::AddHeader(const SHVTChar* s, ...)
{
SHVString str;
	SHVVA_LIST args;
	SHVVA_START(args, s);
	str.FormatList(s,args);
	AddHeader(str);
	SHVVA_END(args);
}

/*************************************
 * AddLine
 *************************************/
void SHVTestBase::AddLine(const SHVStringC str)
{
SHVMutexLocker lock(LogLock);
	Log.Add(new LogString(LogTypeLine, SHVString(str).ReleaseBuffer()));
}

/*************************************
 * AddLine
 *************************************/
void SHVTestBase::AddLine(const SHVTChar* s, ...)
{
SHVString str;
	SHVVA_LIST args;
	SHVVA_START(args, s);
	str.FormatList(s,args);
	AddLine(str);
	SHVVA_END(args);
}

/*************************************
 * Success
 *************************************/
SHVStringBuffer SHVTestBase::Success(SHVModuleList& modules, bool ok)
{
	if (ok)
		return modules.GetConfig().Find(_S("success.true"), _S("yay"))->ToString();
	return modules.GetConfig().Find(_S("success.false"), _S("nay"))->ToString();
}

// ============================================== documentation ============================================= //

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
