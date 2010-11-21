#ifndef __SHIVA_TESTS_THREADUTILS_MUTEX_H
#define __SHIVA_TESTS_THREADUTILS_MUTEX_H

#include "../shvtestqueued.h"

#include "shiva/include/threadutils/shvmutex.h"
#include "shiva/include/framework/shveventthread.h"


class SHVMutexTester : public SHVTestQueued
{
public:

	// test modes
	enum Actions {
		ActionSimple = 0x01,
		ActionSignalling = 0x02,
		ActionRefCount = 0x04,
	};

	SHVMutexTester(SHVModuleList& modules);
	virtual ~SHVMutexTester();

	virtual const SHVString8C GetGroup() const;
	virtual const SHVString8C GetID() const;
	virtual const SHVStringC GetTitle() const;
	virtual const SHVTestBase::Action* GetActions() const;
	virtual SHVEventQueue* GetQueue();

	// sub tests
private:
	static bool TestSimple(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestSignalling(SHVModuleList& modules, SHVTestBase* self, int flag);
	static bool TestRefCount(SHVModuleList& modules, SHVTestBase* self, int flag);

	// implementation
private:
	SHVEventThread EventQueue;

	struct TestObject
	{
		SHVMutexTester* Self;

		SHVThread<TestObject> Thread1;
		SHVThread<TestObject> Thread2;
		bool ok;
		int Val;

		SHVMutex Lock;
		SHVMutexBase Signaller;

		inline TestObject(SHVTestBase* self) : Self((SHVMutexTester*)self) { ok = true; Val = 0; }

		// Simple test threads
		void SimpleThread();
		void SimpleTestThread();

		// Signaller test threads
		void SignalThread();
		void EmitterThread();

		// RefCount test threads
		void RefLockThread();
		void RefLockTestThread();
	};
};

#endif
