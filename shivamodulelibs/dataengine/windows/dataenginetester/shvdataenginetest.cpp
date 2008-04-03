#include "stdafx.h"
#include "../../../../include/platformspc.h"
#include "../../../../include/utils/shvfile.h"

#include "shvdataenginetest.h"
#include "../../../../include/modules/dataengine/shvdataengine.h"
#include "../../../../include/modules/dataengine/shvdatastruct.h"
#include "../../../../include/modules/dataengine/shvdatarowc.h"
#include "../../../../include/modules/dataengine/shvdatarow.h"
#include "../../../../include/modules/dataengine/shvdatarowlistc.h"
#include "../../../../include/modules/dataengine/shvdatarowlist.h"

// ============================================== Test utility function ======================================
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
				msg += _T(" ");
			if (Struct[idx]->GetDataType() == SHVDataVariant::TypeString)
			{
				val.Format(_T("%-20s"), row->AsString(idx).GetSafeBuffer());
			}
			else
				val.Format(_T("%12s"), row->AsString(idx).GetSafeBuffer());
			msg += val;
		}
		result->AddLog(msg);
	}
	else
		result->AddLog(_T("<Empty row>"));
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

// ============================================= Concurrency test class ======================================
class SHVThreadDone
{
private:
	SHVDataEngineTest& tester;
public:
	SHVThreadDone(SHVDataEngineTest& t): tester(t) {}
	~SHVThreadDone() { tester.ThreadDone(); }
};

class SHVTestConcurrency
{
public:
	SHVTestConcurrency(SHVDataEngine& dataEngine, SHVTestResult* result, SHVDataFactory* factory, SHVDataEngineTest& engineTester, const SHVString8C& alias);
	void StartTest();
	void RunThread();
private:
	SHVDataFactoryRef Factory;
	SHVDataEngineTest& EngineTester;
	SHVTestResult* Result;
	SHVString8 Alias;
	SHVDataSessionRef InputData;
	SHVThread<SHVTestConcurrency> Thread;
};

SHVTestConcurrency::SHVTestConcurrency(SHVDataEngine& dataEngine, SHVTestResult* result, SHVDataFactory* factory, SHVDataEngineTest& engineTester, const SHVString8C& alias): EngineTester(engineTester)
{
	Result = result;
	Factory = factory;
	Alias = alias;
	InputData = dataEngine.CreateSession();
}

void SHVTestConcurrency::StartTest()
{
	Thread.Start(this, &SHVTestConcurrency::RunThread, SHVThreadBase::PrioNormal, Alias.ToStrT());
}

