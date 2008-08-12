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

#include "shvtabtestpageform.h"


//=========================================================================================================
// SHVFormTabTestPage
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVFormTabTestPage::SHVFormTabTestPage(SHVGUIManager* manager, SHVControlContainer* controlContainer, const SHVStringC labelStr)
	: SHVForm<>(manager,controlContainer,"FormTabTestPage")
{
	LabelStr = labelStr;
}

/*************************************
 * InitializeForm
 *************************************/
void SHVFormTabTestPage::InitializeForm()
{
SHVMenuRef toplevelmenu, menu;

	SetStandardLayoutEngine();

	Label = GetManager()->NewLabel();
	Label->SetParent(GetContainer())->SetText(LabelStr);

	EditBox = GetManager()->NewEdit();
	EditBox->SetParent(GetContainer());

	Button = GetManager()->NewButton();
	Button->SetParent(GetContainer())->SetText(_T("Click me"))->SubscribeClicked(new SHVEventSubscriber(this));

	lblLongDate = GetManager()->NewLabel();
	lblLongDate->SetParent(GetContainer())->SetText(_T("Long date:"));

	dtLongDate = GetManager()->NewDateTime(SHVControlDateTime::SubTypeLongDate);
	dtLongDate->SetParent(GetContainer());

	lblShortDate = GetManager()->NewLabel();
	lblShortDate->SetParent(GetContainer())->SetText(_T("Short date:"));

	dtShortDate = GetManager()->NewDateTime(SHVControlDateTime::SubTypeShortDate);
	dtShortDate->SetParent(GetContainer());

	lblTime = GetManager()->NewLabel();
	lblTime->SetParent(GetContainer())->SetText(_T("Time:"));

	dtTime = GetManager()->NewDateTime(SHVControlDateTime::SubTypeTime);
	dtTime->SetParent(GetContainer());

	lvData = GetManager()->NewListView();
	lvData->SetParent(GetContainer())->AddColumn(_T("Data"),50)->AddColumn(_T("Note"),100)->AddColumn(_T("Other"),50);
	lvData->AddItem(_T("Stuff"))->SetItemText(_T("Note"),0,1);
	lvData->AddItem(_T("Stuff1"))->SetItemText(_T("Note"),1,1);
	lvData->AddItem(_T("Stuff2"))->SetItemText(_T("Note"),2,1);
	lvData->AddItem(_T("Stuff3"))->SetItemText(_T("Note"),3,1);

	lvData->SubscribeSelectedChanged(new SHVEventSubscriber(this));

	GetContainer()->ResizeControls();

	Show();
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVFormTabTestPage::SetData(SHVDataBinder* data)
{
SHVBool retVal(SHVBool::False);

	return retVal;
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVFormTabTestPage::GetData(SHVDataBinder* data)
{
SHVBool retVal(SHVBool::False);

	return retVal;
}

/*************************************
 * OnEvent
 *************************************/
void SHVFormTabTestPage::OnEvent(SHVEvent* event)
{
	if (event->GetCaller() == NULL && event->GetObject() == Button)
	{
		if (SHVEvent::Equals(event,SHVControlButton::EventClicked))
		{
			// Set label to edit box contents
			Label->SetText(EditBox->GetText());

			// reset other labels width
			lblLongDate->SetText(lblLongDate->GetText());
			lblShortDate->SetText(lblShortDate->GetText());
			lblTime->SetText(lblTime->GetText());

			if (GetContainer()->GetTitle().Right(1) != _T("*"))
				GetContainer()->SetTitle(GetContainer()->GetTitle()+SHVStringC(_T("*")));
			GetContainer()->ResizeControls();

			dtLongDate->SetTime(dtShortDate->GetTime());

			if (!lvData->GetSelected().IsNull())
				lvData->SetItemText(EditBox->GetText(),lvData->GetSelected(),2);
		}
	}
	else if (event->GetCaller() == NULL && event->GetObject() == lvData)
	{
		if (!event->GetSubID().IsNull())
			EditBox->SetText(lvData->GetItemText(event->GetSubID(),2));
	}
}

/*************************************
 * OnResizeForm
 *************************************/
void SHVFormTabTestPage::OnResizeForm(SHVControlContainer* container, SHVControlLayout* layout)
{
SHVRegionRef rgn = GetManager()->CreateRegion(container);

	rgn->SetMargin(5,5);

	rgn->Move(Label)->And(lblLongDate)->And(lblShortDate)->And(lblTime)->UnifyWidth();

	rgn->Move(Label)->Top()->Left();
	rgn->Move(EditBox)->Top()->CtrlMaxWidth(200)->FillHorizontal(Label)->ClipTop();
	rgn->Move(lblLongDate)->Top()->Left();
	rgn->Move(dtLongDate)->Top()->RightOf(lblLongDate)->ClipTop();
	rgn->Move(lblShortDate)->Top()->Left();
	rgn->Move(dtShortDate)->Top()->RightOf(lblShortDate)->ClipTop();
	rgn->Move(lblTime)->Top()->Left();
	rgn->Move(dtTime)->Top()->RightOf(lblTime)->ClipTop();

	rgn->Move(Button)->Bottom()->Right()->ClipBottom();

	rgn->Move(lvData)->FillPercent(0,0,100,100);
}
