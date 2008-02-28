#include "stdafx.h"
#include "../../../../include/platformspc.h"

#include "shvdataenginetest.h"
#include "../../include/shvdataengine.h"
#include "../../include/shvdatastruct.h"


void DumpRow(SHVTestResult* result, const SHVDataRowC* row)
{
	if (row)
	{
		if (row->GetStruct()->GetColumnCount() == 4)
			result->AddLog(_T("%d %s %20s %20s"), 
				(int) row->AsInt(0),
				row->AsString(1).GetSafeBuffer(),
				row->AsString(2).GetSafeBuffer(),
				row->AsString(3).GetSafeBuffer());
		else
			result->AddLog(_T("%d %s %20s %20s %20s"), 
				(int) row->AsInt(0),
				row->AsString(1).GetSafeBuffer(),
				row->AsString(2).GetSafeBuffer(),
				row->AsString(3).GetSafeBuffer(),
				row->AsString(4).GetSafeBuffer());
	}
	else
		result->AddLog(_T("Empty row"));
}

void DumpData(SHVTestResult* result, SHVDataRowListC* data)
{
	while (data->NextRow())
	{
		DumpRow(result, data->GetCurrentRow());
	}
}

SHVBool SHVDataEngineTest::TestInsert(SHVTestResult* result)
{
SHVDataSessionRef Test = DataEngine->CreateSession();
SHVDataRowListRef data;
SHVDataRowListRef childdata;
SHVDataRowRef r;
SHVTime now;
	result->AddLog(_T("------------------------- TestInsert"));
	// Clear the table
	result->AddLog(_T("Clear table"));
	Test->ExecuteNonQuery(_T("delete from test"));
	Test->ExecuteNonQuery(_T("delete from testchild"));

	if(Test->StartEdit()) 
		result->AddLog(_T("Started transaction"));
	else
		return SHVBool::False;

	data = Test->GetRows("test", _T(""), 0);
	if(data->IsOk()) 
		result->AddLog(_T("Initialized"));
	else
		return SHVBool::False;
	childdata = Test->GetRows("testchild", _T(""), 0);
	if(childdata->IsOk()) 
		result->AddLog(_T("Initialized child"));
	else
		return SHVBool::False;
	now.SetNow();
	for (int i = 0; i < 10; i++)
	{
		r = data->AddRow();
		r->SetInt(0, i);
		r->SetTime(1, now);
		r->SetString(2, _T("Nisse"));
		r->SetString(3, _T("Hat"));
		if (r->AcceptChanges()) 
			result->AddLog(_T("%d Accepted"), i);
		else
			return SHVBool::False;
		now.AddSeconds(1);
		r = childdata->AddRow();
		r->SetInt(0, i);
		r->SetString(1, _T("Fætter"));
		if (r->AcceptChanges()) 
			result->AddLog(_T("Child %d Accepted"), i);
		else
			return SHVBool::False;
	}
	if(Test->Commit()) 
		result->AddLog(_T("End transaction"));
	else
		return SHVBool::False;
	return SHVBool::True;
}

SHVBool SHVDataEngineTest::TestQueryTable(SHVTestResult* result)
{
SHVDataSessionRef Test = DataEngine->CreateSession();
SHVDataRowListCRef data;
SHVDataRowListCRef revdata;
SHVDataRowKeyRef key;
	result->AddLog(_T("------------------------- TestQueryTable"));
	data = Test->QueryTable("test", _T(""), 1);
	if (data->IsOk())
	{
		result->AddLog(_T("Find a search key"));
		for (int i = 0; i < 5; i++, data->NextRow());
		key = data->GetCurrentRow()->GetKey(1);
		for (size_t i = 0; i < key->Count(); i++)
		{
			result->AddLog(_T("%-10s = %s"), (*key)[i].Key.ToStrT().GetSafeBuffer(), (*key)[i].Value->AsString().GetSafeBuffer());
		}

		result->AddLog(_T("Forward"));
		data->Reset();
		DumpData(result, data);

		result->AddLog(_T("Find"));
		DumpRow(result, data->Find(key));
		DumpData(result, data);

		result->AddLog(_T("Now backward"));
		revdata = data->Reverse(_T(""));
		if (revdata->IsOk())
		{
			DumpData(result, revdata);
			return SHVBool::True;
		}
		else
			return SHVBool::False;
	}
	return SHVBool::False;
}

