#ifndef __SHIVA_THREADUTILS_THREAD_H
#define __SHIVA_THREADUTILS_THREAD_H

#include "shvthreadbase.h"

// ========================================================================================================
///  SHVThread template class
/**
 * This is the class to use for basic worker threads. It takes a C++ class function pointer and a reference
 * to the object to start the function pointer from on the Start method\n
 *\note It is not possible to restart the thread from itself, for this you must use SHVThreadBase.
 *\see Start
 */

template<class T>
class SHVThread : SHVThreadBase
{
public:

	// defines
	typedef void (T::*ThreadFunc)();

	SHVThread();

	bool Start(T* owner, ThreadFunc func, short priority = SHVThreadBase::PrioNormal, const SHVStringC& name=NULL);

	inline bool Kill();

	inline bool IsRunning();
	inline SHVThreadBase::ThreadID GetThreadID() const;

private:
	///\cond INTERNAL
	struct StartThreadData{ SHVThreadBase* thread; ThreadFunc func; T* self; };
	static void StartThreadFunc(void* val);
	///\endcond
};




// ============================================= implementation ============================================= //
/// \class SHVThread shvthread.h "shiva/include/threadutils/shvthread.h"

/*************************************
 * Constructor
 *************************************/
template<class T>
SHVThread<T>::SHVThread()
{}

/*************************************
 * Start
 *************************************/
/// Starts the thread
/**
 \param owner Pointer to an object from wich to start the thread
 \param func Pointer to the function to be called on owner
 \param priority One of the Prio* enum priorities (optional)
 \param name The name of the thread, for debugging (optional)
 \return Success
 *
 * If the thread is not already running, it will start the thread as owner->func().
 * It will be started with the thread priority given by the priority parameter.
 * It will return true if the thread gets started, false if the start failed, or
 * it is already running.
 * NOTE: name is not supported on all platforms. It is meant as an aid for debugging.
 * Heres a usage example:
\code
class MyClass
{
	SHVThread<MyClass> MyThread;
	int itr;

	void MyThreadFunc()
	{
		// Perform work in the thread.
		printf("Hello world from thread # %d\n", itr);

		// When execution stops in the thread func the thread stops running.
	}

public:

	MyClass()
	{
		MyThread.Start(this,&MyClass::MyThreadFunc);
	}
};
\endcode
 */
template<class T>
bool SHVThread<T>::Start(T* owner, ThreadFunc func, short priority, const SHVStringC& name)
{
StartThreadData* data = (StartThreadData*)::malloc(sizeof(StartThreadData));
bool retVal;

	data->thread = (SHVThreadBase*)this;
	data->func = func;
	data->self = owner;

	retVal = SHVThreadBase::Start(&StartThreadFunc,data,priority,name);

	if (!retVal)
		::free(data);

	return retVal;
}

/*************************************
 * Kill
 *************************************/
template<class T>
bool SHVThread<T>::Kill()
{ return SHVThreadBase::Kill(); }

/*************************************
 * IsRunning
 *************************************/
/// Checks if the thread is started
/**
 \return true if the thread is running
 */
template<class T>
bool SHVThread<T>::IsRunning()
{ return SHVThreadBase::IsRunning(); }

/*************************************
 * GetThreadID
 *************************************/
template<class T>
SHVThreadBase::ThreadID SHVThread<T>::GetThreadID() const
{ return SHVThreadBase::GetThreadID(); }

/*************************************
 * StartThreadFunc
 *************************************/
///\cond INTERNAL
template<class T>
void SHVThread<T>::StartThreadFunc(void* val)
{
// move the data onto the stack
SHVThreadBase* thread = ((StartThreadData*)val)->thread;
ThreadFunc func = ((StartThreadData*)val)->func;
T* self = ((StartThreadData*)val)->self;

	::free(val);

	(self->*func)();

	thread->ResetHandle();
}
///\endcond
#endif
