#include "stdafx.h"
#include "../../platformspc.h"
#include "../../threadutils/shvmutexlocker.h"

#include "shvtest.h"


//=========================================================================================================
// SHVTest class
//=========================================================================================================
/// \class SHVTest shvtest.h "shiva/include/unittest/src/shvtest.h"

/*************************************
 * Constructor
 *************************************/
SHVTest::SHVTest()
{
	OK = true;
}

/*************************************
 * Perform
 *************************************/
SHVBool SHVTest::Perform(SHVModuleList& modules, int actionFlags, SHVEventSubscriberBase* result)
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
bool SHVTest::IsPerforming()
{
	return false;
}

/*************************************
 * DisplayResult
 *************************************/
void SHVTest::DisplayResult(SHVTestLogger* logger)
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
bool SHVTest::InitializePerform()
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
void SHVTest::PerformAction(SHVModuleList& modules, const Action* action)
{
bool result;
	AddHeader(_S("%s:"), action->Name);
	result = (*action->Func)(modules, this, action->Flag);
	OK = OK && result;
}

/*************************************
 * EmitPerformedEvent
 *************************************/
void SHVTest::EmitPerformedEvent(SHVModuleList& modules, SHVEventSubscriberBase* result)
{
	result->EmitNow(modules,new SHVEventString(NULL,"performed",SHVInt(),SHVInt(),this));
}

/*************************************
 * AddHeader
 *************************************/
void SHVTest::AddHeader(const SHVStringC str)
{
SHVMutexLocker lock(LogLock);
	Log.Add(new LogString(LogTypeHeader, SHVString(str).ReleaseBuffer()));
}

/*************************************
 * AddHeader
 *************************************/
void SHVTest::AddHeader(const SHVTChar* s, ...)
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
void SHVTest::AddLine(const SHVStringC str)
{
SHVMutexLocker lock(LogLock);
	Log.Add(new LogString(LogTypeLine, SHVString(str).ReleaseBuffer()));
}

/*************************************
 * AddLine
 *************************************/
void SHVTest::AddLine(const SHVTChar* s, ...)
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
SHVStringBuffer SHVTest::Success(SHVModuleList& modules, bool ok)
{
	if (ok)
		return modules.GetConfig().Find(_S("success.true"), _S("yay"))->ToString();
	return modules.GetConfig().Find(_S("success.false"), _S("nay"))->ToString();
}
