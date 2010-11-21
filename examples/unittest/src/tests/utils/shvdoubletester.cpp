#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvdoubletester.h"


/*************************************
 * GetGroup
 *************************************/
const SHVString8C SHVDoubleTester::GetGroup() const
{
	return "utils";
}

/*************************************
 * GetID
 *************************************/
const SHVString8C SHVDoubleTester::GetID() const
{
	return "double";
}

/*************************************
 * GetTitle
 *************************************/
const SHVStringC SHVDoubleTester::GetTitle() const
{
	return _S("SHVDouble test");
}

/*************************************
 * GetActions
 *************************************/
const SHVTestBase::Action* SHVDoubleTester::GetActions() const
{
static const SHVTestBase::Action actions[] = {
	{ ActionNormal, "normal", _S("Null test and return val"), _S("This will test SHVDouble value is correct when returned from functionsm, and the nullable feature"), &SHVDoubleTester::TestNormal },
	{ 0, NULL, NULL, NULL, NULL } }; // Termination
	
	return actions;
}

/*************************************
 * sub tests
 *************************************/
bool SHVDoubleTester::TestNormal(SHVModuleList& modules, SHVTestBase* self, int)
{
bool ok;
SHVDouble null, shvdouble1, shvdouble2;
double i;

	ok = true;
	ok = (ok && (null.IsNull()));
	null = 5;
	ok = (ok && (!null.IsNull()));
	null.SetToNull();
	ok = (ok && (null.IsNull()));
	ok = (ok && (ReturnValDouble(100) == 100));
	ok = (ok && (ReturnValDouble(100) == SHVInt(100)));
	ok = (ok && (ReturnValDouble(SHVDouble(100)) == SHVInt(100)));

	i = ReturnValSHVDouble(5) + 10 - ReturnValSHVDouble(15);
	ok = (ok && (i == 0));

	ok = (ok && (SHVDouble().IfNull(-1) == -1));

	shvdouble1 = shvdouble2 = SHVInt(5) + i;
	ok = (ok && (!shvdouble1.IsNull() && !shvdouble2.IsNull() && shvdouble1 == shvdouble2));

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	
	return ok;
}

/*************************************
 * helpers
 *************************************/
double SHVDoubleTester::ReturnValDouble(double i)
{
SHVDouble retVal = i;
	return retVal;
}
SHVDouble SHVDoubleTester::ReturnValSHVDouble(double i)
{
SHVDouble retVal = i;
	return retVal;
}
