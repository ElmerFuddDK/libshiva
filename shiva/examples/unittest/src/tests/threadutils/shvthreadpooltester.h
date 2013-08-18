#ifndef __SHIVA_TESTS_THREADUTILS_THREADPOOL_H
#define __SHIVA_TESTS_THREADUTILS_THREADPOOL_H

#include "shiva/include/unittest/src/shvtestqueued.h"

#include "shiva/include/threadutils/shvthreadpool.h"
#include "shiva/include/framework/shveventthread.h"


class SHVThreadPoolTester : public SHVTestQueued
{
public:

	// test modes
	enum Actions {
		ActionSimpleExecute = 0x01,
	};

	SHVThreadPoolTester(SHVModuleList& modules);
	virtual ~SHVThreadPoolTester();

	virtual const SHVString8C GetGroup() const;
	virtual const SHVString8C GetID() const;
	virtual const SHVStringC GetTitle() const;
	virtual const SHVTestBase::Action* GetActions() const;
	virtual SHVEventQueue* GetQueue();

	// sub tests
private:
	static bool TestSimpleExecute(SHVModuleList& modules, SHVTestBase* self, int flag);

	// implementation
private:
	SHVEventThread EventQueue;

	struct TestObject
	{
		SHVThreadPoolTester* Self;

		SHVThreadPool<TestObject> ThreadPool;
		bool ok;
		volatile int ThreadStartingCounter;
		volatile int Counter;
		SHVMutex Lock;

		TestObject(SHVTestBase* self, int maxThreads = 25);

		void ThreadStarting();
		void ThreadFuncSimple(void*);
	};
};

#endif
