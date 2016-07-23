#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvthreadpooltester.h"

/*************************************
 * Constructor
 *************************************/
SHVThreadPoolTester::SHVThreadPoolTester(SHVModuleList& modules) : SHVTestQueued()
{
	EventQueue.StartThread(modules);
}

/*************************************
 * Destructor
 *************************************/
SHVThreadPoolTester::~SHVThreadPoolTester()
{
	EventQueue.StopThread();
}


/*************************************
 * GetGroup
 *************************************/
const SHVString8C SHVThreadPoolTester::GetGroup() const
{
	return "threadutils";
}

/*************************************
 * GetID
 *************************************/
const SHVString8C SHVThreadPoolTester::GetID() const
{
	return "threadpool";
}

/*************************************
 * GetTitle
 *************************************/
const SHVStringC SHVThreadPoolTester::GetTitle() const
{
	return _S("SHVThreadPool test");
}

/*************************************
 * GetQueue
 *************************************/
SHVEventQueue* SHVThreadPoolTester::GetQueue()
{
	return &EventQueue;
}

/*************************************
 * GetActions
 *************************************/
const SHVTestBase::Action* SHVThreadPoolTester::GetActions() const
{
static const SHVTestBase::Action actions[] = {
	{ ActionSimpleExecute, "simpleexecute", _S("simpleexecute"), _S("Tests exhaustive execution of functions in the thread pool"), &SHVThreadPoolTester::TestSimpleExecute },
	{ 0, NULL, NULL, NULL, NULL } }; // Termination

	return actions;
}

/*************************************
 * sub tests
 *************************************/
bool SHVThreadPoolTester::TestSimpleExecute(SHVModuleList &modules, SHVTestBase *self, int flag)
{
bool ok = true;
TestObject tester(self,25);

	SHVUNUSED_PARAM(flag);

	// We run 100 functions in the pool
	tester.Counter = 100;
	
	// Starts 5 threads initially, up to 25
	ok = ok && tester.ThreadPool.Start(5,tester.ThreadStartingCounter);

	// Execute the 100 functions that should eventually result in Counter being 0
	for (int i=tester.Counter; i && ok; i--)
	{
		ok = (ok && ( tester.ThreadPool.Execute(&tester,&TestObject::ThreadFuncSimple,NULL) != SHVThreadBase::InvalidThreadID ));
	}
	
	// Stop the threads, waiting for the last 25 functions to execute
	tester.ThreadPool.Stop();

	// Test that all threads got to start and that the counter is 0
	ok = (ok && (tester.ThreadStartingCounter == 0));
	ok = (ok && (tester.Counter == 0));
	ok = (ok && tester.ok);

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	return ok;
}


/*************************************
 * Tester constructor
 *************************************/
SHVThreadPoolTester::TestObject::TestObject(SHVTestBase* self, int maxThreads) : Self((SHVThreadPoolTester*)self)
{
	ok = true;
	Counter = 0;
	ThreadStartingCounter = maxThreads;
	ThreadPool.SetThreadStartingFunction(this,&TestObject::ThreadStarting);
}

/*************************************
 * Tester implementation
 *************************************/
void SHVThreadPoolTester::TestObject::ThreadStarting()
{
	SHVThreadBase::Sleep(25);
	SHVRefObject::LockedDecrement(&ThreadStartingCounter);
}
void SHVThreadPoolTester::TestObject::ThreadFuncSimple(void*)
{
	SHVThreadBase::Sleep(25);
	SHVRefObject::LockedDecrement(&Counter);
}