void SHVTestConcurrency::RunThread()
{
SHVThreadDone Done(EngineTester);
SHVDataSessionRef Session = Factory->CreateSession();
SHVDataRowListCRef test;
SHVDataRowListRef testInsert;
SHVBool res = SHVBool::False;
bool errLogged = false;
int retry = 10;
int step  = 0;	
int bulkCount = 0;
bool more = true;
	res = Factory->RegisterAlias("test", Alias, true, Session);
	if (res)
	{
		step++;
		test = InputData->QueryTable("test", _T(""), 1);
		res = test->IsOk();
	}
	if (res)
	{
		step++;
		testInsert = Session->GetRows(Alias, _T(""), 1);
		if (!testInsert.IsNull())
			res = testInsert->IsOk();
		else
			res.SetError(SHVSQLiteWrapper::SQLite_ERROR);
	}
	if (!res)
	{
		Result->AddLog(_T("Error(%s step %i) (Initializing): %s"), Alias.ToStrT().GetSafeBuffer(), step, Session->GetErrorMessage().GetSafeBuffer());
	}
	else
	{
		res = SHVBool::False;
		while ((!res || more) && retry)
		{
			errLogged = false;
			if (!more)
			{
				test->Reset();
			}
			else
				more = true;
			res = Session->StartEdit();
			if (!res && !errLogged)
			{
				errLogged = true;
				Result->AddLog(_T("Error(%s) (StartEdit): %s"), Alias.ToStrT().GetSafeBuffer(), Session->GetErrorMessage().GetSafeBuffer());
			}
			while (res && (res = test->NextRow()) && bulkCount++ < 100)
			{
				res = testInsert->IsOk();
				if (res)
				{
				SHVDataRowRef newRow = testInsert->AddRow();
					newRow->SetCorr(test->GetCurrentRow());
					res = newRow->AcceptChanges();
				}
				if (!res && !errLogged)
				{
					Result->AddLog(_T("Error(%s) (Accept) : %s"), Alias.ToStrT().GetSafeBuffer(), Session->GetErrorMessage().GetSafeBuffer());
					errLogged = true;
				}
			}
			bulkCount = 0;
			if (res.GetError() == SHVSQLiteWrapper::SQLite_DONE)
			{
				res = SHVBool::True;
				more = false;
			}
			else
			if (res.GetError() == SHVSQLiteWrapper::SQLite_ROW)
			{
				res = SHVBool::True;
				more = true;
			}
			if (!res && !errLogged)
			{
				errLogged = true;
				Result->AddLog(_T("Error(%s) (NextRow) : %s"), Alias.ToStrT().GetSafeBuffer(), Session->GetErrorMessage().GetSafeBuffer());
			}
			if (res)
				res = Session->Commit();
			if (!res)
			{
				if (!errLogged)
					Result->AddLog(_T("Error(%s) (Commit %i) : %s"), Alias.ToStrT().GetSafeBuffer(), 11-retry, Session->GetErrorMessage().GetSafeBuffer());
				Session->Rollback();
			}
			if (!more)
				retry--;
			else
				retry = 10;
			if (!res)
				Result->AddLog(_T("Retry(%s) %i"), Alias.ToStrT().GetSafeBuffer(), 10-retry);
		}
	}
	if (!res)
		Result->AddLog(_T("Total failure %s"), Alias.ToStrT().GetSafeBuffer());
}

// ============================================== Test functions =============================================
SHVBool SHVDataEngineTest::AddPerson(SHVDataRowList* list, int pk_person, const SHVStringC& firstName, const SHVStringC& middleName, const SHVStringC& lastName)
{
SHVBool retVal;
int before;
SHVDataRowRef newRow = list->AddRow();
	newRow->SetInt(0, pk_person);
	newRow->SetString(1, firstName);
	newRow->SetString(2, middleName);
	newRow->SetString(3, lastName);
	before = newRow->GetRefCount();
	retVal = newRow->AcceptChanges();
	return retVal;
}

SHVBool SHVDataEngineTest::TestInsert(SHVTestResult* result)
{
// Lets put some data into the test table
SHVDataSessionRef Session = DataEngine->CreateSession();
SHVDataRowListRef RowList;
SHVDataRowListRef ChildRowList;
SHVDataRowListCRef QueryData;
SHVBool retVal = SHVBool::True;
SHVFile file;
SHVString data;
SHVDataRowRef newRow;
SHVTime time;
int i = 0;

	time.SetNow();
	RowList = Session->GetRowsIndexed("test", _T(""), 1);
	ChildRowList = Session->GetRowsIndexed("testchild", _T(""), 0);
	file.Open(_T("testwords.txt"), SHVFile::FlagOpen|SHVFile::FlagRead);
	data.SetBufferSize(file.GetSize() / sizeof(SHVWChar));
	file.Seek(sizeof(SHVWChar));
	file.ReadString(data);
	Session->StartEdit();
	for (size_t pos = 0; pos < data.GetLength();)
	{
		newRow = RowList->AddRow();
		newRow->SetInt("key", ++i);
		newRow->SetTime("sort", time);
		newRow->SetString("descript1", data.Tokenize(_T(","), pos));
		newRow->SetString("descript2", data.Tokenize(_T(","), pos));
		if (!newRow->AcceptChanges())
			Result->AddLog(_T("Error : %s"), Session->GetErrorMessage().GetSafeBuffer());
		newRow = ChildRowList->AddRow();
		newRow->SetInt("key", i);
		newRow->SetString("descript3", data.Tokenize(_T(","), pos));
		newRow->AcceptChanges();
	}
	Session->Commit();
	Result->AddLog(_T("Rows added %i - %i"), RowList->GetRowCount(), i);
	return retVal;
}

