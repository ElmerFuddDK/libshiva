#include "stdafx.h"
#include "shiva/include/platformspc.h"
#include "shiva/include/utils/shvmd5sum.h"

#include "shvmd5sumtester.h"


/*************************************
 * GetGroup
 *************************************/
const SHVString8C SHVMd5SumTester::GetGroup() const
{
	return "utils";
}

/*************************************
 * GetID
 *************************************/
const SHVString8C SHVMd5SumTester::GetID() const
{
	return "md5";
}

/*************************************
 * GetTitle
 *************************************/
const SHVStringC SHVMd5SumTester::GetTitle() const
{
	return _S("SHVMd5Sum test");
}

/*************************************
 * GetActions
 *************************************/
const SHVTestBase::Action* SHVMd5SumTester::GetActions() const
{
static const SHVTestBase::Action actions[] = {
	{ ActionAll, "all", _S("All tests"), _S("This will test SHVMd5Sum"), &SHVMd5SumTester::TestAll },
	{ 0, NULL, NULL, NULL, NULL } }; // Termination
	
	return actions;
}

/*************************************
 * sub tests
 *************************************/
bool SHVMd5SumTester::TestAll(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok = true;
SHVMd5Sum md5;

	md5.Initialize();
	md5.CalculateString8("These are not the droids you are looking for");
	ok = ( ok && md5.GetMd5() == _S("46221fb52613b6b94daf5207dbf8443f") );

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	
	return ok;
}
