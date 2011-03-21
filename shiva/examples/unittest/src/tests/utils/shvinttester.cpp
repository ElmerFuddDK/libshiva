#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvinttester.h"


/*************************************
 * GetGroup
 *************************************/
const SHVString8C SHVIntTester::GetGroup() const
{
	return "utils";
}

/*************************************
 * GetID
 *************************************/
const SHVString8C SHVIntTester::GetID() const
{
	return "int";
}

/*************************************
 * GetTitle
 *************************************/
const SHVStringC SHVIntTester::GetTitle() const
{
	return _S("SHVInt test");
}

/*************************************
 * GetActions
 *************************************/
const SHVTestBase::Action* SHVIntTester::GetActions() const
{
static const SHVTestBase::Action actions[] = {
	{ ActionAll, "all", _S("All tests"), _S("This will test SHVInt"), &SHVIntTester::TestAll },
	{ 0, NULL, NULL, NULL, NULL } }; // Termination
	
	return actions;
}

/*************************************
 * sub tests
 *************************************/
bool SHVIntTester::TestAll(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok;
SHVInt null, shvint1, shvint2;
int i;

	ok = true;
	ok = (ok && (null.IsNull()));
	null = 5;
	ok = (ok && (!null.IsNull()));
	null.SetToNull();
	ok = (ok && (null.IsNull()));
	ok = (ok && (ReturnValInt(100) == 100));
	ok = (ok && (ReturnValInt(100) == SHVInt(100)));
	ok = (ok && (ReturnValInt(SHVInt(100)) == SHVInt(100)));

	i = ReturnValSHVInt(5) + 10 - ReturnValSHVInt(15);
	ok = (ok && (i == 0));

	ok = (ok && (SHVInt().IfNull(-1) == -1));

	shvint1 = shvint2 = SHVInt(5) + i;
	ok = (ok && (!shvint1.IsNull() && !shvint2.IsNull() && shvint1 == shvint2));

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	
	return ok;
}

/*************************************
 * helpers
 *************************************/
int SHVIntTester::ReturnValInt(int i)
{
SHVInt retVal = i;
	return retVal;
}
SHVInt SHVIntTester::ReturnValSHVInt(int i)
{
SHVInt retVal = i;
	return retVal;
}
