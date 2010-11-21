#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvmutextester.h"


/*************************************
 * Constructor
 *************************************/
SHVMutexTester::SHVMutexTester(SHVModuleList& modules) : SHVTestQueued()
{
	EventQueue.StartThread(modules);
}

/*************************************
 * Destructor
 *************************************/
SHVMutexTester::~SHVMutexTester()
{
	EventQueue.StopThread();
}


/*************************************
 * GetGroup
 *************************************/
const SHVString8C SHVMutexTester::GetGroup() const
{
	return "threadutils";
}

/*************************************
 * GetID
 *************************************/
const SHVString8C SHVMutexTester::GetID() const
{
	return "mutex";
}

/*************************************
 * GetTitle
 *************************************/
const SHVStringC SHVMutexTester::GetTitle() const
{
	return _S("SHVMutex and SHVMutexBase test");
}

/*************************************
 * GetQueue
 *************************************/
SHVEventQueue* SHVMutexTester::GetQueue()
{
	return &EventQueue;
}

/*************************************
 * GetActions
 *************************************/
const SHVTestBase::Action* SHVMutexTester::GetActions() const
{
static const SHVTestBase::Action actions[] = {
	{ ActionSimple, "simple", _S("simple"), _S("This will test simple mutex operations"), &SHVMutexTester::TestSimple },
	{ ActionSignalling, "signalling", _S("signalling"), _S("This will signalling using SHVMutexBase"), &SHVMutexTester::TestSignalling },
	{ ActionRefCount, "refcount", _S("refcount"), _S("This will test reference counting of SHVMutex"), &SHVMutexTester::TestRefCount },
	{ 0, NULL, NULL, NULL, NULL } }; // Termination

	return actions;
}

/*************************************
 * sub tests
 *************************************/
bool SHVMutexTester::TestSimple(SHVModuleList &modules, SHVTestBase *self, int flag)
{
bool ok = true;
TestObject tester(self);

	SHVUNUSED_PARAM(flag);

	ok = (ok && (tester.Thread1.Start(&tester,&SHVMutexTester::TestObject::SimpleThread)));
	ok = (ok && (tester.Thread2.Start(&tester,&SHVMutexTester::TestObject::SimpleTestThread)));

	ok = (ok && (tester.Thread1.IsRunning()));
	ok = (ok && (tester.Thread2.IsRunning()));

	SHVThreadBase::Sleep(750);

	ok = (ok && (!tester.Thread1.IsRunning()));
	ok = (ok && (!tester.Thread2.IsRunning()));
	ok = (ok && tester.ok);

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	return ok;
}
bool SHVMutexTester::TestSignalling(SHVModuleList &modules, SHVTestBase *self, int flag)
{
bool ok = true;
TestObject tester(self);

	SHVUNUSED_PARAM(flag);

	ok = (ok && (tester.Thread1.Start(&tester,&SHVMutexTester::TestObject::SignalThread)));
	ok = (ok && (tester.Thread2.Start(&tester,&SHVMutexTester::TestObject::EmitterThread)));

	ok = (ok && (tester.Thread1.IsRunning()));
	ok = (ok && (tester.Thread2.IsRunning()));

	SHVThreadBase::Sleep(1500);

	ok = (ok && (!tester.Thread1.IsRunning()));
	ok = (ok && (!tester.Thread2.IsRunning()));
	ok = (ok && tester.ok);

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	return ok;
}
bool SHVMutexTester::TestRefCount(SHVModuleList &modules, SHVTestBase *self, int flag)
{
bool ok = true;
TestObject tester(self);

	SHVUNUSED_PARAM(flag);

	ok = (ok && (tester.Thread1.Start(&tester,&SHVMutexTester::TestObject::RefLockThread)));
	ok = (ok && (tester.Thread2.Start(&tester,&SHVMutexTester::TestObject::RefLockTestThread)));

	ok = (ok && (tester.Thread1.IsRunning()));
	ok = (ok && (tester.Thread2.IsRunning()));

	SHVThreadBase::Sleep(1000);

	ok = (ok && (!tester.Thread1.IsRunning()));
	ok = (ok && (!tester.Thread2.IsRunning()));
	ok = (ok && tester.ok);

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	return ok;
}

/*************************************
 * Simple test threads
 *************************************/
void SHVMutexTester::TestObject::SimpleThread()
{
	ok = (ok && (Lock.Lock()));
	ok = (ok && (Lock.IsLocked()));
	ok = (ok && (Lock.HasLock()));

	ok = (ok && (Signaller.Lock()));
	ok = (ok && (Signaller.IsLocked()));
	ok = (ok && (!Signaller.Lock(250)));

	Signaller.Unlock();
	Lock.Unlock();
}
void SHVMutexTester::TestObject::SimpleTestThread()
{
	SHVThreadBase::Sleep(100);
	ok = (ok && (Lock.IsLocked()));
	ok = (ok && (!Lock.HasLock()));

	ok = (ok && (Signaller.IsLocked()));
}

/*************************************
 * Signaller test threads
 *************************************/
void SHVMutexTester::TestObject::SignalThread()
{
int iterations = 0;
	Lock.Lock();
	Signaller.Lock(); // set the signaller to "unsignalled"
	Val = 1;
	while (Val && ok)
	{
		ok = (ok && (++iterations == Val));

		Lock.Unlock();
		
		// wait for a signal
		Signaller.Lock();

		Lock.Lock();
	}
	Signaller.Unlock();
	Lock.Unlock();
}
void SHVMutexTester::TestObject::EmitterThread()
{
	SHVThreadBase::Sleep(100);

	for (int i=0; i<10; i++)
	{
		Lock.Lock();
		Val++;
		Signaller.Unlock(); // signal thread
		Lock.Unlock();
		SHVThreadBase::Sleep(25);
	}

	Lock.Lock();
	ok = (ok && (Thread1.IsRunning()));
	Val=0;
	Signaller.Unlock(); // signal thread
	Lock.Unlock();
}

/*************************************
 * RefCount test threads
 *************************************/
void SHVMutexTester::TestObject::RefLockThread()
{
int i=0;
	Val = 1;
	// lock them
	for (i=0; i<10; i++)
	{
		Lock.Lock();
		ok = (ok && (Lock.IsLocked()));
		ok = (ok && (Lock.HasLock()));
	}

	SHVThreadBase::Sleep(100);
	Val = 0;
	i=Lock.UnlockAll();
	SHVThreadBase::Sleep(50);
	Lock.LockMultiple(i);
	Val = 1;

	for (i=0; Lock.HasLock(); i++)
	{
		Lock.Unlock();
	}
	ok = (ok && (i==10));
}
void SHVMutexTester::TestObject::RefLockTestThread()
{
	SHVThreadBase::Sleep(50);
	ok = (ok && (Lock.GetLockCount() == 10));
	ok = (ok && (Lock.Lock()));
	ok = (ok && (Lock.HasLock()));

	SHVThreadBase::Sleep(250);

	ok = (ok && (!Val));

	Lock.Unlock();
}
