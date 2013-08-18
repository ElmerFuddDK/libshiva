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

#include "../../../include/threadutils/shvthreadpoolbase.h"


// ========================================================================================================
//  SHVThreadPoolBase class
// ========================================================================================================
/// \class SHVThreadPoolBase shvthreadpoolbase.h "shiva/include/threadutils/shvthreadpoolbase.h"

/*************************************
 * Constructor
 *************************************/
SHVThreadPoolBase::SHVThreadPoolBase(int initialCount, int maxCount, short priority, SHVInt stackSize)
	: MaxCount(maxCount), Priority(priority), StackSize(stackSize)
{
	ThreadsStarting = 0;
	Running = false;
	ThreadStartingFunc = NULL;
	ThreadStartingData = NULL;
	
	if (initialCount >= 0)
		SHVVERIFY(Start(initialCount, maxCount, priority));
}

/*************************************
 * Destructor
 *************************************/
SHVThreadPoolBase::~SHVThreadPoolBase()
{
	Stop();
	SHVASSERT(WaitQueue.GetCount() == 0);
}

/*************************************
 * SetThreadStartingFunction
 *************************************/
void SHVThreadPoolBase::SetThreadStartingFunction(SHVThreadBase::ThreadFunc func, void *data)
{
	Lock.Lock();
	ThreadStartingFunc = func;
	ThreadStartingData = data;
	Lock.Unlock();
}

/*************************************
 * Execute
 *************************************/
/// Executes a function with a thread from the pool
SHVThreadBase::ThreadID SHVThreadPoolBase::Execute(SHVThreadBase::ThreadFunc func, void* data)
{
SHVThreadBase::ThreadID retVal = SHVThreadBase::InvalidThreadID;
PoolThreadData* thread = NULL;
bool err = false;

	Lock.Lock();
	while (!thread && Running && !err)
	{
		// Locate an idle thread and start it
		for (size_t i=0; !thread && i<ThreadPool.CalculateCount(); i++)
		{
			if (ThreadPool[i]->Func == NULL)
			{
				thread = ThreadPool[i];
				thread->Func = func;
				thread->Data = data;
				thread->Lock.Unlock();
			}
		}
		
		// If no idle and there is headroom to start a new
		if (!thread && MaxCount > (int)ThreadPool.CalculateCount())
		{
		PoolThreadFuncData* threadFuncData = new PoolThreadFuncData;
			threadFuncData->Self = this;
			threadFuncData->Data = thread = new PoolThreadData;
			
			ThreadPool.Add(threadFuncData->Data);
			threadFuncData->Data->Func = func;
			threadFuncData->Data->Data = data;
			
			SHVVERIFY(threadFuncData->Data->Thread.Start((SHVThreadBase::ThreadFunc)&SHVThreadPoolBase::PoolThreadFunc,threadFuncData,Priority,NULL,StackSize));
		}
		
		// If there still is no thread, wait for one to be available
		if (!thread)
		{
		bool isPoolThread = false;
		SHVThreadBase::ThreadID curThread = SHVThreadBase::GetCurrentThreadID();
			
			for (size_t j=0; !isPoolThread && j<ThreadPool.CalculateCount(); j++)
			{
				isPoolThread = (ThreadPool[j]->Thread.GetThreadID() == curThread);
			}
			
			if (!isPoolThread)
			{
			PoolThreadWaitData waitObj;
				
				waitObj.Func = func;
				waitObj.Data = data;
				waitObj.PoolThread = NULL;
				waitObj.Lock.Lock();
				WaitQueue.AddTail(&waitObj);
				Lock.Unlock();
				
				waitObj.Lock.Lock();
				waitObj.Lock.Unlock();
				thread = waitObj.PoolThread;
				
				Lock.Lock();
			}
			else
			{
				err = true;
			}
		}
	}
	if (thread)
		retVal = thread->Thread.GetThreadID();
	SHVASSERT(!Running || retVal != SHVThreadBase::InvalidThreadID);
	Lock.Unlock();
	
	return retVal;
}

