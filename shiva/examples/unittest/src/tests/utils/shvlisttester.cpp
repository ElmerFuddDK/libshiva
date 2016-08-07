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
SHVListBuffer<SHVString,SHVStringC> ListTester_GetEntries()
{
SHVList<SHVString,SHVStringC> retVal;
	
	retVal.AddTail(_S("5"));
	retVal.AddTail(_S("6"));
	retVal.AddTail(_S("7"));
	retVal.AddTail(_S("8"));
	
	return retVal.ReleaseBuffer();
}
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
	
	if (ok)
	{
	SHVList<SHVString,SHVStringC> list1;
	SHVList<SHVString,SHVStringC> listTemp(list.ReleaseBuffer());
		list1.AddTail(_S("This is a test"));
		list1 = listTemp.ReleaseBuffer();
		
		ok = (ok && list1.GetCount() == 6);
		ok = (ok && listTemp.GetCount() == 0);
		ok = (ok && list.GetCount() == 0);
		
		ok = (ok && (list1.GetAt(pos = list1.Find(_S("2"))) == _S("2")) );
		ok = (ok && (list1.FindIndex(1)) );
		ok = (ok && (list1.GetAt(pos = list1.FindIndex(1)) == _S("R belong 2 US")) );
		
		list1 += ListTester_GetEntries();
		ok = (ok && list1.GetCount() == 10);
		ok = (ok && (list1.GetAt(pos = list1.Find(_S("2"))) == _S("2")) );
		ok = (ok && (list1.GetAt(pos = list1.Find(_S("5"))) == _S("5")) );
		if (ok)
		{
			list1 += list1.ReleaseBuffer();
			i=0;
			for(SHVListPos pos=NULL; list1.MovePrev(pos);i++);
			ok = (ok && i == 10);
		}
	}
	
	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	
	return ok;
}
