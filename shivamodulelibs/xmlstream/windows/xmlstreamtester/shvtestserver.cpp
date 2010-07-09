/*
 *   Copyright (C) 2008 by Lars Eriksen
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of the
 *   License, or (at your option) any later version with the following
 *   exeptions:
 *
 *   1) Static linking to the library does not constitute derivative work
 *      and does not require the author to provide source code for the
 *      application.
 *      Compiling applications with the source code directly linked in is
 *      Considered static linking as well.
 *
 *   2) You do not have to provide a copy of the license with programs
 *      that are linked against this code.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "stdafx.h"
#include "../../../../include/platformspc.h"

#include "shvtestserver.h"

//=========================================================================================================
// SHVTestServer class - Implements the test framework - use from test modules
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVTestServer::SHVTestServer(SHVModuleList& modules, SHVLogger& logger) : SHVModule(modules,"TestServer"), Logger(logger)
{
SHVEventSubscriberRef directQueued = new SHVEventSubscriber(this,&Modules,true);
	InitiateTestPending = false;
	PerformingTest = false;

	EventSubscribe(EventInternalAddLogLine,directQueued);
	EventSubscribe(EventInternalTestDone,directQueued);
}

/*************************************
 * PostRegister
 *************************************/
void SHVTestServer::PostRegister()
{
	if (InitiateTestPending)
		InitiateTest();
}

/*************************************
 * InitiateTest
 *************************************/
void SHVTestServer::InitiateTest()
{
	if (!Modules.IsRegistered())
		InitiateTestPending = true;
	else
	{
		InitiateTestPending = false;
		PerformingTest = true;
		EmitEvent(new SHVEvent(this,EventStartTest));
		PerformingTest = false;

		CheckTestDone();
	}
}

/*************************************
 * TestPending
 *************************************/
bool SHVTestServer::TestPending(SHVModule* caller)
{
	if (PerformingTest)
	{
		if (!PendingTests.Find(caller))
			PendingTests.AddTail(caller);

		return true;
	}

	return false;
}

/*************************************
 * TestDone
 *************************************/
void SHVTestServer::TestDone(SHVTestResult* result)
{
	EmitEvent(new SHVEvent(this,EventInternalTestDone,0,result));
}

/*************************************
 * AddLogLine
 *************************************/
void SHVTestServer::AddLogLine(SHVModuleBase* caller, const SHVStringC& log)
{
SHVRefObjectTemplate<SHVString>* obj = new SHVRefObjectTemplate<SHVString>;
	obj->Object().Format(_S("LOG (%s - 0x%X): %s"), caller->GetName().GetSafeBuffer(), SHVThreadBase::GetCurrentThreadID(), log.GetSafeBuffer());
	EmitEvent(new SHVEvent(this,EventInternalAddLogLine,0,obj));
}

/*************************************
 * OnEvent
 *************************************/
void SHVTestServer::OnEvent(SHVEvent* event)
{
	if (event->GetCaller() == this)
	{
		if (SHVEvent::Equals(event,EventInternalAddLogLine))
		{
		SHVRefObjectTemplate<SHVString>* obj = (SHVRefObjectTemplate<SHVString>*)event->GetObject();
			Logger.AddLine(obj->Object());
		}
		else if (SHVEvent::Equals(event,EventInternalTestDone))
		{
		SHVTestResult* result = (SHVTestResult*)event->GetObject();
		
			if (result)
			{
			SHVListPos pos = PendingTests.Find(result->Caller);
				
				if (!result || result->Title.IsNull())
				{
					Logger.AddHeader(_S("Log for module %s"), result->Caller->GetName().ToStrT().GetBufferConst());
				}
		
				if (result)
				{
				SHVListIterator<SHVString,const SHVStringC&> itr(result->Log);
		
					if (!result->Title.IsNull())
						Logger.AddHeader(_S("Log for %s"), result->Title.GetSafeBuffer());
		
					while (itr.MoveNext())
					{
						Logger.AddLine(itr.Get());
					}
					if (result->GetResult())
						Logger.AddLine(_S("Test result: %s"), Logger.Success(result->GetResult()));
					else
						Logger.AddLine(_S("Test result: %s with code %d"), Logger.Success(result->GetResult()), result->GetResult().GetError());

					Logger.AddLine(_S(""));
				}
				else
				{
					Logger.AddLine(_S("No results"));
				}

				if (pos)
				{
					PendingTests.RemoveAt(pos);
					CheckTestDone();
				}
			}
		
		}
	}
}

///\cond INTERNAL
/*************************************
 * CheckTestDone
 *************************************/
void SHVTestServer::CheckTestDone()
{
	if (PendingTests.GetCount() == 0)
		EmitEvent(new SHVEvent(this,EventTestDone));
}
///\endcond



//=========================================================================================================
// SHVTestResult class - Contains the result of a test
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVTestResult::SHVTestResult(SHVModule* caller, const SHVStringC& title) : Caller(caller), Title(title), Result(true)
{}

/*************************************
 * AddLog
 *************************************/
void SHVTestResult::AddLog(const SHVStringC& log)
{
	Log.AddTail(log);
}
void SHVTestResult::AddLog(const SHVTChar* s, ...)
{
SHVString str;
SHVVA_LIST args;
	SHVVA_START(args, s);
	str.FormatList(s,args);
	AddLog(str);
	SHVVA_END(args);
}

/*************************************
 * operator bool
 *************************************/
SHVTestResult::operator bool()
{
	return Result;
}

/*************************************
 * operator == bool
 *************************************/
SHVBool SHVTestResult::operator=(SHVBool res)
{
	if (Result)
		Result = res;
	return res;
}

/*************************************
 * GetResult
 *************************************/
SHVBool SHVTestResult::GetResult()
{
	return Result;
}
