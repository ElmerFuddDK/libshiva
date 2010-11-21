#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvbooltester.h"


/*************************************
 * GetGroup
 *************************************/
const SHVString8C SHVBoolTester::GetGroup() const
{
	return "utils";
}

/*************************************
 * GetID
 *************************************/
const SHVString8C SHVBoolTester::GetID() const
{
	return "bool";
}

/*************************************
 * GetTitle
 *************************************/
const SHVStringC SHVBoolTester::GetTitle() const
{
	return _S("SHVBool test");
}

/*************************************
 * GetActions
 *************************************/
const SHVTestBase::Action* SHVBoolTester::GetActions() const
{
static const SHVTestBase::Action actions[] = {
	{ ActionReturnVal, "returnval", _S("Return value from functions"), _S("This will test SHVBool value is correct when returned from functions"), &SHVBoolTester::TestReturnVal },
	{ ActionAssignment, "assignment", _S("Assignment"), _S("This will test SHVBool assignments"), &SHVBoolTester::TestAssignment },
	{ 0, NULL, NULL, NULL, NULL } }; // Termination
	
	return actions;
}

/*************************************
 * sub tests
 *************************************/
bool SHVBoolTester::TestReturnVal(SHVModuleList& modules, SHVTestBase* self, int)
{
bool ok;

	ok = true;
	ok = (ok && (!ReturnValInt(100) && ReturnValInt(100).GetError() == 100));
	ok = (ok && ( ReturnValInt(0)   && ReturnValInt(0)  .GetError() == SHVBool::True));
	ok = (ok && (!ReturnValInt(SHVBool::False) && ReturnValInt(SHVBool::False).GetError() == -1));
	ok = (ok && ( ReturnValBool(true)  && ReturnValBool(true) .GetError() == SHVBool::True ));
	ok = (ok && (!ReturnValBool(false) && ReturnValBool(false).GetError() == SHVBool::False));

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	
	return ok;
}
bool SHVBoolTester::TestAssignment(SHVModuleList& modules, SHVTestBase* self, int)
{
SHVBool a;
SHVBool b;
bool ok;
	
	ok = true;

	a = SHVBool::True;  b = true;  ok = (ok && (a == b &&  a &&  b));
	a = SHVBool::False; b = false; ok = (ok && (a == b && !a && !b));

	a.SetError(100); ok = (ok && (a == 100 && !a));

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	return ok;
}

/*************************************
 * helpers
 *************************************/
SHVBool SHVBoolTester::ReturnValInt(int err)
{
	return err;
}
SHVBool SHVBoolTester::ReturnValBool(bool b)
{
	return b;
}
