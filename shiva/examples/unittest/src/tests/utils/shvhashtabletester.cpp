#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvhashtabletester.h"


/*************************************
 * GetGroup
 *************************************/
const SHVString8C SHVHashTableTester::GetGroup() const
{
	return "utils";
}

/*************************************
 * GetID
 *************************************/
const SHVString8C SHVHashTableTester::GetID() const
{
	return "hashtable";
}

/*************************************
 * GetTitle
 *************************************/
const SHVStringC SHVHashTableTester::GetTitle() const
{
	return _S("SHVHashTable test");
}

/*************************************
 * GetActions
 *************************************/
const SHVTestBase::Action* SHVHashTableTester::GetActions() const
{
static const SHVTestBase::Action actions[] = {
	{ ActionStringInsert, "stringinsert", _S("String insert"), _S("This will test String Insert"), &SHVHashTableTester::TestStringInsert },
	{ ActionReleaseBuffer, "releasebuffer", _S("ReleaseBuffer"), _S("This will transferring contents between objects"), &SHVHashTableTester::TestReleaseBuffer },
	{ 0, NULL, NULL, NULL, NULL } }; // Termination
	
	return actions;
}

/*************************************
 * sub tests
 *************************************/
bool SHVHashTableTester::TestStringInsert(SHVModuleList& modules, SHVTestBase* self, int)
{
bool ok;
SHVHashTableString<SHVString,const SHVStringC> table;

	ok = true;
	InsertStrings(table);

	ok = (ok && ( table[_S("string 2")] == _S("string data 2 - really")));
	ok = (ok && ( !table.Find(_S("string not found"))));

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	
	return ok;
}
bool SHVHashTableTester::TestReleaseBuffer(SHVModuleList& modules, SHVTestBase* self, int)
{
bool ok;
SHVHashTableString<SHVString,const SHVStringC> table1;
SHVHashTableString<SHVString,const SHVStringC> table2;

	table2.Insert(_S("test"),_S("test2"));

	ok = true;
	InsertStrings(table1);

	table2 = table1.ReleaseBuffer();
	
	ok = (ok && table1.GetCount() == 0);
	ok = (ok && table1.GetSize() == table2.GetSize());
	ok = (ok && table2.GetCount() == 4);
	
	table1[_S("Testing")] = _S("Testing");
	
	if (ok)
	{
	SHVHashTableString<SHVString,const SHVStringC> table3(table2.ReleaseBuffer());
	
		table2 = table3.ReleaseBuffer();
	}
	
	ok = (ok && table2.GetCount() == 4);
	ok = (ok && table2[_S("string 1")] == _S("string data 1"));
	
	if (ok)
	{
	SHVHashTableString<SHVString,const SHVStringC> tableAdd;
		tableAdd.Insert(_S("add1"),_S("test"));
		table2 += tableAdd.ReleaseBuffer();
		table2 += tableAdd.ReleaseBuffer(); // Go crazy
		ok = (ok && table2.GetCount() == 5);
		tableAdd.Insert(_S("add2"),_S("test"));
		tableAdd.Insert(_S("add3"),_S("test"));
		tableAdd.Insert(_S("add4"),_S("test"));
		tableAdd.Insert(_S("add5"),_S("test"));
		tableAdd.Insert(_S("add6"),_S("test"));
		tableAdd.Insert(_S("add7"),_S("test"));
		tableAdd.Insert(_S("add8"),_S("test"));
		tableAdd.Insert(_S("add9"),_S("test"));
		tableAdd.Insert(_S("add10"),_S("test"));
		table2 += tableAdd.ReleaseBuffer();
		ok = (ok && table2.GetCount() == 14);
		ok = (ok && table2[_S("add10")] == _S("test"));
		table2 += table2.ReleaseBuffer();
		ok = (ok && table2[_S("add10")] == _S("test"));
	}
	
	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	
	return ok;
}

/*************************************
 * helpers
 *************************************/
void SHVHashTableTester::InsertStrings(SHVHashTableString<SHVString,const SHVStringC>& table)
{
	table.Insert(_S("string 1"),_S("string data 1"));
	table.Insert(_S("string 2"),_S("string data 2"));
	table.Insert(_S("string 3"),_S("string data 3"));
	table.Insert(_S("string 4"),_S("string data 4"));

	table[_S("string 2")] = _S("string data 2 - really");
}
