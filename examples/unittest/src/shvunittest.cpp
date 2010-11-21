#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shiva/include/framework/shveventstdin.h"
#include "logger/shvtestloggerconsole.h"

#include "shvunittest.h"


//=========================================================================================================
// SHVUnitTest class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVUnitTest::SHVUnitTest(SHVModuleList& modules) : SHVModule(modules,"UnitTest")
{
	Performing = false;
}

/*************************************
 * Register
 *************************************/
SHVBool SHVUnitTest::Register()
{
	// resolve the GUI manager
	SHVModuleResolver<SHVGUIManager>(Modules,GUIManager,"GUIManager");

	Modules.EventSubscribe(__EVENT_GLOBAL_STDIN, new SHVEventSubscriber(this));

	TestResultSubs = new SHVEventSubscriberFunc<SHVUnitTest>(this,&SHVUnitTest::OnTestResult);
	TestResultShowSubs = new SHVEventSubscriberFunc<SHVUnitTest>(this,&SHVUnitTest::OnTestResultShow,&Modules);
	MenuSubs = new SHVEventSubscriberFunc<SHVUnitTest>(this,&SHVUnitTest::OnMenuEvent,&Modules);

	return SHVModule::Register();
}

/*************************************
 * PostRegister
 *************************************/
void SHVUnitTest::PostRegister()
{
	SHVModule::PostRegister();

	if (GUIManager)
	{
	SHVMenuRef menu;
	SHVMenuRef subMenuMain, subMenuGroup, subMenuItem;
	SHVListIterator<TestGroupPtr, TestGroup*> testItr(TestGroups);
		
		// Create form main and initialize it
		FormMain = new SHVFormMain(GUIManager,GUIManager->GetMainWindow());
		FormMain->InitializeForm();
		TestLogger = FormMain->CreateLogger();
		

		// Create the menu
		menu = FormMain->GetContainer()->CreateMenu(MenuSubs);
		
		subMenuMain = menu->AddSubMenu(_S("Program"));
		subMenuMain->AddStringItem(MenuClearLog, _S("Clear log"));
		subMenuMain->AddSeparator();
		subMenuMain->AddStringItem(MenuQuit, _S("Quit"));
	
		subMenuMain = menu->AddSubMenu(_S("Tests"));
		subMenuMain->AddStringItem(MenuPerformAll, _S("Perform all"));

		// Add sub test menu items
		while (testItr.MoveNext())
		{
			for (size_t i = 0; i<testItr.Get()->Tests.CalculateCount();i++)
			{
				if (subMenuGroup.IsNull())
				{
					subMenuGroup = subMenuMain->AddSubMenu(testItr.Get()->Group.ToStrT());
					subMenuGroup->AddStringItem((int)ActionMenuItems.CalculateCount(),
											SHVStringC::Format(_S("%s - all"), testItr.Get()->Group.ToStrT().GetSafeBuffer()));
					ActionMenuItems.Add(new ActionMenuItem(testItr.Get()));
				}
				subMenuItem = subMenuGroup->AddSubMenu(testItr.Get()->Tests[i]->GetTitle());
				
				subMenuItem->AddStringItem((int)ActionMenuItems.CalculateCount(),
										SHVStringC::Format(_S("%s - all"), testItr.Get()->Tests[i]->GetTitle().GetSafeBuffer()));
				ActionMenuItems.Add(new ActionMenuItem(SHVTestBase::FlagAll, testItr.Get()->Tests[i]));
				
				for (const SHVTestBase::Action* actions = testItr.Get()->Tests[i]->GetActions(); actions && actions->Flag; actions++)
				{
					subMenuItem->AddStringItem((int)ActionMenuItems.CalculateCount(),
											SHVStringC::Format(_S("%s - %s"), testItr.Get()->Tests[i]->GetTitle().GetSafeBuffer(), actions->Name));
					ActionMenuItems.Add(new ActionMenuItem(actions->Flag, testItr.Get()->Tests[i]));
				}
			}
			subMenuGroup = NULL;
		}
		menu->Show();
	}
	else
	{
		TestLogger = new SHVTestLoggerConsole();
	}
	

	// Imprint the strings for success and failure into the config system
	Modules.GetConfig().Set(_S("success.true"), TestLogger->Success(true));
	Modules.GetConfig().Set(_S("success.false"), TestLogger->Success(false));
	

	// Do we need to perform something at starup ? (Command line perform option)
	if (!Modules.GetConfig().Find(_S("perform"))->ToString().IsEmpty())
		PerformTestFromString(Modules.GetConfig().Find(_S("perform"))->ToString());
	
}

