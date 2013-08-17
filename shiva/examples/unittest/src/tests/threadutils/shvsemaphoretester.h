#ifndef __SHIVA_TESTS_THREADUTILS_SEMAPHORE_H
#define __SHIVA_TESTS_THREADUTILS_SEMAPHORE_H

#include "shiva/include/unittest/src/shvtestqueued.h"

#include "shiva/include/threadutils/shvsemaphore.h"
#include "shiva/include/framework/shveventthread.h"


class SHVSemaphoreTester : public SHVTestQueued
{
public:

	// test modes
	enum Actions {
		ActionSimple = 0x01,
		ActionResourceCount = 0x02,
	};

	SHVSemaphoreTester(SHVModuleList& modules);
	virtual ~SHVSemaphoreTester();

	virtual const SHVString8C GetGroup() const;
	virtual const SHVString8C GetID() const;
	virtual const SHVStringC GetTitle() const;
	virtual const SHVTestBase::Action* GetActions() const;
	virtual SHVEventQueue* GetQueue();

	// sub tests
private:
	static bool TestSimple(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestResourceCount(SHVModuleList& modules, SHVTestBase* self, int flag);

	// implementation
private:
	SHVEventThread EventQueue;

	struct TestObject
	{
		SHVSemaphoreTester* Self;

		SHVThread<TestObject> Thread1;
		SHVThread<TestObject> Thread2;
		bool ok;
		int Val;

		SHVSemaphore Semaphore;

		inline TestObject(SHVTestBase* self) : Self((SHVSemaphoreTester*)self), Semaphore(0) { ok = true; Val = 0; }

		// Simple test threads
		void SimpleThread();
		void SimpleTestThread();

		// ResourceCount test threads
		void ResourceCountThread();
		void ResourceCountTestThread();
	};
};

#endif
