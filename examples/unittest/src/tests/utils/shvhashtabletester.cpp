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

	ok = (ok && ( table[_T("string 2")] == _T("string data 2 - really")));
	ok = (ok && ( !table.Find(_T("string not found"))));

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	
	return ok;
}

/*************************************
 * helpers
 *************************************/
void SHVHashTableTester::InsertStrings(SHVHashTableString<SHVString,const SHVStringC>& table)
{
	table.Insert(_T("string 1"),_T("string data 1"));
	table.Insert(_T("string 2"),_T("string data 2"));
	table.Insert(_T("string 3"),_T("string data 3"));
	table.Insert(_T("string 4"),_T("string data 4"));

	table[_T("string 2")] = _T("string data 2 - really");
}
