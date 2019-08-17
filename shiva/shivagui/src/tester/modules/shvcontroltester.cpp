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

#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvcontroltester.h"
#include "shiva/include/gui/shvcontrollayout.h"
#include "shiva/include/gui/utils/shvregion.h"
#include "shiva/include/gui/utils/shvdraw.h"


//=========================================================================================================
// SHVControlTester - Module for testing controls
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlTester::SHVControlTester(SHVModuleList& modules) : SHVModule(modules,"ControlTester")
{
	Counter = 0;
}

/*************************************
 * Register
 *************************************/
SHVBool SHVControlTester::Register()
{
SHVFontRef font;
int scale;

	if (!SHVModuleResolver<SHVGUIManager>(Modules,GUIManager,"GUIManager"))
		return false;

	scale = Modules.GetConfig().FindInt(_S("scale"))->ToInt().IfNull(100);

	if (scale != 100)
	{
		font = GUIManager->GetFont(SHVGUIManager::CfgFontNormal)->CreateCopy(100);

		GUIManager->GetConfig().SetRef(SHVGUIManager::CfgFontNormal,font);
		GUIManager->GetMainWindow()->SetFont(font,false);
	}

	Modules.EventSubscribe(SHVModuleList::EventClosing, new SHVEventSubscriber(this));
	GUIManager->GetMainWindow()->SetFont(GUIManager->GetFont(SHVGUIManager::CfgFontLargeBold)->CreateCopy(250),false);
	
	return SHVModule::Register();
}

/*************************************
 * PostRegister
 *************************************/
void SHVControlTester::PostRegister()
{
	SHVModule::PostRegister();

	GUIManager->GetMainWindow()->SetTitle(_S("SHIVA GUI test application"));
	
	GUIManager->GetMainWindow()->SetLayoutEngine(new SHVControlLayoutCallback<SHVControlTester>(this,&SHVControlTester::OnResize));
	
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
	Button->SubscribeClicked(new SHVEventSubscriberFunc<SHVControlTester>(this,&SHVControlTester::OnClick));

	ComboBox = GUIManager->NewComboBox(SHVControlComboBox::SubTypeList)->SetParent(GUIManager->GetMainWindow());
	ComboBox->AddItem(_S("1"))
			->AddItem(_S("2"))
			->AddItem(_S("3"))
			->AddItem(_S("4"))
			->SubscribeSelectedChanged(new SHVEventSubscriberFunc<SHVControlTester>(this,&SHVControlTester::OnComboBoxEvent));

	//Label = GUIManager->NewLabel()->SetParent(GUIManager->GetMainWindow(),0);
	Label = GUIManager->NewLabelCustomDraw(new SHVEventSubscriberFunc<SHVControlTester>(this,&SHVControlTester::OnCustomDraw))->SetParent(GUIManager->GetMainWindow(),0);
	Label->SetFont(GUIManager->GetMainWindow()->GetFont(),true);

	Checkbox = GUIManager->NewCheckbox()->SetParent(GUIManager->GetMainWindow())->SetText(_S("Check me out"))->SetChecked(SHVInt());
	
	SHVFontRef tstFont = GUIManager->CreateFont("Times",27,SHVFont::StyleItalic|SHVFont::StyleBold);
	if (!tstFont.IsNull())
	{
		printf("Cell height : %d\n", tstFont->GetHeight());
		Label->SetFont(tstFont,true);
	}
	Label->SetText(_S("Label text"));

	SHVMenuRef menu = GUIManager->GetMainWindow()->CreateMenu(new SHVEventSubscriberFunc<SHVControlTester>(this,&SHVControlTester::OnMenuEvent));
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

/*************************************
 * Unregister
 *************************************/
void SHVControlTester::Unregister()
{
	SHVModule::Unregister();
	Label = NULL;
	Button = NULL;
	NewWindow = NULL;
	Edit = NULL;
	TabTest = NULL;
}

/*************************************
 * OnEvent
 *************************************/
void SHVControlTester::OnEvent(SHVEvent* event)
{
	SHVUNUSED_PARAM(event);
	printf("Delaying shutdown by 2500 ms\n");
	Modules.EmitEvent(new SHVEventString(this,__EVENT_GLOBAL_DELAYSHUTDOWN,2500));
	Label->SetParent(NULL);
}

/*************************************
 * OnResize
 *************************************/
void SHVControlTester::OnResize(SHVControlContainer* container, SHVControlLayout* /*layout*/)
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

/*************************************
 * OnResizeDummy
 *************************************/
void SHVControlTester::OnResizeDummy(SHVControlContainer* /*container*/, SHVControlLayout* /*layout*/)
{
}

/*************************************
 * OnPopupMenu
 *************************************/
void SHVControlTester::OnPopupMenu(SHVEvent* event)
{
	SHVUNUSED_PARAM(event);
	GUIManager->ShowMessageBox(_S("Something\nOr....\nSomething else\nOr maybe:\nSomething else entirely"),_S("Button"));
	GUIManager->ShowMessageBox(_S("Something\nOr....\nSomething else\nOr maybe:\nSomething else entirely"));
	GUIManager->ShowMessageBox(_S("Something\nOr....\nSomething else\nOr maybe:\nSomething else entirely"),_S("Click me"));
	GUIManager->ShowMessageBox(_S("Something\nOr....\nSomething else\nOr maybe:\nSomething else entirely"));
	NewWindow = SHVFormTest::Create(GUIManager);
	NewWindow->InitializeForm();
}

/*************************************
 * OnClick
 *************************************/
void SHVControlTester::OnClick(SHVEvent* event)
{
	SHVUNUSED_PARAM(event);
	SHVMenuRef popup = GUIManager->CreatePopupMenu(new SHVEventSubscriberFunc<SHVControlTester>(this,&SHVControlTester::OnPopupMenu),Button);
	popup->AddStringItem(SHVInt(),_S("Test dialogs"));
	popup->Show(SHVMenu::PopupBelowWindow);
}

/*************************************
 * OnCustomDraw
 *************************************/
void SHVControlTester::OnCustomDraw(SHVEvent* event)
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
	draw->DrawText(GUIManager->GetFont(SHVGUIManager::CfgFontNormalBold),"TEEEEEST teeest\nThis is a test",client, GUIManager->CreateColor(0,0,0xFF), SHVDraw::TextHCenter|SHVDraw::TextVCenter|SHVDraw::TextMultiLine);
}

/*************************************
 * OnMenuEvent
 *************************************/
void SHVControlTester::OnMenuEvent(SHVEvent* event)
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

/*************************************
 * OnComboBoxEvent
 *************************************/
void SHVControlTester::OnComboBoxEvent(SHVEvent* event)
{
	SHVUNUSED_PARAM(event);
	GUIManager->ShowMessageBox(SHVStringC::Format(_S("Selected item : %s"), ComboBox->GetText().GetSafeBuffer()));
}
