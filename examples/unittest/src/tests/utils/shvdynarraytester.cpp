#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvdynarraytester.h"

/*************************************
 * GetGroup
 *************************************/
const SHVString8C SHVDynArrayTester::GetGroup() const
{
	return "utils";
}

/*************************************
 * GetID
 *************************************/
const SHVString8C SHVDynArrayTester::GetID() const
{
	return "dynarray";
}

/*************************************
 * GetTitle
 *************************************/
const SHVStringC SHVDynArrayTester::GetTitle() const
{
	return _S("SHVDynArray test");
}

/*************************************
 * GetActions
 *************************************/
const SHVTestBase::Action* SHVDynArrayTester::GetActions() const
{
static const SHVTestBase::Action actions[] = {
	{ ActionInsert, "insert", _S("Insert into dyn array"), _S("This will test insertion into dyn array"), &SHVDynArrayTester::TestInsert },
	{ ActionSort, "sort", _S("Sorting of the array"), _S("This will test sorting"), &SHVDynArrayTester::TestSort },
	{ ActionRemove, "remove", _S("Removal of items in the array"), _S("Remove items from an array"), &SHVDynArrayTester::TestRemove },
	{ 0, NULL, NULL, NULL, NULL } }; // Termination
	
	return actions;
}

/*************************************
 * sub tests
 *************************************/
bool SHVDynArrayTester::TestInsert(SHVModuleList& modules, SHVTestBase* self, int)
{
SHVDynArray<SHVString,const SHVStringC> array;
bool ok;

	ok = true;

	InsertData(array);
	
	ok = (ok && (array.GetCount() == 10));
	ok = (ok && (array[5] && *array[5] == _S("0")));

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	return ok;
}
bool SHVDynArrayTester::TestSort(SHVModuleList& modules, SHVTestBase* self, int)
{
SHVDynArray<SHVString,const SHVStringC> array;
bool ok;
SHVStringC five(_S("5"));
SHVStringC zero(_S("0"));
SHVStringC nine(_S("9"));
SHVStringC seven(_S("7"));

	ok = true,

	InsertData(array);
	array.Sort(&SortFunc);

	for (size_t i=0; i<array.GetCount()-1 && ok; i++)
	{
		ok = (array[i] && array[i+1] &&  *array[i] <= *array[i+1]);
	}

	ok = (ok && (array.FindFirstMatchIndex(&five , &MatchFunc) == 5));
	ok = (ok && (array.FindFirstMatchIndex(&zero , &MatchFunc) == 0));
	ok = (ok && (array.FindFirstMatchIndex(&nine , &MatchFunc) == 9));
	ok = (ok && (array.FindFirstMatchIndex(&seven, &MatchFunc) == 7));

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	return ok;
}
bool SHVDynArrayTester::TestRemove(SHVModuleList& modules, SHVTestBase* self, int)
{
SHVDynArray<SHVString,const SHVStringC> array;
bool ok;

	ok = true,

	InsertData(array);
	array.Remove(4);
	array.Remove(4);

	ok = (ok && (array.GetCount() == 8));
	ok = (ok && (array[4] && *array[4] == _S("6")));
	ok = (ok && (array[7] && *array[7] == _S("5")));

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	return ok;
}

/*************************************
 * helpers
 *************************************/
void SHVDynArrayTester::InsertData(SHVDynArray<SHVString,const SHVStringC>& array)
{
	array.Add(new SHVString(_S("2")));
	array.Add(new SHVString(_S("9")));
	array.Add(new SHVString(_S("7")));
	array.Add(new SHVString(_S("3")));
	array.Add(new SHVString(_S("8")));
	array.Add(new SHVString(_S("0")));
	array.Add(new SHVString(_S("6")));
	array.Add(new SHVString(_S("4")));
	array.Add(new SHVString(_S("1")));
	array.Add(new SHVString(_S("5")));
}
int SHVDynArrayTester::SortFunc(SHVString** str1, SHVString** str2)
{
const SHVStringC* s1 = (const SHVStringC*)*str1;
SHVString* s2 = *str2;
	return PerformMatch(s1,s2);
}
int SHVDynArrayTester::MatchFunc(const SHVStringC** str1, SHVString** str2)
{
const SHVStringC* s1 = *str1;
SHVString* s2 = *str2;
	return PerformMatch(s1,s2);
}
int SHVDynArrayTester::PerformMatch(const SHVStringC* s1, SHVString* s2)
{
	if (!s1 && s2)
		return -1;
	if (s1 && !s2)
		return 1;
	if (!s1 && !s2)
		return 0;

	return s1->Compare(*s2);
}
