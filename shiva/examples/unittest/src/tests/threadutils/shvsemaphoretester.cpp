#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvsemaphoretester.h"


/*************************************
 * Constructor
 *************************************/
SHVSemaphoreTester::SHVSemaphoreTester(SHVModuleList& modules) : SHVTestQueued()
{
	EventQueue.StartThread(modules);
}

/*************************************
 * Destructor
 *************************************/
SHVSemaphoreTester::~SHVSemaphoreTester()
{
	EventQueue.StopThread();
}


/*************************************
 * GetGroup
 *************************************/
const SHVString8C SHVSemaphoreTester::GetGroup() const
{
	return "threadutils";
}

/*************************************
 * GetID
 *************************************/
const SHVString8C SHVSemaphoreTester::GetID() const
{
	return "semaphore";
}

/*************************************
 * GetTitle
 *************************************/
const SHVStringC SHVSemaphoreTester::GetTitle() const
{
	return _S("SHVSemaphore test");
}

/*************************************
 * GetQueue
 *************************************/
SHVEventQueue* SHVSemaphoreTester::GetQueue()
{
	return &EventQueue;
}

/*************************************
 * GetActions
 *************************************/
const SHVTestBase::Action* SHVSemaphoreTester::GetActions() const
{
static const SHVTestBase::Action actions[] = {
	{ ActionSimple, "simple", _S("simple"), _S("This will test simple mutex operations"), &SHVSemaphoreTester::TestSimple },
	{ ActionResourceCount, "resourcecount", _S("resourcecount"), _S("Tests locking multiple resources"), &SHVSemaphoreTester::TestResourceCount },
	{ 0, NULL, NULL, NULL, NULL } }; // Termination

	return actions;
}

/*************************************
 * sub tests
 *************************************/
bool SHVSemaphoreTester::TestSimple(SHVModuleList &modules, SHVTestBase *self, int flag)
{
bool ok = true;
TestObject tester(self);

	SHVUNUSED_PARAM(flag);

	ok = (ok && (tester.Thread1.Start(&tester,&SHVSemaphoreTester::TestObject::SimpleThread)));
	ok = (ok && (tester.Thread2.Start(&tester,&SHVSemaphoreTester::TestObject::SimpleTestThread)));

	ok = (ok && (tester.Thread1.IsRunning()));
	ok = (ok && (tester.Thread2.IsRunning()));

	SHVThreadBase::Sleep(750);

	ok = (ok && (!tester.Thread1.IsRunning()));
	ok = (ok && (!tester.Thread2.IsRunning()));
	ok = (ok && tester.ok);

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	return ok;
}
bool SHVSemaphoreTester::TestResourceCount(SHVModuleList &modules, SHVTestBase *self, int flag)
{
bool ok = true;
TestObject tester(self);

	SHVUNUSED_PARAM(flag);

	ok = (ok && (tester.Thread1.Start(&tester,&SHVSemaphoreTester::TestObject::ResourceCountThread)));
	ok = (ok && (tester.Thread2.Start(&tester,&SHVSemaphoreTester::TestObject::ResourceCountTestThread)));

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
void SHVSemaphoreTester::TestObject::SimpleThread()
{
long tickStart = SHVThreadBase::GetTicksInMilliSecs();

	ok = (ok && (Semaphore.GetResourceCount() == 0));
	Semaphore.Signal(); // Create one resource this way
	ok = (ok && (Semaphore.GetResourceCount() == 1));
	Semaphore.Wait(); // Remove the surplus resource
	ok = (ok && (Semaphore.GetResourceCount() == 0));
	ok = (ok && (SHVThreadBase::GetTicksInMilliSecs()-tickStart < 50));

	Semaphore.Wait(); // Block here until SimpleTestThread has signalled
	ok = (ok && (SHVThreadBase::GetTicksInMilliSecs()-tickStart > 75));
}
void SHVSemaphoreTester::TestObject::SimpleTestThread()
{
	SHVThreadBase::Sleep(100);
	Semaphore.Signal();
}

/*************************************
 * RefCount test threads
 *************************************/
void SHVSemaphoreTester::TestObject::ResourceCountThread()
{
	Semaphore.Lock();
	Semaphore.Signal(5); // We now have 5 resources
	SHVThreadBase::Sleep(100);
	ok = (ok && (Semaphore.GetResourceCount() == 5));
	Semaphore.Unlock();
	SHVThreadBase::Sleep(100);
	Semaphore.Signal();
}
void SHVSemaphoreTester::TestObject::ResourceCountTestThread()
{
long tickStart = SHVThreadBase::GetTicksInMilliSecs();
	SHVThreadBase::Sleep(50);
	Semaphore.Wait(2);
	ok = (ok && (SHVThreadBase::GetTicksInMilliSecs()-tickStart > 75 && SHVThreadBase::GetTicksInMilliSecs()-tickStart < 200));
	Semaphore.Wait(2);
	ok = (ok && (SHVThreadBase::GetTicksInMilliSecs()-tickStart > 75 && SHVThreadBase::GetTicksInMilliSecs()-tickStart < 200));
	Semaphore.Wait(2);
	ok = (ok && (SHVThreadBase::GetTicksInMilliSecs()-tickStart > 175));
}
