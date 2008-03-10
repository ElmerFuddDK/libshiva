#include "stdafx.h"
#include "../../../../include/platformspc.h"

#include "shvdataenginetest.h"
#include "../../include/shvdataengine.h"
#include "../../include/shvdatastruct.h"
#include "../../include/shvdatarowc.h"
#include "../../include/shvdatarow.h"
#include "../../include/shvdatarowlistc.h"
#include "../../include/shvdatarowlist.h"

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

	data = Test->GetRowsIndexed("test", _T(""), 0);
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
		if (!r->AcceptChanges()) 
			return SHVBool::False;
		now.AddSeconds(1);
		r = childdata->AddRow();
		r->SetInt(0, i);
		r->SetString(1, _T("Fætter"));
		if (!r->AcceptChanges()) 
			return SHVBool::False;
	}
	if(Test->Commit()) 
		result->AddLog(_T("End transaction"));
	else
		return SHVBool::False;
	return SHVBool::True;
}

SHVBool SHVDataEngineTest::TestUpdate(SHVTestResult* result)
{
SHVDataSessionRef Test = DataEngine->CreateSession();
SHVDataRowListRef data;
SHVDataRowRef editRow;
SHVTime now;
SHVDataRowKeyRef key;
	now.SetNow();
	if (Test->StartEdit())
	{
		data = Test->GetRowsIndexed("test", _T("key = 5"), 1);
		if (data->NextRow())
		{
			result->AddLog(_T("Found row key = 5"));
			editRow = data->EditCurrentRow();
			now.AddSeconds(-(60*60*24));
			editRow->SetTime("sort", now);
			now.AddSeconds(60*60*24);
			editRow->AcceptChanges();
		}
		data = Test->GetRowsIndexed("test", _T(""), 0);
		SHVDataVariant* v = DataEngine->CreateVariant();
		v->SetInt(5);
		key = DataEngine->CreateKey();
		key->AddKey("idx", v, false);
		if (data->Find(key))
		{
			result->AddLog(_T("Found row at index 4"));
			editRow = data->EditCurrentRow();
			editRow->Delete();
			editRow->AcceptChanges();
		}
		data = Test->GetRowsIndexed("test", _T(""), 0);
		data->EnableNonAccepted(true);
		for (int i = 0; i < 5; i++)
		{
			editRow = data->AddRow();
			editRow->SetInt(0, 15 + i);
			editRow->SetTime(1, now);
			editRow->SetString(2, _T("Tisse"));
			editRow->SetString(3, _T("Kat"));
			now.AddSeconds(1);
			editRow->AcceptChanges();
		}
		key->SetKeyValue(0, SHVInt(6));
		DumpRow(result, data->Find(key));
		if (Test->Commit())
		{
			result->AddLog(_T("Position after commit"));
			DumpRow(result, data->Find(key));
			return SHVBool::True;
		}
		else
		{
			result->AddLog(_T("Failed with %s"), Test->GetFactory()->GetErrorMessage().GetSafeBuffer());
			return SHVBool::False;
		}
	}
	else
		return SHVBool::False;
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

SHVBool SHVDataEngineTest::TestSpeed2(SHVTestResult* result)
{
SHVDataSessionRef Data = DataEngine->CreateSession();
SHVDataRowListRef rows;
SHVBool retVal;
SHVTime time;
DWORD start;
DWORD delta;

	time.SetNow();
	result->AddLog(_T("------------------------- TestSpeed2"));
	if (!Data->ExecuteNonQuery(_T("delete from speedtest")))
	{
		// Should not fail..
		return SHVBool::False;
	}
	retVal = Data->StartEdit();
	if (retVal)
	{
		rows = Data->GetRows("speedtest", _T(""), 0);
		for (int i = 0; i < 100000; i++)
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
				retVal = SHVBool::False;
				break;
			}
			time.AddSeconds(1);
		}
	}
	if (retVal)
	{
		Data->Commit();
		start = GetTickCount();
		rows = Data->GetRowsIndexed("speedtest", _T(""), 1);
		retVal = rows->IsOk();
		if (retVal)
		{
			delta = GetTickCount() - start;
			result->AddLog(_T("Indexed list with 100000 records took %lu"), delta);
		}
		start = GetTickCount();
		rows->Reset();
		while(rows->NextRow())
		{
		}
		delta = GetTickCount() - start;
		result->AddLog(_T("Iteration through list with 100000 records took %lu"), delta);
	}
	return retVal;
}

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

