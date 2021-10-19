#ifndef __SHIVA_THREADUTILS_THREADPOOL_H
#define __SHIVA_THREADUTILS_THREADPOOL_H

#include "shvthreadpoolbase.h"

// ========================================================================================================
///  SHVThreadPool template class
/**
 * This is the class to use for thread pools.
 */

template<class T, class O = void*>
class SHVThreadPool : SHVThreadPoolBase
{
public:

	// defines
	typedef void (T::*ThreadStartFunc)();
	typedef void (T::*ThreadFunc)(O);


	// Constructor
	SHVThreadPool(int initialCount = -1, int maxCount = -1, short priority = SHVThreadBase::PrioNormal, SHVInt stackSize = SHVInt());


	void SetThreadStartingFunction(T* owner, ThreadStartFunc func);

	bool Start(int initialCount = 5, int maxCount = 25, short priority = SHVThreadBase::PrioNormal, SHVInt stackSize = SHVInt());
	void Stop();

	void WaitIdle();

	SHVThreadBase::ThreadID Execute(T* owner, ThreadFunc func, O o);


private:
	///\cond INTERNAL
	struct ExecuteData{ ThreadFunc func; T* self; O obj; };
	static void ExecThreadFunc(void* val);
	static void StartThreadFunc(void* val);
	
	T* ThreadStartingOwner;
	ThreadStartFunc ThreadStartingFunc;
	///\endcond
};




// ============================================= implementation ============================================= //
/// \class SHVThreadPool shvthreadpool.h "shiva/include/threadutils/shvthreadpool.h"

/*************************************
 * Constructor
 *************************************/
template<class T,class O>
SHVThreadPool<T,O>::SHVThreadPool(int initialCount, int maxCount, short priority, SHVInt stackSize) : SHVThreadPoolBase(initialCount,maxCount,priority,stackSize)
{
	ThreadStartingOwner = NULL;
	ThreadStartingFunc = NULL;
}

/*************************************
 * SetThreadStartingFunction
 *************************************/
template<class T,class O>
void SHVThreadPool<T,O>::SetThreadStartingFunction(T* owner, ThreadStartFunc func)
{
	Lock.Lock();
	ThreadStartingOwner = owner;
	ThreadStartingFunc = func;
	if (owner && func)
	{
		SHVThreadPoolBase::ThreadStartingFunc = &StartThreadFunc;
		SHVThreadPoolBase::ThreadStartingData = this;
	}
	else
	{
		SHVThreadPoolBase::ThreadStartingFunc = NULL;
		SHVThreadPoolBase::ThreadStartingData = NULL;
	}
	Lock.Unlock();
}

/*************************************
 * Start
 *************************************/
template<class T,class O>
bool SHVThreadPool<T,O>::Start(int initialCount, int maxCount, short priority, SHVInt stackSize)
{
	return SHVThreadPoolBase::Start(initialCount,maxCount,priority,stackSize);
}

/*************************************
 * Stop
 *************************************/
template<class T,class O>
void SHVThreadPool<T,O>::Stop()
{
	SHVThreadPoolBase::Stop();
}

/*************************************
 * WaitIdle
 *************************************/
template<class T,class O>
void SHVThreadPool<T,O>::WaitIdle()
{
	SHVThreadPoolBase::WaitIdle();
}

/*************************************
 * Execute
 *************************************/
template<class T,class O>
SHVThreadBase::ThreadID SHVThreadPool<T,O>::Execute(T* owner, ThreadFunc func, O o)
{
ExecuteData* data = (ExecuteData*)::malloc(sizeof(ExecuteData));
SHVThreadBase::ThreadID retVal;
	data->func = func;
	data->self = owner;
	data->obj = o;
	
	retVal = SHVThreadPoolBase::Execute(&ExecThreadFunc,data);
	
	if (retVal == SHVThreadBase::InvalidThreadID)
		::free(data);
	
	return retVal;
}

///\cond INTERNAL
/*************************************
 * ExecThreadFunc
 *************************************/
template<class T, class O>
void SHVThreadPool<T,O>::ExecThreadFunc(void* val)
{
// move the data onto the stack
ThreadFunc func = ((ExecuteData*)val)->func;
T* self = ((ExecuteData*)val)->self;
O obj = ((ExecuteData*)val)->obj;

	::free(val);

	(self->*func)(obj);
}

/*************************************
 * StartThreadFunc
 *************************************/
template<class T, class O>
void SHVThreadPool<T,O>::StartThreadFunc(void* val)
{
SHVThreadPool<T,O>* self = (SHVThreadPool<T,O>*)val;
	(self->ThreadStartingOwner->*(self->ThreadStartingFunc))();
}
///\endcond
#endif
