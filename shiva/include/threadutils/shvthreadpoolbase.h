#ifndef __SHIVA_THREADUTILS_THREADPOOLBASE_H
#define __SHIVA_THREADUTILS_THREADPOOLBASE_H

#include "shvthreadbase.h"
#include "shvmutexbase.h"
#include "../utils/shvvector.h"
#include "../utils/shvlist.h"


// ========================================================================================================
///  SHVThreadPoolBase class
/**
 * This class implements a thread pool from which you can invoke thread
 * functions.
 */

class SHVAPI SHVThreadPoolBase
{
public:

	SHVThreadPoolBase(int initialCount = -1, int maxCount = -1, short priority = SHVThreadBase::PrioNormal, SHVInt stackSize = SHVInt());
	~SHVThreadPoolBase();

	void SetThreadStartingFunction(SHVThreadBase::ThreadFunc func, void* data);

	bool Start(int initialCount = 5, int maxCount = 25, short priority = SHVThreadBase::PrioNormal, SHVInt stackSize = SHVInt());
	void Stop();

	SHVThreadBase::ThreadID Execute(SHVThreadBase::ThreadFunc func, void* data);


protected:
	///\cond INTERNAL
	bool Running;
	int MaxCount;
	short Priority;
	SHVInt StackSize;
	struct PoolThreadData{ SHVThreadBase Thread; SHVMutexBase Lock; SHVThreadBase::ThreadFunc Func; void* Data; };
	struct PoolThreadWaitData{ SHVMutexBase Lock; SHVThreadBase::ThreadFunc Func; void* Data; PoolThreadData* PoolThread; };
	struct PoolThreadFuncData{ PoolThreadData* Data; SHVThreadPoolBase* Self; };
	SHVThreadBase::ThreadFunc ThreadStartingFunc;
	void* ThreadStartingData;
	SHVMutexBase Lock;
	SHVMutexBase ThreadInitLock;
	int ThreadsStarting;
	SHVVector<PoolThreadData,5> ThreadPool;
	SHVList<PoolThreadWaitData*> WaitQueue;
	static void PoolThreadFunc(void* val);
	///\endcond
};

#endif