SHVBool SHVDataEngineTest::TestConcurrency(SHVTestResult* result)
{
SHVDataSessionRef SRW = DataEngine->CreateSession();
SHVDataSessionRef SR  = DataEngine->CreateSession();
SHVBool retVal = SHVBool::True;

SHVDataRowListRef RWList;
SHVDataRowListCRef RList;
SHVDataRowRef EmmaRow;
SHVDataRowKeyRef EmmaSearch;

    // Clear the persons table
	SRW->ExecuteNonQuery(_T("delete from person"));
	// Lets start by create 10 person (Realistic data this time)
	RWList = SRW->GetRowsIndexed("person", _T(""), 0);
	retVal = retVal && SRW->StartEdit();
	retVal = retVal && AddPerson(RWList, 1, _T("Mogens"), _T("Bak"), _T("Nielsen"));
	retVal = retVal && AddPerson(RWList, 2, _T("Laura"), _T("Birkemose"), _T("Nielsen"));
	retVal = retVal && AddPerson(RWList, 3, _T("Emma"), _T("Birkemose"), _T("Nielsen"));
	retVal = retVal && AddPerson(RWList, 4, _T("Hanne"), _T("Birkemose"), _T("Nielsen"));
	retVal = retVal && AddPerson(RWList, 5, _T("Lars"), _T(""), _T("Eriksen"));
	retVal = retVal && AddPerson(RWList, 6, _T("Peter"), _T(""), _T("Have"));
	retVal = retVal && AddPerson(RWList, 7, _T("Dirty"), _T("Bad"), _T("Harry"));
	retVal = retVal && AddPerson(RWList, 8, _T("Killer"), _T("Lame"), _T("Joe"));
	retVal = retVal && AddPerson(RWList, 9, _T("Lamer"), _T("L."), _T("Lamest"));
	retVal = retVal && AddPerson(RWList, 10, _T("Duke"), _T(""), _T("Man"));
	retVal = retVal && SRW->Commit();
	if (retVal)
	{
		// Lets make a select on middlename = 'Birkemose'
		RWList = SRW->GetRowsIndexed("person", _T("middleName = 'Birkemose'"), 1);
		retVal = RWList->IsOk();
		// Lets find Emma and changes her last name to 'Hansen'
		retVal = retVal && SRW->StartEdit();
		if (retVal)
		{
			EmmaSearch = DataEngine->CopyKey(RWList->GetStruct()->GetIndex(1));
			EmmaSearch->SetKeyValue(0, SHVString(_T("Emma")));
			EmmaSearch->SetKeyValue(1, SHVString(_T("Birkemose")));
			EmmaSearch->SetKeyValue(2, SHVString(_T("Nielsen")));
			if (RWList->Find(EmmaSearch))
			{
				EmmaRow = RWList->EditCurrentRow();
				// Now lets try the readonly list
				RList = SR->QueryTable("person", _T(""), 0);
				// Dump the data to see if it fails
				SR->StartEdit(); // This is evil i know ... but
				retVal = DumpData(result, RList);
				if (retVal)
				{
					result->AddLog(_T("No locks!!!!!!!!!!!!"));
					EmmaRow->SetString("lastName", _T("Hansen"));
					EmmaRow->AcceptChanges();
					// Dump the data again to see if it fails this time
					RList->Reset();
					retVal = DumpData(result, RList);
					if (retVal)						
					{
						retVal = SRW->Commit();
						if (!retVal)
							result->AddLog(DataEngine->GetDefaultFactory()->GetErrorMessage());
					}
					if (retVal)
						result->AddLog(_T("Commited ok"));
				}
				else
				{
					result->AddLog(DataEngine->GetDefaultFactory()->GetErrorMessage());
				}
			}
			else
				retVal = SHVBool::False;
		}
	}
	return retVal;
}


void SHVDataEngineTest::PerformTest(SHVTestResult* result)
{
SHVBool ok;
	Result = result;
	result->AddLog(_T("Insert test"));
	ok = TestConcurrency(result);
	/*
	ok = TestInsert(result); 
	if (ok)
		ok = TestUpdate(result);
	if (ok)
		ok = TestQuery(result);
	if (ok)
		ok = TestSpeed(result);
	if (ok)
		ok = TestSpeed2(result);
	*/
	*result = ok;
}

void SHVDataEngineTest::RunThread()
{
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
