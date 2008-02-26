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
#include "../../include/platformspc.h"

#include "shvmessagebox.h"
#include "shvguimanagerimpl.h"


//=========================================================================================================
// SHVGUIManagerImpl - Module for registering control types, and instantiating them
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVMessageBox::SHVMessageBox(SHVGUIManager* manager, SHVControlContainer* controlContainer) : SHVForm(manager,controlContainer,"MessageBox")
{
	ButtonSubscriber = new SHVEventSubscriber(this);
}

/*************************************
 * Create
 *************************************/
SHVMessageBox* SHVMessageBox::Create(SHVGUIManager* manager, const SHVStringC text, const SHVStringC title, int type)
{
SHVControlContainer* dialog = manager->NewModalDialog();
SHVMessageBox* msgBox = (dialog ? new SHVMessageBox(manager,dialog) : NULL);

	if (msgBox && dialog->Create(0))
	{
		dialog->SetTitle(title.IsNull() ? manager->GetMainWindow()->GetTitle() : title);
		msgBox->Text = text;

		dialog->SetSize(200,80);

		((SHVGUIManagerImpl*)manager)->MessageBoxes.AddTail(msgBox);
	}

	return msgBox;
}

/*************************************
 * ResultSubscribe
 *************************************/
void SHVMessageBox::ResultSubscribe(SHVEventSubscriberBase* resultSubscriber)
{
	ResultSubscriber = resultSubscriber;
}

/*************************************
 * InitializeForm
 *************************************/
void SHVMessageBox::InitializeForm(SHVControlLayout* layout)
{
SHVRegionRef rgn = GUIManager->CreateRegion(ControlContainer);

	TextBox = GUIManager->NewEdit(SHVControlEdit::SubTypeMultiLine);
	OK = GUIManager->NewButton();
	Cancel = GUIManager->NewButton();

	OK->SetParent(ControlContainer);
	Cancel->SetParent(ControlContainer);
//	TextBox->SetParent(ControlContainer,SHVControl::FlagVisible|
//										SHVControlEdit::FlagFlat|
//										SHVControlEdit::FlagReadonly);
	TextBox->SetParent(ControlContainer);
	TextBox->SetFlag(SHVControlEdit::FlagFlat);
	TextBox->SetFlag(SHVControlEdit::FlagReadonly);

	SHVASSERT(TextBox->GetFlag(SHVControlEdit::FlagFlat|SHVControlEdit::FlagReadonly));


	OK->SetText(_T("OK"))->SubscribeClicked(ButtonSubscriber);
	Cancel->SetText(_T("Cancel"))->SubscribeClicked(ButtonSubscriber);
	TextBox->SetText(Text);

	rgn->Move(Cancel)->Bottom()->AlignLeftRight(NULL,NULL,SHVRegion::AlignRight)->ClipBottom();
	rgn->Move(OK)->Bottom()->RightOf(Cancel)->ClipBottom();

	rgn->Move(TextBox)->SetPercent(0,0,100,100);

	Show();
}

/*************************************
 * PreClose
 *************************************/
SHVBool SHVMessageBox::PreClose()
{
SHVGUIManagerImpl* manager = (SHVGUIManagerImpl*)GUIManager;
	
	for (SHVListPos pos = manager->MessageBoxes.Find(this);pos;pos = manager->MessageBoxes.Find(this))
	{
		manager->MessageBoxes.RemoveAt(pos);
	}

	return SHVBool::True;
}

/*************************************
 * OnEvent
 *************************************/
void SHVMessageBox::OnEvent(SHVEvent* event)
{
	if (event->GetCaller() == NULL) // from a non-module
	{
		if (event->GetObject() == OK)
		{
			if (!ResultSubscriber.IsNull())
				ResultSubscriber->EmitNow(GUIManager->GetModuleList(),new SHVEvent(NULL,EventClicked,SHVInt(1),this));
			ControlContainer->Close();
		}
		else if (event->GetObject() == Cancel)
		{
			if (!ResultSubscriber.IsNull())
				ResultSubscriber->EmitNow(GUIManager->GetModuleList(),new SHVEvent(NULL,EventClicked,SHVInt(0),this));
			ControlContainer->Close();
		}
	}
}
