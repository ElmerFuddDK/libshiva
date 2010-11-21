#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvthreadtester.h"

/*************************************
 * Constructor
 *************************************/
SHVThreadTester::SHVThreadTester(SHVModuleList& modules) : SHVTestQueued()
{
	EventQueue.StartThread(modules);
}

/*************************************
 * Destructor
 *************************************/
SHVThreadTester::~SHVThreadTester()
{
	EventQueue.StopThread();
}


/*************************************
 * GetGroup
 *************************************/
const SHVString8C SHVThreadTester::GetGroup() const
{
	return "threadutils";
}

/*************************************
 * GetID
 *************************************/
const SHVString8C SHVThreadTester::GetID() const
{
	return "thread";
}

/*************************************
 * GetTitle
 *************************************/
const SHVStringC SHVThreadTester::GetTitle() const
{
	return _S("SHVThread and SHVThreadBase test");
}

/*************************************
 * GetQueue
 *************************************/
SHVEventQueue* SHVThreadTester::GetQueue()
{
	return &EventQueue;
}

/*************************************
 * GetActions
 *************************************/
const SHVTestBase::Action* SHVThreadTester::GetActions() const
{
static const SHVTestBase::Action actions[] = {
	{ ActionStartup, "startup", _S("startup"), _S("Tests thread startup"), &SHVThreadTester::TestStartup },
	{ ActionPounding, "pounding", _S("pounding"), _S("Tests thread startup"), &SHVThreadTester::TestPounding },
	{ 0, NULL, NULL, NULL, NULL } }; // Termination

	return actions;
}

/*************************************
 * sub tests
 *************************************/
bool SHVThreadTester::TestStartup(SHVModuleList &modules, SHVTestBase *self, int flag)
{
bool ok = true;
TestObject tester(self);

	SHVUNUSED_PARAM(flag);

	ok = (ok && (tester.Thread1.Start(&tester,&SHVThreadTester::TestObject::ThreadFunc1)));
	ok = (ok && (tester.Thread2.Start(&tester,&SHVThreadTester::TestObject::ThreadFunc2)));

	ok = (ok && (tester.Thread1.IsRunning()));
	ok = (ok && (tester.Thread2.IsRunning()));

	SHVThreadBase::Sleep(1000);

	ok = (ok && (!tester.Thread1.IsRunning()));
	ok = (ok && (!tester.Thread2.IsRunning()));
	ok = (ok && tester.ok);

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	return ok;
}
bool SHVThreadTester::TestPounding(SHVModuleList &modules, SHVTestBase *self, int flag)
{
bool ok = true;
TestObject tester(self);

	SHVUNUSED_PARAM(flag);

	ok = (ok && (tester.PoundThread.Start(&SHVThreadTester::TestObject::ThreadPound,&tester)));

	for (int i = 0; ok && i<100 && tester.Counter<100; i++)
		SHVThreadBase::Sleep(100);

	ok = (ok && (tester.Counter == 100));
	ok = (ok && (!tester.PoundThread.IsRunning()));

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	return ok;
}

/*************************************
 * implementation
 *************************************/
void SHVThreadTester::TestObject::ThreadFunc1()
{
	SHVThreadBase::Sleep(500);
}
void SHVThreadTester::TestObject::ThreadFunc2()
{
	SHVThreadBase::Sleep(200);
	ok = (ok && (Thread1.IsRunning()));
	SHVThreadBase::Sleep(600);
	ok = (ok && (!Thread1.IsRunning()));
}
void SHVThreadTester::TestObject::ThreadPound(void* data)
{
TestObject* self = (TestObject*)data;
	self->PoundThread.ResetHandle();
	SHVRefObject::LockedIncrement(&self->Counter);
	if (self->Counter < 100)
		self->PoundThread.Start(&ThreadPound,self);
}
