/*
 *   Copyright (C) 2008 by Lars Eriksen
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of the
 *   License, or (at your option) any later version with the following
 *   exeptions:
 *
 *   1) Static linking to the library does not constitute derivative work
 *      and does not require the author to provide source code for the
 *      application.
 *      Compiling applications with the source code directly linked in is
 *      Considered static linking as well.
 *
 *   2) You do not have to provide a copy of the license with programs
 *      that are linked against this code.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "../../../../include/platformspc.h"
#include "../../../../include/shvversion.h"
#include "../../../../include/utils/shvmath.h"
#include "../../../../include/threadutils/shvthread.h"

#include "../../../../include/frameworkimpl/shvmainthreadeventqueue.h"
#include "../../../../include/utils/shvdll.h"

#include "../../../../include/gui/shvguimanager.h"
#include "../../../../include/gui/shvregisterbitmap.h"

#include "heyyou.xpm"

#include "../../../../include/gui/shvform.h"
#include "modules/shvtabtestform.h"
class SHVFormTest : public SHVForm<>
{
protected:

	SHVControlContainerRef Container;
	SHVControlEditRef TextBox;
	SHVControlButtonRef OK;
	SHVControlButtonRef Cancel;
	SHVInt Seed;

	SHVEventSubscriberRef ButtonSubscriber;

	enum Menus {
		MenuQuit = 1
	};
	
	SHVFormTest(SHVGUIManager* manager, SHVControlContainer* controlContainer) : SHVForm<>(manager,controlContainer,"FormTest")
	{
		ButtonSubscriber = new SHVEventSubscriber(this);
	}

	void OnEvent(SHVEvent* event)
	{
		if (event->GetObject() == Cancel)
			Container->SetFlag(SHVControl::FlagVisible,false);
		else if (event->GetObject() == OK)
			Container->SetColor(GetManager()->CreateColor(SHVMath::Rand(Seed)&0xFF,SHVMath::Rand(Seed)&0xFF,SHVMath::Rand(Seed)&0xFF));
	}
	
	void OnMenuEvent(SHVEvent* event)
	{
		if (event->GetSubID() == SHVInt(MenuQuit))
		{
			GetManager()->GetModuleList().CloseApp();
		}
	}
	
public:

	// Constructor
	static SHVFormTest* Create(SHVGUIManager* manager)
	{
	SHVControlContainer* dialog = manager->NewDialog();
		return (dialog ? new SHVFormTest(manager,dialog) : NULL);
	}

	// From SHVForm
	void InitializeForm()
	{
		SHVASSERT(!GetContainer()->IsCreated());
	
		GetContainer()->Create(0);
		GetContainer()->SetTitle(_S("Test dialog"));
	
		GetContainer()->SetSize(240,120);
		
		SHVMenuRef menu = GetContainer()->CreateMenu(new SHVEventSubscriberFunc<SHVFormTest>(this,&SHVFormTest::OnMenuEvent));
		SHVMenuRef menu2 = menu->AddSubMenu(_S("test"));
		menu2->AddStringItem(SHVInt(), _S("Test 1"));
		menu2->AddStringItem(SHVInt(), _S("Test 2"), SHVMenu::FlagDisabled);
		menu2->AddStringItem(SHVInt(), _S("Test 3"));
		SHVMenuRef menu3 = menu2->AddSubMenu(_S("Sub"));
		menu2->AddSeparator();
		menu2->AddStringItem(MenuQuit, _S("Quit"));
		menu3->AddStringItem(SHVInt(), _S("Test 4"));
		menu3->AddSeparator();
		menu3->AddStringItem(MenuQuit, _S("Quit"));
		menu->AddSeparator();
		menu->AddStringItem(MenuQuit, _S("Quit"));
		menu->Show();
		
		if (GetContainer()->IsCreated())
		{
		SHVRegionRef rgn = GetManager()->CreateRegion(GetContainer());
			
			Container = GetManager()->NewContainer(SHVControlContainer::SubTypeWindow);
			Container->SetParent(GetContainer());
			rgn->Move(Container)->FillPercent(0,0,100,100,SHVRect(10,10,10,10));
			rgn = GetManager()->CreateRegion(Container);
			
			TextBox = GetManager()->NewEdit(SHVControlEdit::SubTypeMultiLine);
			TextBox->SetParent(Container,SHVControl::FlagVisible|
												SHVControlEdit::FlagFlat|
												SHVControlEdit::FlagReadonly);
			TextBox->SetText(_S("TEXT\nTEXT\nTEXT\nTEXT\nTEXT\nTEXT\nTEXT\nTEXT\nTEXT\nTEXT\nTEXT"));
	
			OK = GetManager()->NewButton();
			OK->SetParent(Container);
			OK->SetText(_S("OK"))->SubscribeClicked(ButtonSubscriber);
	
			Cancel = GetManager()->NewButton();
			Cancel->SetParent(Container);
			Cancel->SetText(_S("Cancel"))->SubscribeClicked(ButtonSubscriber);
	
			rgn->SetMargin(4,3);
	
			// Set width of OK and Cancel
			rgn->Move(OK)->CtrlWidth(40)->And(Cancel)->CtrlWidth(40);
	
			// Move OK and Cancel
			rgn->Move(OK)->LeftOf(Cancel,0)->And(Cancel)
				->Bottom()->AlignHorizontal(NULL,NULL,SHVRegion::AlignHCenter)->ClipBottom();
	
			rgn->Move(TextBox)->FillPercent(0,0,100,100);
	
			Show();
		}
	}

	SHVBool PreClose()
	{
		return SHVBool::True;
	}
};
typedef SHVRefObjectContainer<SHVFormTest> SHVFormTestRef;


class SHVTest : public SHVModule
{
public:
	
	SHVGUIManager* GUIManager;
	SHVControlCheckboxRef Checkbox;
	SHVControlLabelRef Label;
	SHVControlButtonRef Button;
	SHVControlComboBoxRef ComboBox;
	SHVControlEditRef Edit;
	SHVControlListViewRef ListView;
	SHVFormTestRef NewWindow;
	SHVFormTabTestRef TabTest;
	int Counter;
	
	enum Menus {
		MenuTestTab = 1,
		MenuQuit
	};

	SHVTest(SHVModuleList& modules) : SHVModule(modules,"Test")
	{
		Counter = 0;
	}
	
	void OnResize(SHVControlContainer* container, SHVControlLayout* /*layout*/)
	{
		if (Label)
		{
		SHVRegionRef rgn = GUIManager->CreateRegion(container);
			if (++Counter > 10)
			{
				Label->SetFlag(SHVControl::FlagVisible);
				Label->SetText(SHVStringC::Format(_S("Label text : %d and counting"), Counter));
			}
			rgn->Move(Checkbox)->Bottom()->Left()->ClipBottom();
			rgn->Move(ComboBox)->CtrlWidth(20)->RightOf(Button)->And(Button)->Bottom(5)->AlignHorizontal(NULL,NULL,SHVRegion::AlignHCenter,5)->ClipBottom();
			rgn->Move(Label)
				->FillHorizontal(NULL,NULL,SHVRegion::AlignHCenter)
				->AlignVertical(NULL,NULL,SHVRegion::AlignBottom,10)
				->ClipBottom();
			rgn->Move(Edit)
				->FillPercent(0,0,50,100, SHVRect(2,2,2,2));
			rgn->Move(ListView)
				->FillPercent(50,0,50,100, SHVRect(2,2,2,2));
		}
	}

	void OnResizeDummy(SHVControlContainer* /*container*/, SHVControlLayout* /*layout*/)
	{
	}

	void OnPopupMenu(SHVEvent* event)
	{
		GUIManager->ShowMessageBox(_S("Noget\nEller\nNoget\nAndet\nEller\nNoget\nTredje\nNoget\nNoget\nNoget\nNoget\nNoget\nNoget\nNoget"),_S("Knap"));
		GUIManager->ShowMessageBox(_S("En heel masse text med maaaaange linjer hvis word wrap virker eller noget eller noget andet..."),_S("Knap"));
		NewWindow = SHVFormTest::Create(GUIManager);
		NewWindow->InitializeForm();
	}

	void OnClick(SHVEvent* event)
	{
		SHVMenuRef popup = GUIManager->CreatePopupMenu(new SHVEventSubscriberFunc<SHVTest>(this,&SHVTest::OnPopupMenu),Button);
		popup->AddStringItem(SHVInt(),_S("Test dialoger"));
		popup->Show(SHVMenu::PopupBelowWindow);
	}

	void OnCustomDraw(SHVEvent* event)
	{
	SHVDraw* draw = SHVDraw::FromDrawEvent(event);
	SHVRect client(draw->GetClientRect(Label));
		
		draw->DrawRect(client, GUIManager->CreateColor(0xFF,0x00,0xFF));
		draw->DrawXORRect(client);
		draw->DrawXORLine(SHVPoint(client.GetRight()-1, client.GetTop()+1),SHVPoint(client.GetLeft()+1, client.GetBottom()-1));
		//draw->DrawLine(SHVPoint(client.GetX()+1, client.GetY()+1),SHVPoint(client.GetRight()-1, client.GetBottom()-1),GUIManager->CreateColor(0x00,0xFF,0x00));
		draw->DrawLine(SHVPoint(client.GetX()+1, client.GetY()+1),SHVPoint(client.GetRight()-1, client.GetBottom()-1),GUIManager->CreatePen(GUIManager->CreateColor(0x00,0xFF,0x00),SHVPen::StyleSolid,3));
		//draw->DrawPolyline(GUIManager->CreateColor(0xFF,0,0), 2, SHVPoint::Val(client.GetX()+1, client.GetY()+1), SHVPoint::Val(client.GetRight()-1, client.GetTop()+1));
		draw->DrawPolygon(GUIManager->CreateColor(0xFF,0,0), 3, SHVPoint::Val(client.GetX()+1, client.GetY()+1), SHVPoint::Val(client.GetRight()-1, client.GetTop()+1), SHVPoint::Val(client.GetRight()-1, client.GetBottom()+1));
		client.Shrink(5,2,5,2);
		draw->DrawBitmapCentered(GUIManager->CreateBitmap(1),client,GUIManager->CreateColor(0xFF,0xFF,0xFF));
		draw->DrawBitmap(GUIManager->CreateBitmap(1),SHVPoint(client.GetX(),client.GetY()),13,10);
		draw->DrawText(GUIManager->GetFont(SHVGUIManager::CfgFontNormalBold),"TEEEEEST teeest\nDette er en test",client, GUIManager->CreateColor(0,0,0xFF), SHVDraw::TextHCenter|SHVDraw::TextVCenter|SHVDraw::TextMultiLine);
	}

	void OnMenuEvent(SHVEvent* event)
	{
		if (event->GetSubID() == SHVInt(MenuTestTab))
		{
		SHVControlContainer* dlg;
			TabTest = new SHVFormTabTest(GUIManager, dlg = GUIManager->NewDialog());
			dlg->Create(SHVControl::FlagVisible);
			TabTest->InitializeForm();
		}
		else if (event->GetSubID() == SHVInt(MenuQuit))
		{
			Modules.CloseApp();
		}
	}

	void OnComboBoxEvent(SHVEvent* event)
	{
		SHVUNUSED_PARAM(event);
		GUIManager->ShowMessageBox(SHVStringC::Format(_S("Selected item : %s"), ComboBox->GetText().GetSafeBuffer()));
	}

	SHVBool Register()
	{
		printf("In register\n");
		
		if (!SHVModuleResolver<SHVGUIManager>(Modules,GUIManager,"GUIManager"))
			return false;
	
		SHVASSERT(Modules.ResolveModule("Test"));
		SHVASSERT(Modules.ResolveModule("Timer"));
	
		Modules.EventSubscribe(SHVModuleList::EventClosing, new SHVEventSubscriber(this));
		
		GUIManager->GetMainWindow()->SetFont(GUIManager->GetFont(SHVGUIManager::CfgFontLargeBold)->CreateCopy(250),false);
	
		return SHVModule::Register();
	}
	
	void PostRegister()
	{
		GUIManager->GetMainWindow()->SetTitle(_S("SHIVA GUI test application"));
		
		GUIManager->GetMainWindow()->SetLayoutEngine(new SHVControlLayoutCallback<SHVTest>(this,&SHVTest::OnResize));
		
		Edit = GUIManager->NewEdit(SHVControlEdit::SubTypeMultiLine)->SetParent(GUIManager->GetMainWindow(),SHVControl::FlagVisible);
		Edit->SetFont(GUIManager->GetMainWindow()->GetFont(),true);
		ListView = GUIManager->NewListView()->SetParent(GUIManager->GetMainWindow(), SHVControl::FlagVisible);
		ListView->AddColumn(_S("Col 1"), 150);
		ListView->AddColumn(_S("Col 2"), 200);
		ListView->AddItem(_S("Item 1"))->SetItemText(_S("Col 2"),0,1);
		ListView->AddItem(_S("Item 2"))->SetItemText(_S("Col 2"),1,1);

		Button = GUIManager->NewButton()
			->SetParent(GUIManager->GetMainWindow())
			->SetText(_S("Click me"));
		Button->SubscribeClicked(new SHVEventSubscriberFunc<SHVTest>(this,&SHVTest::OnClick));

		ComboBox = GUIManager->NewComboBox(SHVControlComboBox::SubTypeList)->SetParent(GUIManager->GetMainWindow());
		ComboBox->AddItem(_S("1"))
				->AddItem(_S("2"))
				->AddItem(_S("3"))
				->AddItem(_S("4"))
				->SubscribeSelectedChanged(new SHVEventSubscriberFunc<SHVTest>(this,&SHVTest::OnComboBoxEvent));

		//Label = GUIManager->NewLabel()->SetParent(GUIManager->GetMainWindow(),0);
		Label = GUIManager->NewLabelCustomDraw(new SHVEventSubscriberFunc<SHVTest>(this,&SHVTest::OnCustomDraw))->SetParent(GUIManager->GetMainWindow(),0);
		Label->SetFont(GUIManager->GetMainWindow()->GetFont(),true);

		Checkbox = GUIManager->NewCheckbox()->SetParent(GUIManager->GetMainWindow())->SetText(_S("Check me out"))->SetChecked(SHVInt());
		
		SHVFontRef tstFont = GUIManager->CreateFont("Times",27,SHVFont::StyleItalic|SHVFont::StyleBold);
		if (!tstFont.IsNull())
		{
			printf("Cell height : %d\n", tstFont->GetHeight());
			Label->SetFont(tstFont,true);
		}
		Label->SetText(_S("Label text"));

		SHVMenuRef menu = GUIManager->GetMainWindow()->CreateMenu(new SHVEventSubscriberFunc<SHVTest>(this,&SHVTest::OnMenuEvent));
		SHVMenuRef menu2 = menu->AddSubMenu(_S("test"));
		menu2->AddStringItem(MenuTestTab, _S("Test 1 (tabs)"));
		menu2->AddStringItem(SHVInt(), _S("Test 2"), SHVMenu::FlagDisabled);
		menu2->AddStringItem(SHVInt(), _S("Test 3"));
		SHVMenuRef menu3 = menu2->AddSubMenu(_S("Sub"));
		menu2->AddSeparator();
		menu2->AddStringItem(MenuQuit, _S("Quit"));
		menu3->AddStringItem(SHVInt(), _S("Test 4"));
		menu3->AddSeparator();
		menu3->AddStringItem(MenuQuit, _S("Quit"));
		menu->AddSeparator();
		menu->AddStringItem(MenuQuit, _S("Quit"));
		menu->Show();
		
		GUIManager->GetMainWindow()->SetMinimumSize(120,100);
		GUIManager->GetMainWindow()->SetSize(300,100);
		GUIManager->GetMainWindow()->SetColor(GUIManager->CreateColor(0xFF,0xFF,0xFF));
		GUIManager->GetMainWindow()->ResizeControls();
		GUIManager->GetMainWindow()->SetFlag(SHVControl::FlagVisible);
	}

	void OnEvent(SHVEvent* event)
	{
		SHVUNUSED_PARAM(event);
		printf("Delaying shutdown by 2500 ms\n");
		Modules.EmitEvent(new SHVEventString(this,__EVENT_GLOBAL_DELAYSHUTDOWN,2500));
		Label->SetParent(NULL);
	}

	void Unregister()
	{
		printf("In unregister\n");
		SHVModule::Unregister();
		Label = NULL;
		Button = NULL;
		NewWindow = NULL;
		Edit = NULL;
		TabTest = NULL;
	}
};



int main(int argc, char *argv[])
{
SHVDll guilib;
int retVal = -1;

	SHVUNUSED_PARAM(argc);
	SHVUNUSED_PARAM(argv);

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR,__SHIVA_VERSION_MINOR,__SHIVA_VERSION_RELEASE))
	{
		::fprintf(stderr,_T("Invalid version of libshiva.dll\n"));
	}
	else if (!guilib.Load(guilib.CreateLibFileName(_T("shivaguigtk"))))
	{
		::fprintf(stderr,_T("Could not load GUI module library shivaguigtk\n"));
	}
	else
	{
	SHVMainThreadEventQueue mainqueue((SHVMainThreadEventDispatcher*)guilib.CreateObjectInt(NULL,SHVDll::ClassTypeMainThreadDispatcher));
	SHVRegisterBitmap(mainqueue.GetModuleList(),1,(char**)heyyou_xpm);

// 		mainqueue.GetModuleList().AddModule(new SHVControlTester(mainqueue.GetModuleList()));
 		mainqueue.GetModuleList().AddModule(new SHVTest(mainqueue.GetModuleList()));

		retVal = mainqueue.Run();
	}

	guilib.Unload();

	return retVal;
}