SHVBool SHVDataEngineTest::TestQuery(SHVTestResult* result)
{
SHVDataSessionRef Test = DataEngine->CreateSession();
SHVDataRowListCRef data;
SHVDataRowListCRef revdata;
SHVDataRowKeyRef key;
SHVString8 condition;
SHVString8 orderby;
SHVString select;

	result->AddLog(_T("------------------------- TestQuery"));
	DataEngine->BuildKeySQL(DataEngine->FindStruct("test")->GetIndex(1), condition, orderby, false);
	select.Format(_T("select test.*, descript3 from test join testchild on test.key = testchild.key where %s order by %s"),
		condition.ToStrT().GetSafeBuffer(), 
		orderby.ToStrT().GetSafeBuffer());
	data = Test->Query(select, DataEngine->FindStruct("test")->GetIndex(1));
	if (data->IsOk())
	{
		result->AddLog(_T("Find a search key"));
		for (int i = 0; i < 5; i++, data->NextRow());
		key = data->GetCurrentRow()->GetKey(0);
		for (size_t i = 0; i < key->Count(); i++)
		{
			result->AddLog(_T("%-10s = %s"), (*key)[i].Key.ToStrT().GetSafeBuffer(), (*key)[i].Value->AsString().GetSafeBuffer());
		}

		result->AddLog(_T("Forward"));
		data->Reset();
		DumpData(result, data);

		result->AddLog(_T("Find"));
		DumpRow(result, data->Find(key));
		DumpData(result, data);
		return SHVBool::True;
	}
	return SHVBool::False;
}

SHVBool SHVDataEngineTest::TestSpeed(SHVTestResult* result)
{
SHVDataSessionRef Test = DataEngine->CreateSession();
DWORD start;
DWORD delta;
SHVDataRowListRef rows;
SHVTime time;
SHVBool retVal = SHVBool::True;
SHVDataRowKeyRef key;
	
	time.SetNow();
	// lets start by clearing old data
	result->AddLog(_T("------------------------- TestSpeed"));
	if (!Test->ExecuteNonQuery(_T("delete from speedtest")))
	{
		// Should not fail..
		return SHVBool::False;
	}
	// Destructor will make a rollback, so all is ok
	Test->StartEdit();
	// Lets start be create a datarowlist with primary index
	rows = Test->GetRows("speedtest", _T(""), 0);

	// Lets insert 1000 rows
	start = GetTickCount();
	for (int i = 0; i < 1000; i++)
	{
	SHVDataRowRef r = rows->AddRow();
		r->SetInt(0, i);
		r->SetDouble(1, i * 3 / 2);
		r->SetString(2, _T("this is a test"));
		r->SetTime(3, time);
		r->SetBool(4, (i & 1) == 1);
		if (!r->AcceptChanges())
		{
			result->AddLog(_T("Could not add row"));
			retVal = SHVBool::True;
			break;
		}
		if (i == 500)
			key = r->GetKey(1);
		time.AddSeconds(1);
	}
	// All was ok lets commit
	if (retVal)
	{
		Test->Commit();
	// Now lets search using the secondary index
	SHVDataRowListCRef rows;
		delta = GetTickCount() - start;
		result->AddLog(_T("Insertion of 1000 rows took %lu milliseconds"), delta);
		start = GetTickCount();
		for (int i = 0; i < 1000; i++)
		{
			rows = Test->QueryTable("speedtest", _T(""), 1);
			rows->Find(key);
		}
		delta = GetTickCount() - start;
		result->AddLog(_T("Search 1000 times in 1000 rows took %lu milliseconds"), delta);
	}
	return retVal;
}

void SHVDataEngineTest::PerformTest(SHVTestResult* result)
{
SHVBool ok;
	result->AddLog(_T("Insert test"));
	ok = TestInsert(result); 
	if (ok)
		ok = TestQueryTable(result);
	if (ok)
		ok = TestQuery(result);
	if (ok)
		ok = TestSpeed(result);
	*result = ok;
}


SHVBool SHVDataEngineTest::Register()
{
SHVBool retVal = SHVBool::True;
	DataEngine = (SHVDataEngine*) Modules.ResolveModule("DataEngine");
	return SHVTestModule::Register() && retVal;
}

void SHVDataEngineTest::PostRegister()
{
SHVDataStructRef dataStruct = DataEngine->CreateStruct();
SHVDataRowKeyRef key;

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
	key->AddKey("sort", true);
	key->AddKey("descript1", false);
	dataStruct->AddIndex(key);
	DataEngine->RegisterTable(dataStruct);

	// Table testchild
	dataStruct = DataEngine->CreateStruct();
	dataStruct->SetTableName("testchild");
	dataStruct->Add("key", SHVDataVariant::TypeInt);
	dataStruct->Add("descript3", SHVDataVariant::TypeString, 20);
	key = dataStruct->CreateIndexKey();
	key->AddKey("key", false);
	dataStruct->AddIndex(key);
	DataEngine->RegisterTable(dataStruct);

	// Table speedtest
	dataStruct = DataEngine->CreateStruct();
	dataStruct->SetTableName("speedtest");
	dataStruct->Add("key", SHVDataVariant::TypeInt);
	dataStruct->Add("doublecol", SHVDataVariant::TypeDouble);
	dataStruct->Add("stringcol", SHVDataVariant::TypeString);
	dataStruct->Add("timecol", SHVDataVariant::TypeTime);
	dataStruct->Add("boolcol", SHVDataVariant::TypeBool);
	key = dataStruct->CreateIndexKey();
	key->AddKey("key", false);
	dataStruct->AddIndex(key);
	key = dataStruct->CreateIndexKey();
	key->AddKey("doublecol", false);
	key->AddKey("timecol", true);
	dataStruct->AddIndex(key);
	DataEngine->RegisterTable(dataStruct);
}

