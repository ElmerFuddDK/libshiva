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
#include "../../../include/platformspc.h"

#include "../../../include/frameworkimpl/shvtimerimpl.h"

#define TIMER_MINIMUM_KICK 600L

//=========================================================================================================
// SHVTimerImpl module - Implementation of the timer module
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVTimerImpl::SHVTimerImpl(SHVModuleList& modules) : SHVTimer(modules), TimerThread(this)
{
}

/*************************************
 * Register
 *************************************/
SHVBool SHVTimerImpl::Register()
{
	TimerThread.StartThread(Modules);

	return SHVModule::Register();
}

/*************************************
 * PostRegister
 *************************************/
void SHVTimerImpl::PostRegister()
{
	if (TimerList.GetCount())
		TimerThread.ResetTimerThread();
}

/*************************************
 * Unregister
 *************************************/
void SHVTimerImpl::Unregister()
{
	TimerThread.StopThread();

	if (TimerThread.LockEvent())
	{
		TimerList.RemoveAll();
		TimerThread.UnlockEvent();
	}

	SHVModule::Unregister();
}

/*************************************
 * CreateTimer
 *************************************/
SHVTimerInstance* SHVTimerImpl::CreateTimer(SHVEventSubscriberBase* subscriber)
{
SHVBool gotLock(TimerThread.LockEvent()); // lock if the event system is running
SHVTimerInstanceImpl* retVal = new SHVTimerInstanceImpl(this,subscriber);

	TimerList.AddTail(retVal);

	if (gotLock)
		TimerThread.UnlockEvent();

	return retVal;
}

/*************************************
 * RemoveTimer
 *************************************/
void SHVTimerImpl::RemoveTimer(SHVTimerInstance* timer)
{
SHVBool gotLock(TimerThread.LockEvent()); // lock if the event system is running
SHVListIterator<SHVTimerInstanceImplRef,SHVTimerInstanceImpl*> itr(TimerList);
SHVListPos pos = NULL;

	while (!pos && itr.MoveNext())
	{
		if ((SHVTimerInstance*)itr.Get() == timer)
			pos = itr.Pos();
	}

	if (pos)
		TimerList.RemoveAt(pos);
	else
		timer->ValidateRefCount();

	if (gotLock)
		TimerThread.UnlockEvent();
}

///\cond INTERNAL
/*************************************
 * RecalculateTimer
 *************************************/
void SHVTimerImpl::RecalculateTimer(SHVTimerInstanceImpl* timer)
{
SHVBool gotLock(TimerThread.LockEvent()); // lock if the event system is running

	// calculate and stuff

	switch (timer->Mode)
	{
	case SHVTimerInstance::ModeAbsolute:
		break;
	case SHVTimerInstance::ModeOnce:
	case SHVTimerInstance::ModeRecurring:
		if (timer->Interval > 0)
		{
		long now = SHVThreadBase::GetTicksInMilliSecs();
			
			timer->TickHit = now + timer->Interval;
			timer->WrapAround = (timer->TickHit < now);

			break;
		}
		// continue
	default:
		timer->Mode = SHVTimerInstance::ModeStopped;
		break;
	}

	if (gotLock)
	{
		TimerThread.UnlockEvent();
		TimerThread.ResetTimerThread();
	}
}
///\endcond


//=========================================================================================================
// SHVTimerInstanceImpl class - interface for a timer
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVTimerInstanceImpl::SHVTimerInstanceImpl(SHVTimerImpl* timer, SHVEventSubscriberBase* subscriber) : Subscriber(subscriber)
{
	Timer = timer;
	Mode = SHVTimerInstance::ModeStopped;
	Interval = 0;
	WrapAround = false;
}

/*************************************
 * Set
 *************************************/
void SHVTimerInstanceImpl::Set(SHVTimerInstance::Modes mode, int interval)
{
	Mode = mode;
	Interval = interval;

	Timer->RecalculateTimer(this);
}

/*************************************
 * SetAbsolute
 *************************************/
void SHVTimerInstanceImpl::SetAbsolute(const SHVTime& time)
{
	AbsoluteTime = time;
	Mode = SHVTimerInstance::ModeAbsolute;
	Timer->RecalculateTimer(this);
}

/*************************************
 * GetMode
 *************************************/
SHVTimerInstance::Modes SHVTimerInstanceImpl::GetMode()
{
	return Mode;
}

/*************************************
 * GetInterval
 *************************************/
int SHVTimerInstanceImpl::GetInterval()
{
	return Interval;
}


///\cond INTERNAL
/*************************************
 * PerformPending
 *************************************/
