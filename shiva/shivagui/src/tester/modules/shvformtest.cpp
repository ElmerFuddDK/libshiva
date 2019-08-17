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

#include "shiva/include/utils/shvmath.h"

#include "shvformtest.h"

//=========================================================================================================
// SHVFormTest
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVFormTest::SHVFormTest(SHVGUIManager* manager, SHVControlContainer* controlContainer) : SHVForm<>(manager,controlContainer,"FormTest")
{
	ButtonSubscriber = new SHVEventSubscriber(this);
}

/*************************************
 * OnEvent
 *************************************/
void SHVFormTest::OnEvent(SHVEvent* event)
{
	if (event->GetObject() == Cancel)
		Container->SetFlag(SHVControl::FlagVisible,false);
	else if (event->GetObject() == OK)
		Container->SetColor(GetManager()->CreateColor(SHVMath::Rand(Seed)&0xFF,SHVMath::Rand(Seed)&0xFF,SHVMath::Rand(Seed)&0xFF));
}

/*************************************
 * OnMenuEvent
 *************************************/
void SHVFormTest::OnMenuEvent(SHVEvent* event)
{
	if (event->GetSubID() == SHVInt(MenuQuit))
	{
		GetManager()->GetModuleList().CloseApp();
	}
}
	
/*************************************
 * Create
 *************************************/
SHVFormTest* SHVFormTest::Create(SHVGUIManager* manager)
{
SHVControlContainer* dialog = manager->NewDialog();
	return (dialog ? new SHVFormTest(manager,dialog) : NULL);
}

/*************************************
 * InitializeForm
 *************************************/
void SHVFormTest::InitializeForm()
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

/*************************************
 * PreClose
 *************************************/
SHVBool SHVFormTest::PreClose()
{
	return SHVBool::True;
}
