#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvlisttester.h"


/*************************************
 * GetGroup
 *************************************/
const SHVString8C SHVListTester::GetGroup() const
{
	return "utils";
}

/*************************************
 * GetID
 *************************************/
const SHVString8C SHVListTester::GetID() const
{
	return "list";
}

/*************************************
 * GetTitle
 *************************************/
const SHVStringC SHVListTester::GetTitle() const
{
	return _S("SHVList test");
}

/*************************************
 * GetActions
 *************************************/
const SHVTestBase::Action* SHVListTester::GetActions() const
{
static const SHVTestBase::Action actions[] = {
	{ ActionAll, "all", _S("All tests"), _S("This will test SHVList"), &SHVListTester::TestAll },
	{ 0, NULL, NULL, NULL, NULL } }; // Termination
	
	return actions;
}

/*************************************
 * sub tests
 *************************************/
bool SHVListTester::TestAll(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok = true;
SHVList<SHVString,SHVStringC> list;
SHVListIterator<SHVString,SHVStringC> itr(list);
SHVListPos pos;
int i;

	list.AddHead(_S("R belong 2 US"));
	list.AddHead(_S("All your base"));
	list.AddTail(_S("1"));
	list.AddTail(_S("2"));
	list.AddTail(_S("3"));
	list.AddTail(_S("4"));
	
	ok = (ok && (list.GetCount() == 6));

	for(i=0, pos = list.GetTailPosition(); pos; list.MovePrev(pos),i++)
	{
		switch (i)
		{
		case 0:
			ok = (ok && (list.GetAt(pos) == _S("4")) );
			break;
		case 3:
			ok = (ok && (list.GetAt(pos) == _S("1")) );
			break;
		case 5:
			ok = (ok && (list.GetAt(pos) == _S("All your base")) );
			break;
		}
	}
	ok = (ok && (i == 6) );

	ok = (ok && (pos = list.Find(_S("2"))) );
	ok = (ok && (list.GetAt(pos = list.Find(_S("2"))) == _S("2")) );
	ok = (ok && (list.FindIndex(1)) );
	ok = (ok && (list.GetAt(pos = list.FindIndex(1)) == _S("R belong 2 US")) );
	ok = (ok && (!list.FindIndex(6)) );
	
	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	
	return ok;
}
