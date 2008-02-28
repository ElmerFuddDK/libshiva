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

	if (msgBox)
	{
		msgBox->Title = title;
		msgBox->Text = text;
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
void SHVMessageBox::InitializeForm()
{

	SHVASSERT(!GetContainer()->IsCreated());

	GetContainer()->Create(0);
	GetContainer()->SetTitle(Title.IsNull() ? GetManager()->GetMainWindow()->GetTitle() : Title);

	GetContainer()->SetSize(240,120);

	if (GetContainer()->IsCreated())
	{
	SHVRegionRef rgn = GetManager()->CreateRegion(GetContainer());

		((SHVGUIManagerImpl*)GetManager())->MessageBoxes.AddTail(this);

		TextBox = GetManager()->NewEdit(SHVControlEdit::SubTypeMultiLine);
		OK = GetManager()->NewButton();
		Cancel = GetManager()->NewButton();

		OK->SetParent(GetContainer());
		Cancel->SetParent(GetContainer());
		TextBox->SetParent(GetContainer(),SHVControl::FlagVisible|
											SHVControlEdit::FlagFlat|
											SHVControlEdit::FlagReadonly);


		OK->SetText(_T("OK"))->SubscribeClicked(ButtonSubscriber);
		Cancel->SetText(_T("Cancel"))->SubscribeClicked(ButtonSubscriber);
		TextBox->SetText(Text);

		rgn->SetMargin(4,3);

		// Set width of OK and Cancel
		rgn->Move(OK)->CtrlWidth(40)->And(Cancel)->CtrlWidth(40);

		// Move OK and Cancel
		rgn->Move(OK)->RightOf(Cancel,0)->And(Cancel)
			->Bottom()->AlignHorizontal(NULL,NULL,SHVRegion::AlignHCenter)->ClipBottom();

		rgn->Move(TextBox)->FillPercent(0,0,100,100);

		Show();
	}
}

/*************************************
 * PreClose
 *************************************/
SHVBool SHVMessageBox::PreClose()
{
SHVGUIManagerImpl* manager = (SHVGUIManagerImpl*)GetManager();
SHVMessageBoxRef self = this;
	
	for (SHVListPos pos = manager->MessageBoxes.Find(this);pos;pos = manager->MessageBoxes.Find(this))
	{
		manager->MessageBoxes.RemoveAt(pos);
	}

	if (!ResultSubscriber.IsNull())
		ResultSubscriber->EmitNow(GetManager()->GetModuleList(),new SHVEvent(NULL,EventClicked,SHVInt(0),this));

	return SHVBool::True;
}

/*************************************
 * OnEvent
 *************************************/
void SHVMessageBox::OnEvent(SHVEvent* event)
{
	if (event->GetCaller() == NULL) // from a non-module
	{
	bool removeSelf = false;

		if (event->GetObject() == OK)
		{
			removeSelf = true;

			if (!ResultSubscriber.IsNull())
				ResultSubscriber->EmitNow(GetManager()->GetModuleList(),new SHVEvent(NULL,EventClicked,SHVInt(1),this));
			GetContainer()->Close();
		}
		else if (event->GetObject() == Cancel)
		{
			removeSelf = true;

			if (!ResultSubscriber.IsNull())
				ResultSubscriber->EmitNow(GetManager()->GetModuleList(),new SHVEvent(NULL,EventClicked,SHVInt(0),this));
			GetContainer()->Close();
		}

		if (removeSelf)
		{
		SHVGUIManagerImpl* manager = (SHVGUIManagerImpl*)GetManager();
		SHVMessageBoxRef self = this;

			for (SHVListPos pos = manager->MessageBoxes.Find(this);pos;pos = manager->MessageBoxes.Find(this))
			{
				manager->MessageBoxes.RemoveAt(pos);
			}
		}
	}
}