/*************************************
 * Unregister
 *************************************/
void SHVUnitTest::Unregister()
{
	SHVModule::Unregister();
	
	// Always cleanup resources from other modules here
	FormMain = NULL;
	TestLogger = NULL;
	TestGroups.RemoveAll();
}

/*************************************
 * OnEvent
 *************************************/
void SHVUnitTest::OnEvent(SHVEvent* event)
{
	if (SHVEventString::Equals(event,__EVENT_GLOBAL_STDIN))
	{
	SHVString8 str(SHVEventStdin::StdinFromEvent(event));
		
		if (str.Left(1) != SHVString8C("/"))
		{
			PerformTestFromString(str);
		}
		else if (str == SHVString8C("/quit"))
		{
			Modules.CloseApp();
		}
		else if (str == SHVString8C("/list"))
		{
		SHVListIterator<TestGroupPtr, TestGroup*> testItr(TestGroups);
			SHVConsole::Printf8("\nTests available:\n");
			
			SHVConsole::Printf8("\n  all - will perform all tests listed below\n");
			
			while (testItr.MoveNext())
			{
				SHVConsole::Printf8("\n%s.all:\n\n  will perform all tests for this group\n",
									testItr.Get()->Group.GetSafeBuffer());
				for (size_t i = 0; i<testItr.Get()->Tests.CalculateCount(); i++)
				{
					SHVConsole::Printf8("\n%s.%s:\n\n  %s - will perform all tests below\n",
										testItr.Get()->Group.GetSafeBuffer(),
										testItr.Get()->Tests[i]->GetTitle().ToStr8().GetSafeBuffer(),
										testItr.Get()->Tests[i]->GetID().GetSafeBuffer());
					for (const SHVTestBase::Action* actions = testItr.Get()->Tests[i]->GetActions(); actions && actions->Flag; actions++)
					{
						SHVConsole::Printf8("  %s.%s.%s - %s\n",
											testItr.Get()->Group.GetSafeBuffer(),
											testItr.Get()->Tests[i]->GetID().GetSafeBuffer(),
											actions->ActionID,
											SHVStringC(actions->Name).ToStr8().GetSafeBuffer());
					}
				}
			}
		}
		
		else if (str == SHVString8C("/help"))
		{
			SHVConsole::Printf8("In order to perform a test, write its ID\n"
								"\n"
								"Commands available:\n"
								" /list         Will list the available test IDs\n"
								" /quit         Quit program\n"
								"\n");
		}
		
	}
}

/*************************************
 * RegisterTest
 *************************************/
void SHVUnitTest::RegisterTest(SHVTestBase* test)
{
	if (Modules.IsRegistered())
	{
		SHVASSERT(false);
		test->ValidateRefCount();
	}
	else
	{
	TestGroup* group = NULL;
	SHVListPos pos;
	int comp;
		for (pos = TestGroups.GetHeadPosition(); pos && !group; TestGroups.MoveNext(pos))
		{
			comp = TestGroups.GetAt(pos)->Group.Compare(test->GetGroup());
			if (!comp)
				group = TestGroups.GetAt(pos);
			else if (comp < 0)
				break;
		}

		if (group)
			group->Tests.Add(test);
		else
			TestGroups.InsertBefore(pos,new TestGroup(test));
	}
}

/*************************************
 * IsPerforming
 *************************************/
bool SHVUnitTest::IsPerforming()
{
bool performing = Performing;
SHVListPos pos;
size_t i, count;

	for (pos = TestGroups.GetHeadPosition(); pos && !performing; TestGroups.MoveNext(pos))
	{
		count = TestGroups.GetAt(pos)->Tests.CalculateCount();
		for (i=0; i<count && !performing; i++)
			performing = TestGroups.GetAt(pos)->Tests[i]->IsPerforming();
	}

	return performing;
}

