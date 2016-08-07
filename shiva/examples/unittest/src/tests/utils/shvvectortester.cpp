#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvvectortester.h"


/*************************************
 * GetGroup
 *************************************/
const SHVString8C SHVVectorTester::GetGroup() const
{
	return "utils";
}

/*************************************
 * GetID
 *************************************/
const SHVString8C SHVVectorTester::GetID() const
{
	return "vector";
}

/*************************************
 * GetTitle
 *************************************/
const SHVStringC SHVVectorTester::GetTitle() const
{
	return _S("SHVVector test");
}

/*************************************
 * GetActions
 *************************************/
const SHVTestBase::Action* SHVVectorTester::GetActions() const
{
static const SHVTestBase::Action actions[] = {
	{ ActionInsert, "insert", _S("Insert"), _S("This will test Insert"), &SHVVectorTester::TestInsert },
	{ ActionRemove, "remove", _S("Remove"), _S("This will test Remove"), &SHVVectorTester::TestRemove },
	{ ActionReleaseBuffer, "releasebuffer", _S("ReleaseBuffer"), _S("This will transferring contents between objects"), &SHVVectorTester::TestReleaseBuffer },
	{ 0, NULL, NULL, NULL, NULL } }; // Termination
	
	return actions;
}

/*************************************
 * sub tests
 *************************************/