/*************************************
 * Start
 *************************************/
/// Starts the thread pool
bool SHVThreadPoolBase::Start(int initialCount, int maxCount, short priority, SHVInt stackSize)
{
PoolThreadFuncData* threadFuncData;
bool retVal = false;

	if (maxCount < 0)
		maxCount = initialCount;
	SHVASSERT(initialCount > 0 && maxCount > 0 && initialCount <= maxCount);
	
	Lock.Lock();
	if (!Running && initialCount >= 0)
	{
		MaxCount = maxCount;
		if (!stackSize.IsNull())
			StackSize = stackSize;
		ThreadsStarting = 0;
		ThreadInitLock.Lock();
		Running = true;
		retVal = true;
		
		Priority = priority;
		
		for (int i=0; i<initialCount;i++)
		{
			threadFuncData = new PoolThreadFuncData;
			threadFuncData->Self = this;
			threadFuncData->Data = new PoolThreadData;
			
			ThreadPool.Add(threadFuncData->Data);
			threadFuncData->Data->Func = NULL;
			threadFuncData->Data->Data = NULL;
			ThreadsStarting++;
			SHVVERIFY(threadFuncData->Data->Thread.Start((SHVThreadBase::ThreadFunc)&SHVThreadPoolBase::PoolThreadFunc,threadFuncData,Priority,NULL,StackSize));
		}
		Lock.Unlock();
		
		if (initialCount > 0)
		{
			ThreadInitLock.Lock();
			SHVASSERT(ThreadsStarting == 0);
			ThreadInitLock.Unlock();
		}
	}
	else
	{
		Lock.Unlock();
	}
	
	return retVal;
}

/*************************************
 * Stop
 *************************************/
/// Stops the thread pool
/**
 * Will wait for all the threads to stop before returning
 */
void SHVThreadPoolBase::Stop()
{
	Lock.Lock();
	if (Running)
	{
	int runningThreads;
		Running = false;
		
		while (WaitQueue.GetCount())
			WaitQueue.PopHead()->Lock.Unlock();
		
		do
		{
			runningThreads = 0;
			for (size_t i=0; i<ThreadPool.CalculateCount(); i++)
			{
				if (ThreadPool[i]->Thread.IsRunning())
				{
					runningThreads++;
					ThreadPool[i]->Lock.Unlock();
				}
			}
			Lock.Unlock();
			if (runningThreads)
				SHVThreadBase::Sleep(50);
			Lock.Lock();
		}
		while (runningThreads);
	}
	Lock.Unlock();
}

///\cond INTERNAL
/*************************************
 * PoolTheadFunc
 *************************************/
void SHVThreadPoolBase::PoolThreadFunc(void* val)
{
PoolThreadFuncData* t = (PoolThreadFuncData*)val;
PoolThreadData* threadData = t->Data;
SHVThreadPoolBase* self = t->Self;

	delete t;

	if (!threadData->Func)
		threadData->Lock.Lock();

	self->Lock.Lock();
	if (self->ThreadsStarting)
	{
		if (--self->ThreadsStarting == 0)
		{
			self->ThreadInitLock.Unlock();
		}
	}
	self->Lock.Unlock();

	if (self->ThreadStartingFunc)
		(*(self->ThreadStartingFunc))(self->ThreadStartingData);

	do
	{
		threadData->Lock.Lock();
		
		if (self->Running && threadData->Func)
		{
			(*threadData->Func)(threadData->Data);
			
			self->Lock.Lock();
			if (self->WaitQueue.GetCount())
			{
			PoolThreadWaitData* data = self->WaitQueue.PopHead();
				threadData->Lock.Unlock();
				threadData->Func = data->Func;
				threadData->Data = data->Data;
				data->PoolThread = threadData;
				data->Lock.Unlock();
			}
			else
			{
				threadData->Func = NULL;
				threadData->Data = NULL;
			}
			self->Lock.Unlock();
		}
	}
	while (self->Running);
	
	threadData->Lock.Unlock();
	threadData->Thread.ResetHandle();
}
///\endcond
