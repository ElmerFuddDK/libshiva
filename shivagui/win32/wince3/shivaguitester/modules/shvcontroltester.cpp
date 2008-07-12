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
#include "../../../../../include/platformspc.h"

#include "shvcontroltester.h"
#include "../../../../../include/gui/shvcontrollayout.h"
#include "../../../../../include/gui/utils/shvregion.h"
#include "../../../../../include/gui/utils/shvdraw.h"

#define __MENU_MSGBOXES  1
#define __MENU_STUFF     2
#define __MENU_DIALOGTST 3
#define __MENU_CLOSE     4


//=========================================================================================================
// SHVControlTester - Module for testing controls
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlTester::SHVControlTester(SHVModuleList& modules) : SHVModule(modules,"ControlTester")
{
}

/*************************************
 * Register
 *************************************/
SHVBool SHVControlTester::Register()
{
SHVFontRef font;

	if (!SHVModuleResolver<SHVGUIManager>(Modules,GUIManager,"GUIManager"))
		return false;

	font = GUIManager->GetFont(SHVGUIManager::CfgFontNormal)->CreateCopy(100);

	GUIManager->GetConfig().SetRef(SHVGUIManager::CfgFontNormal,font);
	GUIManager->GetMainWindow()->SetFont(font,false);

	return SHVModule::Register();
}

/*************************************
 * PostRegister
 *************************************/
void SHVControlTester::PostRegister()
{
SHVEventSubscriberBaseRef subsOnDrawLabel = new SHVEventSubscriberFunc<SHVControlTester>(this,&SHVControlTester::OnDrawLabel);
SHVEventSubscriberBaseRef subsOnDrawContainer = new SHVEventSubscriberFunc<SHVControlTester>(this,&SHVControlTester::OnDrawContainer);
SHVFontRef ownerDrawFont = GUIManager->GetFont(SHVGUIManager::CfgFontNormal)->CreateCopy(250);

	
	GUIManager->GetMainWindow()->SetTitle(_T("SHIVA GUI test application"));
	GUIManager->GetMainWindow()->SetMinimumSize(200,115);
	GUIManager->GetMainWindow()->SetLayoutEngine(new SHVControlLayoutCallback<SHVControlTester>(this,&SHVControlTester::OnResizeContainer));

	Bitmap = GUIManager->CreateBitmap(1);

	Container = GUIManager->NewContainerCustomDraw(subsOnDrawContainer)
		->SetColor(GUIManager->CreateColor(0xFF,0xFF,0xFF))
		->SetParent(GUIManager->GetMainWindow());

	Container->SetLayoutEngine(new SHVControlLayoutCallback<SHVControlTester>(this,&SHVControlTester::OnResizeControls));

	Label = GUIManager->NewLabel()->SetParent(Container)->SetText(_T("Label text"));
	LabelCustomDraw = GUIManager->NewLabelCustomDraw(subsOnDrawLabel)->SetParent(Container)->SetText(_T(""));
	EditBox = GUIManager->NewEdit(SHVControlEdit::SubTypeMultiLine)->SetParent(Container)->SetText(_T("Edit\ntext"))->SetLimit(50);
	Button = GUIManager->NewButton()->SetParent(Container)->SetText(_T("Click Me!"));


	LabelCustomDraw->SetFont(ownerDrawFont,true);
	EditBox->SetHeight(4);


	Button->SubscribeClicked(new SHVEventSubscriber(this,&Modules));

	GUIManager->GetMainWindow()->SetFlag(SHVControl::FlagVisible);
	GUIManager->GetMainWindow()->ResizeControls();

	SHVMenuRef menu = GUIManager->GetMainWindow()->CreateMenu(new SHVEventSubscriber(this,&Modules));
	SHVMenuRef menu2 = menu->AddSubMenu(_T("Test"));
	menu2->AddStringItem(__MENU_MSGBOXES,_T("Look at all those msgs"));
	menu2->AddSeparator();
	menu2->AddStringItem(__MENU_STUFF,_T("More stuff"));
	menu2->AddStringItem(__MENU_DIALOGTST,_T("Dialog test"));
	menu2 = menu->AddSubMenu(_T("Test2"));
	menu2->AddStringItem(__MENU_STUFF,_T("Even more stuff"));
	menu->Show();

	SHVModule::PostRegister();
}

/*************************************
 * Unregister
 *************************************/
void SHVControlTester::Unregister()
{
	Label = NULL;
	LabelCustomDraw = NULL;
	EditBox = NULL;
	Button = NULL;
	TestDialog = NULL;
	Container = NULL;
	Bitmap = NULL;

	SHVModule::Unregister();
}