bool SHVTimerInstanceImpl::PerformPending(long now, bool wrappedAround, long& waitInterval)
{
bool retVal = false;
SHVTime timeNow;
	
	timeNow.SetNow();

	if (wrappedAround)
		WrapAround = false;

	if (!WrapAround)
	{
		switch (Mode)
		{
		case SHVTimerInstance::ModeOnce:
		case SHVTimerInstance::ModeRecurring:
			if (TickHit <= now)
			{
				retVal = true;

				if (Mode == SHVTimerInstance::ModeOnce)
					Mode = SHVTimerInstance::ModeStopped;
				else
				{
					TickHit += Interval;
				}
			}
			break;
		case SHVTimerInstance::ModeAbsolute:
			{
				if (timeNow < AbsoluteTime)
				{
					retVal = false;
					break;
				}
				else
					retVal = true;
			}
			// continue
		default:
			Mode = SHVTimerInstance::ModeStopped;
			break;
		}
	}

	waitInterval = CalculateWaitInterval(now, timeNow);

	return retVal;
}

/*************************************
 * Perform
 *************************************/
void SHVTimerInstanceImpl::Perform()
{
	Subscriber->EmitNow(Timer->GetModuleList(),new SHVEvent(Timer,SHVTimer::EventTimeout,SHVInt(),this));
}

/*************************************
 * CalculateWaitInterval
 *************************************/
long SHVTimerInstanceImpl::CalculateWaitInterval(long now, SHVTime& timeNow)
{
long retVal;

	switch (Mode)
	{
	case SHVTimerInstance::ModeOnce:
	case SHVTimerInstance::ModeRecurring:
		retVal =  TickHit-now;
		if (retVal < 0)
			retVal = 0;
		break;
	case SHVTimerInstance::ModeAbsolute:
		retVal = SHVTime::GapInSeconds(timeNow, AbsoluteTime);
		if (retVal > TIMER_MINIMUM_KICK)
			retVal = TIMER_MINIMUM_KICK * 1000L;
		else
			retVal *= 1000L;
		break;
	default:
		retVal = SHVMutexBase::Infinite;
	}

	return retVal;
}
///\endcond


///\cond INTERNAL
//=========================================================================================================
// SHVTimerThread class - The internal timer thread
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVTimerThread::SHVTimerThread(SHVTimerImpl* timer) : Timer(timer)
{
	LastTick = SHVThreadBase::GetTicksInMilliSecs();
}

/*************************************
 * PreEventDispatch
 *************************************/
void SHVTimerThread::PreEventDispatch()
{

	if (EventActiveInQueue(Timer->GetModuleList()))
	{
	SHVListIterator<SHVTimerInstanceImplRef,SHVTimerInstanceImpl*> itr(Timer->TimerList);
	SHVList<SHVTimerInstanceImplRef,SHVTimerInstanceImpl*> pendingPerforms;
	SHVListIterator<SHVTimerInstanceImplRef,SHVTimerInstanceImpl*> pendingItr(pendingPerforms);
	long now = SHVThreadBase::GetTicksInMilliSecs();
	bool wrappedAround = (now < LastTick);
	long timeoutInterval = TIMER_MINIMUM_KICK * 1000L; // 10 minutes maximum
	long tmpInterval;

		if (LockEvent())
		{
			while (itr.MoveNext())
			{
				if (itr.Get()->PerformPending(now,wrappedAround,tmpInterval))
					pendingPerforms.AddTail(itr.Get());

				if (tmpInterval != SHVMutexBase::Infinite && tmpInterval < timeoutInterval)
					timeoutInterval = tmpInterval;
			}

			UnlockEvent();
		}

		while (pendingItr.MoveNext())
		{
			pendingItr.Get()->Perform();
		}
		pendingPerforms.RemoveAll();

		EventDeactivatedInQueue(Timer->GetModuleList());

		LastTick = now;

		SetSignalTimeout(timeoutInterval);
	}
}

/*************************************
 * ResetTimerThread
 *************************************/
void SHVTimerThread::ResetTimerThread()
{
	if (LockEvent())
	{
	long timeoutInterval = TIMER_MINIMUM_KICK * 1000L; // 10 minutes maximum
	long tmpInterval;
	long now = SHVThreadBase::GetTicksInMilliSecs();
	SHVTime timeNow;
	SHVListIterator<SHVTimerInstanceImplRef,SHVTimerInstanceImpl*> itr(Timer->TimerList);
		timeNow.SetNow();

		while (itr.MoveNext())
		{
			tmpInterval = itr.Get()->CalculateWaitInterval(now, timeNow);

			if (tmpInterval != SHVMutexBase::Infinite && tmpInterval < timeoutInterval)
				timeoutInterval = tmpInterval;
		}

		UnlockEvent();
		SetSignalTimeout(timeoutInterval);
		SignalDispatcher();
	}
}
///\endcond