SHVBool SHVDataEngineTest::TestUpdate(SHVTestResult* result)
{
SHVBool retVal = SHVBool::True;
	return retVal;
}

SHVBool SHVDataEngineTest::TestQueryTable(SHVTestResult* result)
{
SHVBool retVal = SHVBool::True;
	return retVal;
}

SHVBool SHVDataEngineTest::TestQuery(SHVTestResult* result)
{
SHVBool retVal = SHVBool::True;
	return retVal;
}

SHVBool SHVDataEngineTest::TestSpeed(SHVTestResult* result)
{
SHVBool retVal = SHVBool::True;	
SHVDataFactoryRef DB = DataEngine->CreateFactory(_T("test3.db"), &DataEngine->GetDataSchema());
SHVDataSessionRef Session = DB->CreateSession();
SHVDataSessionRef SourceSession = DataEngine->CreateSession();
SHVDataRowListCRef Source;
SHVDataRowListRef Dest;
int cnt = 0;
	retVal = DB->RegisterAlias("test", "test", true, Session);
	if (retVal)
	{
		Source = SourceSession->QueryTable("test", _T(""), 0);
		retVal = Source->IsOk();
	}
	if (retVal)
	{
		Dest = Session->GetRows("test", _T(""), 0);
		retVal = Dest->IsOk();
	}
	if (retVal)
	{
		retVal = Session->StartEdit();
	}
	while (retVal && (retVal == Source->NextRow()))
	{
	SHVDataRowRef newRow;
		newRow = Dest->AddRow();
		newRow->SetCorr(Source->GetCurrentRow());
		newRow->SetInt(0, ++cnt);
		retVal = newRow->AcceptChanges();
		
		if (retVal)
		{
			newRow = Dest->AddRow();
			newRow->SetCorr(Source->GetCurrentRow());
			newRow->SetInt(0, ++cnt);
			retVal = newRow->AcceptChanges();
		}

		if (retVal)
		{
			newRow = Dest->AddRow();
			newRow->SetCorr(Source->GetCurrentRow());
			newRow->SetInt(0, ++cnt);
			retVal = newRow->AcceptChanges();
		}

		if (retVal)
		{
			newRow = Dest->AddRow();
			newRow->SetCorr(Source->GetCurrentRow());
			newRow->SetInt(0, ++cnt);
			retVal = newRow->AcceptChanges();
		}

		if (retVal)
		{
			newRow = Dest->AddRow();
			newRow->SetCorr(Source->GetCurrentRow());
			newRow->SetInt(0, ++cnt);
			retVal = newRow->AcceptChanges();
		}

		if (retVal)
		{
			newRow = Dest->AddRow();
			newRow->SetCorr(Source->GetCurrentRow());
			newRow->SetInt(0, ++cnt);
			retVal = newRow->AcceptChanges();
		}

		if (retVal)
		{
			newRow = Dest->AddRow();
			newRow->SetCorr(Source->GetCurrentRow());
			newRow->SetInt(0, ++cnt);
			retVal = newRow->AcceptChanges();
		}

		if (retVal)
		{
			newRow = Dest->AddRow();
			newRow->SetCorr(Source->GetCurrentRow());
			newRow->SetInt(0, ++cnt);
			retVal = newRow->AcceptChanges();
		}
	}
	if (retVal == SHVSQLiteWrapper::SQLite_DONE)
	{
		retVal = Session->Commit();
	}
	Result->AddLog(_T("Rows inserted(%d)"), cnt);
	return retVal;
}

SHVBool SHVDataEngineTest::TestSpeed2(SHVTestResult* result)
{
SHVBool retVal = SHVBool::True;
	return retVal;
}