void SHVControlTester::OnEvent(SHVEvent* event)
{
	if (event->GetCaller() == NULL) // GUI Events
	{
		if (event->GetObject() == Button && SHVEvent::Equals(event,SHVControlButton::EventClicked))
		{
		SHVMenuRef menu = Button->CreatePopupMenu(new SHVEventSubscriber(this,&Modules));
			menu->AddStringItem(__MENU_STUFF,_T("Stuff"));
			menu->AddSeparator();
			menu->AddStringItem(__MENU_CLOSE,_T("Close"));
			menu->Show(SHVMenu::PopupBelowWindow);
			menu = NULL;
		}
		else if (event->GetObject() == GUIManager->GetMainWindow())
		{
			if (SHVEvent::Equals(event,SHVControl::EventMenu))
			{
				switch (event->GetSubID())
				{
				case __MENU_MSGBOXES:
					GUIManager->ShowMessageBox(_T("Noget\nEller\nNoget\nAndet\nEller\nNoget\nTredje\nNoget"),_T("Knap"),SHVGUIManager::MsgBoxOKCancel);
//					GUIManager->ShowMessageBox(_T("Noget\nEller\nNoget\nAndet\nEller\nNoget\nTredje\nNoget"));
//					GUIManager->ShowMessageBox(_T("Noget\nEller\nNoget\nAndet\nEller\nNoget\nTredje\nNoget"),_T("Knap"));
//					GUIManager->ShowMessageBox(_T("Noget\nEller\nNoget\nAndet\nEller\nNoget\nTredje\nNoget"));
					break;
				case __MENU_STUFF:
					GUIManager->GetMainWindow()->SetColor(GUIManager->CreateColor(::rand()&0xFF,::rand()&0xFF,::rand()&0xFF));
					break;
				case __MENU_DIALOGTST:
/*					{
					SHVControlContainer* dialog;
						TestDialog = new SHVFormTabTest(GUIManager,dialog = GUIManager->NewDialog());
						dialog->Create();
						TestDialog->InitializeForm();
					}*/
					break;
				}
			}
		}
		else if (event->GetObject() == Button)
		{
			if (SHVEvent::Equals(event,SHVControl::EventMenu))
			{
				switch (event->GetSubID())
				{
				case __MENU_STUFF:
					GUIManager->GetMainWindow()->SetColor(GUIManager->CreateColor(::rand()&0xFF,::rand()&0xFF,::rand()&0xFF));
					break;
				case __MENU_CLOSE:
					Modules.CloseApp();
					break;
				}
			}
		}
	}
}

void SHVControlTester::OnResizeContainer(SHVControlContainer* container, SHVControlLayout* layout)
{
SHVRegionRef rgn = GUIManager->CreateRegion(container);

	rgn->ClipTop(15)->Move(Container)->FillPercent(15,0,100,100,SHVRect());
}

void SHVControlTester::OnResizeControls(SHVControlContainer* container, SHVControlLayout* layout)
{
SHVRegionRef rgn = GUIManager->CreateRegion(container);

	rgn->Move(Label)->Top()->Left(100)->ClipTop();
	rgn->Move(EditBox)->FillHorizontal(Label,NULL)->LeftOf(Label)->ClipTop();

	rgn->Move(Button)->Bottom()->AlignHorizontal(NULL,NULL,SHVRegion::AlignHCenter)->ClipBottom(4);

	rgn->Move(LabelCustomDraw)->FillHorizontal()->Top()->ClipTop();
}

void SHVControlTester::OnDrawLabel(SHVEvent* event)
{
SHVDrawRef draw = SHVDraw::FromDrawEvent(event);
SHVControlRef control = (SHVControl*)event->GetObject();
SHVRect rct = draw->GetClientRect(control);

	draw->DrawText(_T("These are not the droids you are looking for"),
		rct,
		NULL,
		SHVDraw::TextSingleLine|SHVDraw::TextHCenter|SHVDraw::TextVCenter|SHVDraw::TextEndEllipsis);
}

void SHVControlTester::OnDrawContainer(SHVEvent* event)
{
SHVDrawRef draw = SHVDraw::FromDrawEvent(event);
SHVRect rct = draw->GetClientRect((SHVControl*)event->GetObject());

	draw->DrawBitmap(Bitmap,SHVPoint(0,0));
	//draw->DrawRectFilled(rct,GUIManager->CreateColor(0xFF,0xFF,0xFF));
}
