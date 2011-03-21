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
