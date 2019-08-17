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

#include "shvtabtestform.h"
#include "shvtabtestpageform.h"

//=========================================================================================================
// SHVFormTabTest
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVFormTabTest::SHVFormTabTest(SHVGUIManager* manager, SHVControlContainer* controlContainer)
	: SHVForm<>(manager,controlContainer,"FormTabTest")
{
}

/*************************************
 * InitializeForm
 *************************************/
void SHVFormTabTest::InitializeForm()
{
SHVRegionRef rgn = GetManager()->CreateRegion(GetContainer());
SHVMenuRef toplevelmenu, menu;

	SetStandardLayoutEngine();

	GetContainer()->SetTitle(_S("Tab test dialog"));

	GetContainer()->SetMinimumSize(230,185);

	tbTabs = GetManager()->NewTab();
	tbTabs->SetParent(GetContainer())->SubscribeSelectedPageChanged(new SHVEventSubscriber(this));

	toplevelmenu = GetContainer()->CreateMenu(new SHVEventSubscriber(this,&GetManager()->GetModuleList()));

	menu = toplevelmenu->AddSubMenu(_S("Dialog"));
	menu->AddStringItem(1,_S("Close"));
	menu->AddStringItem(2,_S("Add tab"));
	menu->AddStringItem(3,_S("Remove tab"));
	menu->AddSeparator();
	menu->AddStringItem(4,_S("Change color"));
	toplevelmenu->Show();

	GetContainer()->ResizeControls();

	Show();
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVFormTabTest::SetData(SHVDataBinder* data)
{
SHVBool retVal(SHVBool::False);

	return retVal;
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVFormTabTest::GetData(SHVDataBinder* data)
{
SHVBool retVal(SHVBool::False);

	return retVal;
}

/*************************************
 * OnEvent
 *************************************/
void SHVFormTabTest::OnEvent(SHVEvent* event)
{
	if (event->GetCaller() == NULL && event->GetObject() == GetContainer())
	{
		if (SHVEvent::Equals(event,SHVControl::EventMenu))
		{
			switch (event->GetSubID())
			{
			case 1:
				GetContainer()->Close();
				break;
			case 2:
				{
				SHVString str;
				SHVControlContainer* cnt;
				SHVFormBase* form;

					str.Format(_S("Tab %d"), tbTabs->GetPageCount()+1);
					cnt = tbTabs->AddPage(str);
					tbTabs->SetPageForm(tbTabs->GetPageCount()-1, form = new SHVFormTabTestPage(GetManager(),cnt,str));

					form->InitializeForm();
					tbTabs->SelectPage(tbTabs->GetPageCount()-1);
				}
				break;
			case 3:
				tbTabs->RemovePage(0);
				break;
			case 4:
				GetContainer()->SetColor(GetManager()->CreateColor(::rand()&0xFF,::rand()&0xFF,::rand()&0xFF));
				break;
			}
		}
	}
	else if (event->GetCaller() == NULL && event->GetObject() == tbTabs)
	{
		if (SHVEvent::Equals(event,SHVControlTab::EventSelectedPageChanged))
		{
			SHVTRACE(_S("Tab number %d selected\n"), (int)event->GetSubID());
		}
	}
}

/*************************************
 * OnResizeForm
 *************************************/
void SHVFormTabTest::OnResizeForm(SHVControlContainer* container, SHVControlLayout* layout)
{
SHVRegionRef rgn = GetManager()->CreateRegion(container);

	rgn->Move(tbTabs)->FillPercent(1,1,98,98,SHVRect(1,1,1,1));
}
