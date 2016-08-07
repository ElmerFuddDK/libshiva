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
	{ ActionReleaseBuffer, "releasebuffer", _S("ReleaseBuffer"), _S("This will transferring contents between objects"), &SHVDynArrayTester::TestReleaseBuffer },
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
bool SHVDynArrayTester::TestReleaseBuffer(SHVModuleList& modules, SHVTestBase* self, int)
{
bool ok = true;
SHVDynArray<SHVString, const SHVStringC> arr1;
SHVDynArray<SHVString, const SHVStringC> arr2;
SHVDynArrayRef< SHVRefObjectTemplate<int> > arrRef1;
SHVDynArrayRef< SHVRefObjectTemplate<int> > arrRef2;

	arr1.Add(new SHVString(_S("0")));
	arr1.Add(new SHVString(_S("1")));
	arr1.Add(new SHVString(_S("2")));
	arr1.Add(new SHVString(_S("3")));
	arr1.Add(new SHVString(_S("4")));
	arr1.Add(new SHVString(_S("5")));
	
	arr2.Add(new SHVString(_S("test")));
	
	ok = (ok && arr1.GetCount() == 6);
	
	arr2 = arr1.ReleaseBuffer();

	arr2.Add(new SHVString(_S("6")));
	arr2.Add(new SHVString(_S("7")));
	arr2.Add(new SHVString(_S("8")));
	arr2.Add(new SHVString(_S("9")));
	arr2.Add(new SHVString(_S("10")));
	
	ok = (ok && arr2.GetCount() == 11);
	
	if (ok)
	{
	SHVDynArray<SHVString, const SHVStringC> arr3(arr2.ReleaseBuffer());
		ok = (ok && arr2.GetCount() == 0);
		ok = (ok && arr3.GetCount() == 11);
		arr2 = arr3.ReleaseBuffer();
		arr3.Add(new SHVString(_S("test")));
		arr3.ReleaseBuffer();
	}
	
	if (ok)
	{
	SHVDynArray<SHVString, const SHVStringC> arrAdd;
		arrAdd.Add(new SHVString(_S("add1")));
		arr2 += arrAdd.ReleaseBuffer();
		arr2 += arrAdd.ReleaseBuffer(); // Go crazy
		ok = (ok && arr2.GetCount() == 12);
		arrAdd.Add(new SHVString(_S("add2")));
		arrAdd.Add(new SHVString(_S("add3")));
		arrAdd.Add(new SHVString(_S("add4")));
		arrAdd.Add(new SHVString(_S("add5")));
		arrAdd.Add(new SHVString(_S("add6")));
		arrAdd.Add(new SHVString(_S("add7")));
		arrAdd.Add(new SHVString(_S("add8")));
		arrAdd.Add(new SHVString(_S("add9")));
		arrAdd.Add(new SHVString(_S("add10")));
		arr2 += arrAdd.ReleaseBuffer();
		ok = (ok && arr2.GetCount() == 21);
		ok = (ok && *arr2[20] == _S("add10"));
		arr2 += arr2.ReleaseBuffer();
		ok = (ok && *arr2[20] == _S("add10"));
	}
	

	arrRef1.Add(new SHVRefObjectTemplate<int>(0));
	arrRef1.Add(new SHVRefObjectTemplate<int>(1));
	arrRef1.Add(new SHVRefObjectTemplate<int>(2));
	arrRef1.Add(new SHVRefObjectTemplate<int>(3));
	arrRef1.Add(new SHVRefObjectTemplate<int>(4));
	arrRef1.Add(new SHVRefObjectTemplate<int>(5));
	
	arrRef2.Add(new SHVRefObjectTemplate<int>(1234));
	
	ok = (ok && arrRef1.GetCount() == 6);
	
	arrRef2 = arrRef1.ReleaseBuffer();
	arrRef2 = arrRef2.ReleaseBuffer(); // This is a weird test
	if (ok)
	{
	SHVDynArrayRef< SHVRefObjectTemplate<int> > arrRef3(arrRef2.ReleaseBuffer());
		ok = (ok && arrRef2.GetCount() == 0);
		ok = (ok && arrRef3.GetCount() == 6);
		arrRef2 = arrRef3.ReleaseBuffer();
	}

	arrRef2.Add(new SHVRefObjectTemplate<int>(6));
	arrRef2.Add(new SHVRefObjectTemplate<int>(7));
	arrRef2.Add(new SHVRefObjectTemplate<int>(8));
	arrRef2.Add(new SHVRefObjectTemplate<int>(9));
	arrRef2.Add(new SHVRefObjectTemplate<int>(10));
	
	ok = (ok && arrRef2.GetCount() == 11);
	ok = (ok && arrRef2[6]->Object() == 6);
	
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