/*************************************
 * PerformTestFromString
 *************************************/
void SHVUnitTest::PerformTestFromString(const SHVString8C str)
{
int dot = str.Find(".");
SHVString8 groupID, testID, actionID;
int flag = 0;
bool allMode, allGroupMode;
SHVListIterator<TestGroupPtr, TestGroup*> testItr(TestGroups);

	Performing = true;

	if (dot > 0)
	{
		testID = str.Mid(dot+1);
		groupID = str.Left(dot);
		
		dot = testID.Find(".");
		if (dot > 0)
		{
			actionID = testID.Mid(dot+1);
			testID = testID.Left(dot);
		}
	}
	else
	{
		groupID = str;
	}
	
	allMode = groupID == "all";
	allGroupMode = testID == "all";

	while (testItr.MoveNext())	
	{
		if (testItr.Get()->Group == groupID || allMode)
		{
			for (size_t i = 0; i<testItr.Get()->Tests.CalculateCount() && (!flag || allMode || allGroupMode); i++)
			{
				if (allMode || allGroupMode || testItr.Get()->Tests[i]->GetID() == testID)
				{
					if (actionID.IsEmpty())
					{
						flag = SHVTestBase::FlagAll;
					}
					else
					{
						for (const SHVTestBase::Action* actions = testItr.Get()->Tests[i]->GetActions(); actions && actions->Flag && !flag; actions++)
						{
							if (actionID == actions->ActionID)
								flag = actions->Flag;
						}
					}
					
					if (flag)
						testItr.Get()->Tests[i]->Perform(Modules,flag,TestResultSubs);
				}
			}
		}
	}
	
	if (!flag)
		SHVConsole::ErrPrintf8("Invalid test : %s\n  For a list of tests, try /list\n", str.GetSafeBuffer());

	Performing = false;
	if (!IsPerforming() && flag)
		TestResultShowSubs->EmitNow(Modules,new SHVEvent(this,-1));
}

/*************************************
 * OnMenuEvent
 *************************************/
void SHVUnitTest::OnMenuEvent(SHVEvent* event)
{
	if (event->GetSubID() == SHVInt(MenuQuit))
	{
		Modules.CloseApp();
	}
	else if (event->GetSubID() == SHVInt(MenuClearLog))
	{
		FormMain->ClearLog();
	}
	else if (event->GetSubID() == SHVInt(MenuPerformAll))
	{
		PerformTestFromString("all");
	}
	else if (event->GetSubID() < SHVInt((int)ActionMenuItems.CalculateCount()))
	{
	ActionMenuItem* item = ActionMenuItems[event->GetSubID()];

		Performing = true;

		if (item->Group)
		{
			for (size_t i=0; i<item->Group->Tests.CalculateCount(); i++)
				item->Group->Tests[i]->Perform(Modules,item->Flag,TestResultSubs);
		}
		else if (item->Tester)
		{
			item->Tester->Perform(Modules,item->Flag,TestResultSubs);
		}

		Performing = false;
		if (!IsPerforming())
			TestResultShowSubs->EmitNow(Modules,new SHVEvent(this,-1));
	}
}

/*************************************
 * OnTestResult
 *************************************/
void SHVUnitTest::OnTestResult(SHVEvent* event)
{
SHVEventQueue* q = TestResultShowSubs->Emit(Modules,event);

	if (!IsPerforming())
		TestResultShowSubs->Emit(Modules,new SHVEvent(this,-1));

	if (q)
		q->SignalDispatcher();
}

/*************************************
 * OnTestResultShow
 *************************************/
void SHVUnitTest::OnTestResultShow(SHVEvent* event)
{
	if (event->GetType() == SHVEvent::TypeBase)
	{
		TestLogger->AddHeader(_S("\n--"));
		TestLogger->AddLine(_S("All done"));
	}
	else
	{
	SHVTestBase* test = SHVTestBase::FromTestEvent(event);
		test->DisplayResult(TestLogger);
	}
}