bool SHVVectorTester::TestInsert(SHVModuleList& modules, SHVTestBase* self, int)
{
bool ok = true;
SHVVector<SHVString,5> vec1;
SHVVector<SHVString,1> vec2;
size_t i,j;

	vec1.Add(new SHVString(_S("0")));
	vec1.Add(new SHVString(_S("1")));
	vec1.Add(new SHVString(_S("2")));
	vec1.Add(new SHVString(_S("3")));
	vec1.Add(new SHVString(_S("4")));
	vec1.Add(new SHVString(_S("5")));
	vec1.Add(new SHVString(_S("6")));
	vec1.Add(new SHVString(_S("7")));
	vec1.Add(new SHVString(_S("8")));
	vec1.Add(new SHVString(_S("9")));
	vec1.Add(new SHVString(_S("10")));

	ok = (ok && (vec1.CalculateCount()==11));

	for (i=vec1.CalculateCount(); i>0;)
	{
		vec2.Add(vec1.Remove(--i).ReleaseReference());
	}

	ok = (ok && (vec1.CalculateCount()==11 && vec2.CalculateCount()==11));

	for (i=0,j=10; i<vec1.CalculateCount() && ok; i++,j--)
	{
		ok = (ok && (vec1[i] == NULL));
		ok = (ok && (vec2[i]->ToLong() == (int)j));
	}


	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	
	return ok;
}
bool SHVVectorTester::TestRemove(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok = true;
SHVVector<SHVString,5> vec1;
size_t i;

	vec1.Add(new SHVString(_S("0")));
	vec1.Add(new SHVString(_S("1")));
	vec1.Add(new SHVString(_S("2")));
	vec1.Add(new SHVString(_S("3")));
	vec1.Add(new SHVString(_S("4")));
	vec1.Add(new SHVString(_S("5")));
	vec1.Add(new SHVString(_S("6")));
	vec1.Add(new SHVString(_S("7")));
	vec1.Add(new SHVString(_S("8")));
	vec1.Add(new SHVString(_S("9")));
	vec1.Add(new SHVString(_S("10")));
	vec1.Add(new SHVString(_S("11")));

	vec1.Remove(2);
	vec1.Remove(4);
	vec1.Remove(11);
	vec1.Truncate();

	ok = (ok && (vec1.CalculateCount()==11));

	for (i=0; i<vec1.CalculateCount() && ok; i++)
	{
		if (i == 2 || i == 4)
			ok = (ok && (vec1[i] == NULL));
		else
			ok = (ok && (vec1[i]->ToLong() == (int)i));
	}

	vec1.Compress();

	ok = (ok && (vec1.CalculateCount()==9));

	for (i=0; i<vec1.CalculateCount() && ok; i++)
	{
		ok = (ok && (vec1[i] != NULL));
	}


	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	
	return ok;
}
bool SHVVectorTester::TestReleaseBuffer(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok = true;
SHVVector<SHVString,5> vec1;
SHVVector<SHVString,5> vec2;
SHVVectorRef<SHVRefObjectTemplate<int>,5> vecRef1;
SHVVectorRef<SHVRefObjectTemplate<int>,5> vecRef2;

	vec1.Add(new SHVString(_S("0")));
	vec1.Add(new SHVString(_S("1")));
	vec1.Add(new SHVString(_S("2")));
	vec1.Add(new SHVString(_S("3")));
	vec1.Add(new SHVString(_S("4")));
	vec1.Add(new SHVString(_S("5")));
	
	vec2.Add(new SHVString(_S("test")));
	
	ok = (ok && vec1.CalculateCount() == 6);
	
	vec2 = vec1.ReleaseBuffer();

	vec2.Add(new SHVString(_S("6")));
	vec2.Add(new SHVString(_S("7")));
	vec2.Add(new SHVString(_S("8")));
	vec2.Add(new SHVString(_S("9")));
	vec2.Add(new SHVString(_S("10")));
	
	ok = (ok && vec2.CalculateCount() == 11);
	
	if (ok)
	{
	SHVVector<SHVString,5> vecAdd;
		vecAdd.Add(new SHVString(_S("add1")));
		vec2 += vecAdd.ReleaseBuffer();
		vec2 += vecAdd.ReleaseBuffer(); // Go crazy
		ok = (ok && vec2.CalculateCount() == 12);
		vecAdd.Add(new SHVString(_S("add2")));
		vecAdd.Add(new SHVString(_S("add3")));
		vecAdd.Add(new SHVString(_S("add4")));
		vecAdd.Add(new SHVString(_S("add5")));
		vecAdd.Add(new SHVString(_S("add6")));
		vecAdd.Add(new SHVString(_S("add7")));
		vecAdd.Add(new SHVString(_S("add8")));
		vecAdd.Add(new SHVString(_S("add9")));
		vecAdd.Add(new SHVString(_S("add10")));
		vec2 += vecAdd.ReleaseBuffer();
		ok = (ok && vec2.CalculateCount() == 21);
		ok = (ok && *vec2[20] == _S("add10"));
		vec2 += vec2.ReleaseBuffer();
		ok = (ok && *vec2[20] == _S("add10"));
	}
	
	vecRef1.Add(new SHVRefObjectTemplate<int>(0));
	vecRef1.Add(new SHVRefObjectTemplate<int>(1));
	vecRef1.Add(new SHVRefObjectTemplate<int>(2));
	vecRef1.Add(new SHVRefObjectTemplate<int>(3));
	vecRef1.Add(new SHVRefObjectTemplate<int>(4));
	vecRef1.Add(new SHVRefObjectTemplate<int>(5));
	
	vecRef2.Add(new SHVRefObjectTemplate<int>(1234));
	
	ok = (ok && vecRef1.CalculateCount() == 6);
	
	vecRef2 = vecRef1.ReleaseBuffer();
	vecRef2 = vecRef2.ReleaseBuffer(); // This is a weird test
	if (ok)
	{
	SHVVectorRef<SHVRefObjectTemplate<int>,5> vecRef3(vecRef2.ReleaseBuffer());
		ok = (ok && vecRef2.CalculateCount() == 0);
		ok = (ok && vecRef3.CalculateCount() == 6);
		vecRef2 = vecRef3.ReleaseBuffer();
	}

	vecRef2.Add(new SHVRefObjectTemplate<int>(6));
	vecRef2.Add(new SHVRefObjectTemplate<int>(7));
	vecRef2.Add(new SHVRefObjectTemplate<int>(8));
	vecRef2.Add(new SHVRefObjectTemplate<int>(9));
	vecRef2.Add(new SHVRefObjectTemplate<int>(10));
	
	ok = (ok && vecRef2.CalculateCount() == 11);
	ok = (ok && vecRef2[6]->Object() == 6);
	
	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	
	return ok;
}
