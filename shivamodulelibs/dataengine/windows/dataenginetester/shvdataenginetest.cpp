/*
 *   Copyright (C) 2008 by Mogens Bak Nielsen, Lars Eriksen
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of the
 *   License, or (at your option) any later version with the following
 *   exeptions:
 *
 *   1) Static linking to the library does not constitute derivative work
 *      and does not require the author to provide source code for the
 *      application.
 *      Compiling applications with the source code directly linked in is
 *      Considered static linking as well.
 *
 *   2) You do not have to provide a copy of the license with programs
 *      that are linked against this code.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "stdafx.h"
#include "../../../../include/platformspc.h"
#include "../../../../include/utils/shvfile.h"
#include "../../../../include/threadutils/shvreadwritelock.h"
#include "../../../../include/threadutils/shvmutexlocker.h"

#include "shvdataenginetest.h"
#include "../../../../include/modules/dataengine/shvdataengine.h"
#include "../../../../include/modules/dataengine/shvdatastruct.h"
#include "../../../../include/modules/dataengine/shvdatarowc.h"
#include "../../../../include/modules/dataengine/shvdatarow.h"
#include "../../../../include/modules/dataengine/shvdatarowlistc.h"
#include "../../../../include/modules/dataengine/shvdatarowlist.h"

// ====================================== Test utility function =====================================
void DumpRow(SHVTestResult* result, const SHVDataRowC* row)
{
	if (row)
	{
	SHVString val;
	SHVString msg;
	const SHVDataStructC& Struct = *row->GetStruct();
		for (size_t idx = 0; idx < Struct.GetColumnCount(); idx++)
		{
			if (idx)
				msg += _S(" ");
			if (Struct[idx]->GetDataType() == SHVDataVariant::TypeString)
			{
				val.Format(_S("%-20s"), row->AsString(idx).GetSafeBuffer());
			}
			else
				val.Format(_S("%12s"), row->AsString(idx).GetSafeBuffer());
			msg += val;
		}
		result->AddLog(msg);
	}
	else
		result->AddLog(_S("<Empty row>"));
}

SHVBool DumpData(SHVTestResult* result, SHVDataRowListC* data)
{
SHVBool retVal;
	while ((retVal = data->NextRow()))
	{
		DumpRow(result, data->GetCurrentRow());
	}
	if (retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
		retVal = SHVBool::True;
	return retVal;
}
// ============================================== Test utility class ================================

class TimeIt
{
private:
	size_t Ticks;
	SHVTestResult* Result;
	SHVString TestName;
public:
	inline TimeIt(SHVTestResult* result, const SHVString& testName): TestName(testName) 
	{ 
		Ticks = GetTickCount(); 
		Result = result; 
	}
	inline size_t CurrentUsed() { return GetTickCount() - Ticks; }
	inline ~TimeIt()
	{
		Result->AddLog(_S("Test %s took %u seconds"), TestName.GetSafeBuffer(), (GetTickCount() - Ticks) / 1000);
		Result = NULL;
	}
};


class RandomServer
{
private:
	SHVThread<RandomServer> RThread;
	int CurrentNumber;
	SHVMutexBase Signaler;
	SHVMutexBase ResultSignaler;
	SHVMutex Lock;
	bool Running;

	void SetRandom() 
	{
		Signaler.Lock();
		ResultSignaler.Unlock();
		while (Running)
		{
			Signaler.Lock();
			CurrentNumber = rand();
			ResultSignaler.Unlock();
		}
		Signaler.Unlock();
	}
public:
	RandomServer(): Running(true) 
	{
		ResultSignaler.Lock();
		RThread.Start(this, &RandomServer::SetRandom); 
		ResultSignaler.Lock();
		ResultSignaler.Unlock();
	}

	~RandomServer()
	{
	SHVMutexLocker lock(Lock);
		Running = false;
		ResultSignaler.Lock();
		Signaler.Unlock();
		ResultSignaler.Lock();
		ResultSignaler.Unlock();
		while (RThread.IsRunning()) SHVThreadBase::Sleep(10);
	}

	int GetNumber() 
	{ 
	SHVMutexLocker lock(Lock);
		ResultSignaler.Lock();
		Signaler.Unlock();
		ResultSignaler.Lock();
		ResultSignaler.Unlock();
		return CurrentNumber;
	}
};

class TestIt
{
public:
	SHVDataEngine* DataEngine;
	SHVString TestName;
	SHVTestResult* Result;
	TimeIt Timer;
	RandomServer Rand;

public:
	inline TestIt(SHVDataEngine* dataEngine, SHVTestResult* result, const SHVStringC& testName):
		DataEngine(dataEngine), 
		TestName(testName), 
		Result(result),
		Timer(result, testName)
		{
		}
	virtual SHVBool PerformTest() = 0;
};

class TestThread
{
private:
	SHVThread<TestThread> Thread;
	SHVReadWriteLock* TermLock;
	SHVMutexBase StartLock;
	SHVBool Ok;
protected:
	TestIt* Owner;
	SHVString ThreadName;
public:
	inline bool IsRunning() { return Thread.IsRunning(); }

	inline TestThread(const SHVStringC& threadName): ThreadName(threadName) {}
	inline TestThread::~TestThread()
	{
		while (IsRunning())
			SHVThreadBase::Sleep(10);
	}
	inline void Start(TestIt* owner, SHVReadWriteLock* termLock)
	{
		Owner = owner;
		TermLock = termLock;
		StartLock.Lock();
		Thread.Start(this, &TestThread::BeginTheWork);
		StartLock.Lock();
		StartLock.Unlock();
	}
	inline SHVBool GetOk() { return Ok; }
	inline const SHVStringC& GetName() { return ThreadName; }

	virtual void BeginTheWork()
	{
		StartLock.Unlock();
		TermLock->LockShared();
		Ok = DoTheWork();
		TermLock->Unlock();
	}

	virtual SHVBool DoTheWork() = 0;
};

class TestThreadServer: public TestIt
{
private:
	SHVList<TestThread*> Tests;
	SHVReadWriteLock Lock;

public:
	inline TestThreadServer(SHVDataEngine* dataEngine, SHVTestResult* result, const SHVStringC& testName):
		TestIt(dataEngine, result, testName) 
		{
		}
	inline void AddTest(TestThread& t)
	{
		Tests.AddTail(&t);
	}
	virtual SHVBool PerformTest()
	{
	SHVListIterator<TestThread*> Iter(Tests);
	SHVBool retVal = SHVBool::True;
		Lock.LockExclusive();
		while (Iter.MoveNext())
			Iter.Get()->Start(this, &Lock);
		Lock.Unlock();
		Lock.LockExclusive();
		while (Iter.MoveNext())
		{
			if (!Iter.Get()->GetOk())
				Result->AddLog(_S("Test thread %s failed"), Iter.Get()->GetName().GetSafeBuffer());
			if (retVal)
				retVal = Iter.Get()->GetOk();
		}
		return retVal;
	}
};

// =============================================== Unit test ========================================
class UnitTest: public TestIt
{
public:
	inline UnitTest(SHVDataEngine* dataEngine, SHVTestResult* result, const SHVStringC& testName):
		TestIt(dataEngine, result, testName) 
		{
		}
	virtual SHVBool PerformTest();
};

SHVBool UnitTest::PerformTest()
{
SHVDataStructRef unitTable = DataEngine->CreateStruct();
SHVDataRowKeyRef key;
SHVDataSessionRef DataSession;
	DataSession = DataEngine->CreateSession();
	DataSession->StartEdit();

	unitTable->SetTableName("unittest");
	unitTable->Add("key", SHVDataVariant::TypeInt);
	unitTable->Add("fstring", SHVDataVariant::TypeString);
	unitTable->Add("fbool", SHVDataVariant::TypeBool);
	unitTable->Add("fint", SHVDataVariant::TypeInt);
	unitTable->Add("fint64", SHVDataVariant::TypeInt64);
	unitTable->Add("fdouble", SHVDataVariant::TypeDouble);
	unitTable->Add("ftime", SHVDataVariant::TypeTime);
	unitTable->SetIsMultiInstance(true);
	key = DataEngine->CreateKey();
	key->AddKey("key", false);
	unitTable->SetPrimaryIndex(key);
	if (DataEngine->RegisterTable(unitTable, DataSession) && 
		DataEngine->RegisterAlias("unittest", "unittest1", true, DataSession))
	{
	SHVDataRowListRef rows = DataSession->GetRows("unittest1", _S(""), 0);
	SHVDataRowListRef copied;
	SHVDataRowRef r;
	SHVTime t;
		t.SetNow();
		r = rows->AddRow();
		r->SetInt(0, 1);
		r->SetString(1, _S("test 1"));
		r->SetBool(2, SHVBool::True);
		r->SetInt(3, 1);
		r->SetInt64(4, 0x0000100000000001);
		r->SetDouble(5, 0.15);
		r->SetTime(6, t);
		r->AcceptChanges();

		t.AddSeconds(10);
		r = rows->AddRow();
		r->SetInt(0, 2);
		r->SetString(1, _S("test 2"));
		r->SetBool(2, SHVBool::True);
		r->SetInt(3, 2);
		r->SetInt64(4, 0x0000100000000002);
		r->SetDouble(5, 0.30);
		r->SetTime(6, t);
		r->AcceptChanges();

		copied = DataSession->CopyAlias("unittest1", "unittest2");
		DumpData(Result, rows);
		DumpData(Result, copied);
		rows = NULL;
		copied = NULL;
		DataEngine->UnregisterAlias("unittest1");
		DataEngine->UnregisterAlias("unittest2");
		DataSession->Commit();
		return SHVBool::True;
	}
	else
		DataSession->Rollback();
	return SHVBool::False;
}


// ==================================================== Test ========================================

class TestShareEx: public TestThread
{
private:
	int Runs;
public:
	inline TestShareEx(const SHVStringC& threadName, int runs): TestThread(threadName), Runs(runs) {}
	virtual SHVBool DoTheWork();
};


SHVBool TestShareEx::DoTheWork()
{
SHVDataSessionRef DataSession;
SHVDataRowListRef DataList;
SHVDataRowRef DataRow;
SHVDataRowKeyRef Key;
SHVBool ok;
	
	Owner->Result->AddLog(_S("%s is started"), ThreadName.GetSafeBuffer());
	DataSession = Owner->DataEngine->CreateSession();
	ok = Owner->DataEngine->RegisterAlias("testtable", ThreadName.ToStr8(), true, DataSession);
	if (!ok)
	{
		Owner->Result->AddLog(_S("Error: %s"), DataSession->GetErrorMessage().GetSafeBuffer());
	}
	for (int i = 0; ok && i < Runs; i++)
	{
		DataList = DataSession->GetRows(ThreadName.ToStr8(), _S(""), 0);
		if (!DataList->IsOk())
		{
			Owner->Result->AddLog(_S("Could not load %s"), ThreadName.GetSafeBuffer());
		}
		ok = DataList->StartEdit();
		if (ok)
		{
			// SHVThreadBase::Sleep(Owner->Rand.GetNumber() % 300);
			for (int j = 0; ok && j < 10; j++)
			{
				DataRow = DataList->AddRow();
				DataRow->SetInt(0, (i*10) + j);
				DataRow->SetString(1, SHVStringC::Format(_S("%s_%d"), ThreadName.GetSafeBuffer(),(i*10) + j));
				DataRow->SetString(2, SHVStringC::Format(_S("z%d"), (i*10) + j));
				ok = DataRow->AcceptChanges();
			}
			if (!ok)
				Owner->Result->AddLog(_S("Error: %s"), DataSession->GetErrorMessage().GetSafeBuffer());
			ok = DataList->EndEdit();
			if (ok)
			{
				DataList->Reset();
				while (DataList->NextRow())
				{
					DumpRow(Owner->Result, DataList->GetCurrentRow());
					// SHVThreadBase::Sleep(Owner->Rand.GetNumber() % 50);
				}
			}
			else
				Owner->Result->AddLog(_S("Error: %s"), DataSession->GetErrorMessage().GetSafeBuffer());
			// Lets edit a record
			if (ok)
			{
				DataList->Reset();
				ok = DataList->StartEdit();
				if (ok)
				{
					Key = DataSession->GetFactory()->CopyKey(DataList->GetStruct()->GetPrimaryIndex());
					Key->SetKeyValue(0, SHVInt((i*10) + 5));
					
					if (DataList->SetPosition(Key))
					{
						DataRow = DataList->EditCurrentRow();
						DataRow->SetString(2, _S("nisse"));
						ok = DataRow->AcceptChanges();
					}
					if (ok)
						DataList->EndEdit();
					else
					{
						Owner->Result->AddLog(_S("Error: %s"), DataSession->GetErrorMessage().GetSafeBuffer());
						DataList->CancelEdit();
					}
				}
				else
					Owner->Result->AddLog(_S("Error: %s"), DataSession->GetErrorMessage().GetSafeBuffer());
			}
			// Let delete a record
			if (ok)
			{
				ok = DataList->StartEdit();
				if (ok)
				{
					Key = DataSession->GetFactory()->CopyKey(DataList->GetStruct()->GetPrimaryIndex());
					Key->SetKeyValue(0, SHVInt((i*10) + 3));
					
					if (DataList->SetPosition(Key))
					{
						DataRow = DataList->EditCurrentRow();
						DataRow->Delete();
						ok = DataRow->AcceptChanges();
					}
					if (ok)
						DataList->EndEdit();
					else
					{
						Owner->Result->AddLog(_S("Error: %s"), DataSession->GetErrorMessage().GetSafeBuffer());
						DataList->CancelEdit();
					}
				}
				else
					Owner->Result->AddLog(_S("Error: %s"), DataSession->GetErrorMessage().GetSafeBuffer());
			}
		}
		else
			Owner->Result->AddLog(_S("Error: %s"), DataSession->GetErrorMessage().GetSafeBuffer());
	}
	DataSession = NULL;
	DataList = NULL;
	DataRow = NULL;
	return ok;
}

// ==================================================== Test ========================================

class TestNotMultiInstance: public TestThread
{
private:
	int Runs;
	int IdOffset;
public:
	inline TestNotMultiInstance(const SHVStringC& threadName, int runs, int idOffset): TestThread(threadName), Runs(runs), IdOffset(idOffset) {}
	virtual SHVBool DoTheWork();
};


SHVBool TestNotMultiInstance::DoTheWork()
{
SHVDataSessionRef DataSession;
SHVDataRowListRef DataList;
SHVDataRowRef DataRow;
SHVBool ok;
SHVDataRowKeyRef Key;
int id;
	
	Owner->Result->AddLog(_S("%s is started"), ThreadName.GetSafeBuffer());
	DataSession = Owner->DataEngine->CreateSession();
	ok = DataSession->SessionValid();
	for (int i = 0; i < Runs && ok; i++)
	{
		DataList = DataSession->GetRows("testtable_nisse", _S(""), 0);
		if (!DataList->IsOk())
		{
			Owner->Result->AddLog(_S("Could not load %s"), ThreadName.GetSafeBuffer());
		}
		ok = DataList->StartEdit();
		if (ok)
		{
			for (int j = 0; j < 10 && ok; j++)
			{
				id = (i*10) + j + IdOffset;
				DataRow = DataList->AddRow();
				DataRow->SetInt(0, id);
				DataRow->SetString(1, SHVStringC::Format(_S("%s_%d"), ThreadName.GetSafeBuffer(),id));
				DataRow->SetString(2, SHVStringC::Format(_S("z%d"), id));
				ok = DataRow->AcceptChanges();
			}
			if (!ok)
			{
				Owner->Result->AddLog(_S("Error: %s"), DataSession->GetErrorMessage().GetSafeBuffer());
				DataList->CancelEdit();
			}
			else
				ok = DataList->EndEdit();
		}
		else
			Owner->Result->AddLog(_S("Error: %s"), DataSession->GetErrorMessage().GetSafeBuffer());
		// Lets edit a record
		if (ok)
		{
			ok = DataList->StartEdit();
			if (ok)
			{
				Key = DataSession->GetFactory()->CopyKey(DataList->GetStruct()->GetPrimaryIndex());
				Key->SetKeyValue(0, SHVInt((i*10) + 5 + IdOffset));
				
				if (DataList->SetPosition(Key))
				{
					DataRow = DataList->EditCurrentRow();
					DataRow->SetString(1, _S("nisse"));
					ok = DataRow->AcceptChanges();
				}
				if (ok)
					DataList->EndEdit();
				else
				{
					Owner->Result->AddLog(_S("Error: %s"), DataSession->GetErrorMessage().GetSafeBuffer());
					DataList->CancelEdit();
				}
			}
			else
				Owner->Result->AddLog(_S("Error: %s"), DataSession->GetErrorMessage().GetSafeBuffer());
		}
		// Let delete a record
		if (ok)
		{
			ok = DataList->StartEdit();
			if (ok)
			{
				Key = DataSession->GetFactory()->CopyKey(DataList->GetStruct()->GetPrimaryIndex());
				Key->SetKeyValue(0, SHVInt((i*10) + 3 + IdOffset));
				
				if (DataList->SetPosition(Key))
				{
					DataRow = DataList->EditCurrentRow();
					DataRow->Delete();
					ok = DataRow->AcceptChanges();
				}
				if (ok)
					DataList->EndEdit();
				else
				{
					Owner->Result->AddLog(_S("Error: %s"), DataSession->GetErrorMessage().GetSafeBuffer());
					DataList->CancelEdit();
				}
			}
			else
				Owner->Result->AddLog(_S("Error: %s"), DataSession->GetErrorMessage().GetSafeBuffer());
		}

	}
	DataSession = NULL;
	DataList = NULL;
	DataRow = NULL;
	return ok;
}
// ==================================================== Test ========================================
void SHVDataEngineTest::PerformTest(SHVTestResult* result)
{
SHVBool ok;
SHVDataStructRef Struct = DataEngine->CreateStruct();
SHVDataRowKeyRef Key;
/*
TestThreadServer testServer(DataEngine, result, _S("TestSharedExclusiveLocks"));
TestShareEx ex1(_S("T1"), 10);
TestShareEx ex2(_S("T2"), 10);
TestShareEx ex3(_S("T3"), 10);
TestShareEx ex4(_S("T4"), 10);
TestShareEx ex5(_S("T5"), 10);
TestNotMultiInstance tm1(_S("T6"), 10, 0);
TestNotMultiInstance tm2(_S("T7"), 10, 100);

	Struct->SetTableName("testtable");
	Struct->Add("id", SHVDataVariant::TypeInt);
	Struct->Add("val1", SHVDataVariant::TypeString, 20);
	Struct->Add("val2", SHVDataVariant::TypeString, 20);

	Key = Struct->CreateIndexKey();
	Key->AddKey("id", false);
	Struct->AddIndex(Key);

	Key = Struct->CreateIndexKey();
	Key->AddKey("val1", false);
	Struct->AddIndex(Key);
	Struct->SetIsMultiInstance(true);

	DataEngine->RegisterTable(Struct);

	Struct = DataEngine->CreateStruct();
	Struct->SetTableName("testtable_nisse");
	Struct->Add("id", SHVDataVariant::TypeInt);
	Struct->Add("val1", SHVDataVariant::TypeString, 20);
	Struct->Add("val2", SHVDataVariant::TypeString, 20);

	Key = Struct->CreateIndexKey();
	Key->AddKey("id", false);
	Struct->AddIndex(Key);

	Key = Struct->CreateIndexKey();
	Key->AddKey("val1", false);
	Struct->AddIndex(Key);
	Struct->SetIsMultiInstance(false);
	DataEngine->RegisterTable(Struct);
	DataEngine->ClearTable("testtable_nisse");

	testServer.AddTest(ex1);
	testServer.AddTest(ex2);
	testServer.AddTest(ex3);
	testServer.AddTest(ex4);
	testServer.AddTest(ex5);
	testServer.AddTest(tm1);
	testServer.AddTest(tm2);

	*result = testServer.PerformTest();
*/
UnitTest t(DataEngine, result, _S("Unit test"));
	t.PerformTest();
}


