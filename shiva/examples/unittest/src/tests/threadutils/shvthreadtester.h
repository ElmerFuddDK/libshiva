#ifndef __SHIVA_TESTS_THREADUTILS_THREAD_H
#define __SHIVA_TESTS_THREADUTILS_THREAD_H

#include "shiva/include/unittest/src/shvtestqueued.h"

#include "shiva/include/threadutils/shvmutex.h"
#include "shiva/include/framework/shveventthread.h"


class SHVThreadTester : public SHVTestQueued
{
public:

	// test modes
	enum Actions {
		ActionStartup = 0x01,
		ActionPounding = 0x02,
		ActionTickCount = 0x04,
	};

	SHVThreadTester(SHVModuleList& modules);
	virtual ~SHVThreadTester();

	virtual const SHVString8C GetGroup() const;
	virtual const SHVString8C GetID() const;
	virtual const SHVStringC GetTitle() const;
	virtual const SHVTestBase::Action* GetActions() const;
	virtual SHVEventQueue* GetQueue();

	// sub tests
private:
	static bool TestStartup(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestPounding(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestTickCount(SHVModuleList& modules, SHVTestBase* self, int flag);

	// implementation
private:
	SHVEventThread EventQueue;

	struct TestObject
	{
		SHVThreadTester* Self;

		SHVThread<TestObject> Thread1;
		SHVThread<TestObject> Thread2;
		SHVThreadBase PoundThread;
		bool ok;
		int Counter;

		inline TestObject(SHVTestBase* self) : Self((SHVThreadTester*)self) { ok = true; Counter = 0; }

		void ThreadFunc1();
		void ThreadFunc2();
		static void ThreadPound(void* data);
	};
};

#endif