SHVBool SHVDataEngineTest::TestConcurrency(SHVTestResult* result)
{
SHVBool retVal = SHVBool::True;
SHVDataSessionRef Session;
SHVDataRowListCRef query;
SHVDataFactoryRef DB1 = DataEngine->GetDefaultFactory();
SHVDataFactoryRef DB2 = DataEngine->CreateFactory(_T("test2.db"), &DB1->GetDataSchema());
SHVVector<SHVTestConcurrency> Tests;
SHVString8 alias;

	WaitLock.Lock();	
	WaitRefCount = 0;
	for (int i = 0; i < 10; i++)
	{	
	SHVTestConcurrency* con;
		alias.Format("test%d", i+1);
		con = new SHVTestConcurrency(*DataEngine, Result, DB2, *this, alias);
		WaitRefCount++;
		Tests.Add(con);
		con->StartTest();
	}
	for (int i = 0; i < 10; i++)
	{	
	SHVTestConcurrency* con;
		alias.Format("test%d", i + 11);
		con = new SHVTestConcurrency(*DataEngine, Result, DB2, *this, alias);
		WaitRefCount++;
		Tests.Add(con);
		con->StartTest();
	}
	WaitLock.Lock();
	return SHVBool::True;
}

void SHVDataEngineTest::PerformTest(SHVTestResult* result)
{
SHVBool ok;
size_t time = GetTickCount();
size_t step = time;
size_t next;
	Result = result;
	result->AddLog(_T("Insert test"));
	
	ok = TestInsert(result);
	
	next = GetTickCount();
	result->AddLog(_T("TestInsert time running %lu"), next - step);
	step = next;
	
	if (ok)
		ok = TestConcurrency(result);
	
	next = GetTickCount();
	result->AddLog(_T("TestConcurrency time running %lu"), next - step);
	step = next;

	if (ok)
		ok = TestSpeed(result);

	next = GetTickCount();
	result->AddLog(_T("TestSpeed time running %lu"), next - step);
	step = next;

	/*
	if (ok)
		ok = TestQuery(result);
	if (ok)
		ok = TestSpeed2(result);
	*/
	result->AddLog(_T("Total time running %lu"), GetTickCount() - time);
	*result = ok;
}

void SHVDataEngineTest::ThreadDone()
{
	WaitRefCount--;
	if (!WaitRefCount)
		WaitLock.Unlock();
}

SHVBool SHVDataEngineTest::Register()
{
SHVBool retVal = SHVBool::True;
SHVDataStructRef dataStruct;
SHVDataRowKeyRef key;

	DataEngine = (SHVDataEngine*) Modules.ResolveModule("DataEngine");
	retVal = DataEngine != NULL;
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
		DataEngine->RegisterTable(dataStruct, false);
		DataEngine->RegisterAlias("test", "test", true);

		// Table testchild
		dataStruct = DataEngine->CreateStruct();
		dataStruct->SetTableName("testchild");
		dataStruct->Add("key", SHVDataVariant::TypeInt);
		dataStruct->Add("descript3", SHVDataVariant::TypeString, 20);
		key = dataStruct->CreateIndexKey();
		key->AddKey("key", false);
		dataStruct->AddIndex(key);
		DataEngine->RegisterTable(dataStruct, false);
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

		DataEngine->RegisterTable(dataStruct, true);
	}

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
				Result->AddLog(_T("Row adding"));
				break;
			case SHVDataRow::RowStateAdded:
				Result->AddLog(_T("Row added"));
				break;
			case SHVDataRow::RowStateDeleting:
				Result->AddLog(_T("Row deleting"));
				break;
			case SHVDataRow::RowStateDeleted:
				Result->AddLog(_T("Row deleted"));
				break;
			case SHVDataRow::RowStateChanging:
				Result->AddLog(_T("Row changing"));
				break;
			case SHVDataRow::RowStateChanged:
				Result->AddLog(_T("Row changed"));
				break;
		}
		if (row->GetRowState() != SHVDataRow::RowStateAdding && row->RowValid())
			DumpRow(Result, row);	
	}
}