SHVBool SHVDataEngineTest::Register()
{
SHVBool retVal = SHVBool::True;
SHVDataStructRef dataStruct;
SHVDataRowKeyRef key;

	DataEngine = (SHVDataEngine*) Modules.ResolveModule("DataEngine");
	retVal = DataEngine != NULL;
	/*
	if (retVal)
	{
		DataEngine->EventSubscribe(SHVDataFactory::EventRowChanged, new SHVEventSubscriber(this));
		dataStruct = DataEngine->CreateStruct();

		// Table test
		dataStruct->SetTableName("test");
		dataStruct->Add("key", SHVDataVariant::TypeInt);
		dataStruct->Add("sort", SHVDataVariant::TypeTime);
		dataStruct->Add("descript1", SHVDataVariant::TypeString, 20);
		dataStruct->Add("descript2", SHVDataVariant::TypeString, 20);
		key = dataStruct->CreateIndexKey();
		key->AddKey("key", false);
		dataStruct->AddIndex(key);
		key = dataStruct->CreateIndexKey();
		key->AddKey("descript1", false);
		key->AddKey("key", false);
		dataStruct->AddIndex(key);
		DataEngine->RegisterTable(dataStruct);
		DataEngine->RegisterAlias("test", "test", true);

		// Table testchild
		dataStruct = DataEngine->CreateStruct();
		dataStruct->SetTableName("testchild");
		dataStruct->Add("key", SHVDataVariant::TypeInt);
		dataStruct->Add("descript3", SHVDataVariant::TypeString, 20);
		key = dataStruct->CreateIndexKey();
		key->AddKey("key", false);
		dataStruct->AddIndex(key);
		DataEngine->RegisterTable(dataStruct);
		DataEngine->RegisterAlias("testchild", "testchild", true);

		// Table testperson
		dataStruct = DataEngine->CreateStruct();
		dataStruct->SetTableName("person");
		dataStruct->Add("pk_person", SHVDataVariant::TypeInt);
		dataStruct->Add("firstName", SHVDataVariant::TypeString,40);
		dataStruct->Add("middleName", SHVDataVariant::TypeString,30);
		dataStruct->Add("lastName", SHVDataVariant::TypeString,40);
		key = dataStruct->CreateIndexKey();
		key->AddKey("pk_person", false);
		dataStruct->AddIndex(key);

		key = dataStruct->CreateIndexKey();
		key->AddKey("firstName", false);
		key->AddKey("middleName", false);
		key->AddKey("lastName", false);
		dataStruct->AddIndex(key);

		DataEngine->RegisterTable(dataStruct);
	}
	*/

	return SHVTestModule::Register() && retVal;
}

void SHVDataEngineTest::PostRegister()
{
}

void SHVDataEngineTest::OnEvent(SHVEvent* event)
{
	if (trace)
	{
	SHVDataRow* row = SHVEventDataRowChanged::Get(event);
		switch (row->GetRowState())
		{
			case SHVDataRow::RowStateAdding:
				Result->AddLog(_S("Row adding"));
				break;
			case SHVDataRow::RowStateAdded:
				Result->AddLog(_S("Row added"));
				break;
			case SHVDataRow::RowStateDeleting:
				Result->AddLog(_S("Row deleting"));
				break;
			case SHVDataRow::RowStateDeleted:
				Result->AddLog(_S("Row deleted"));
				break;
			case SHVDataRow::RowStateChanging:
				Result->AddLog(_S("Row changing"));
				break;
			case SHVDataRow::RowStateChanged:
				Result->AddLog(_S("Row changed"));
				break;
		}
		if (row->GetRowState() != SHVDataRow::RowStateAdding && row->RowValid())
			DumpRow(Result, row);	
	}
}
