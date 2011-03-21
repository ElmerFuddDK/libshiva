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

#include "../../include/gui/shvcontrolcontainer.h"
#include "shvguimanagerimpl.h"
#include "../../include/framework/shveventstructs.h"

//=========================================================================================================
// SHVControlContainer - base control container class
//=========================================================================================================

/*************************************
 * Destructor
 *************************************/
SHVControlContainer::~SHVControlContainer()
{
	Clear();
	// if we are a top level window we need to close it
	if (GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeMainWindow || 
		GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeDialog || 
		GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeModalDialog )
	{
		Close();
	}
	GUIManager->ClearControlEvents(this);
	GetImplementor()->Destroy(this);
}

/*************************************
 * GetType
 *************************************/
int SHVControlContainer::GetType()
{
	return SHVControl::TypeContainer;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlContainer::Create(int flags)
{
SHVBool retVal;
SHVGUIManagerImpl* manager = (SHVGUIManagerImpl*)GetManager();

	SHVASSERT(GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeMainWindow || 
		GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeDialog || 
		GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeModalDialog );

	retVal = GetImplementor()->Create(this, NULL, flags);

	if (retVal)
	{
		// if we are in modal mode, set modal mode
		if (GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeDialog)
		{
			if (manager->TopLevelModalDialogs.GetCount())
				SetModalMode(true);

			manager->TopLevelDialogs.AddTail(this);
		}
		else if (GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeModalDialog)
		{
			if (!manager->TopLevelModalDialogs.GetCount())
			{
			SHVListIterator<SHVControlContainer*> itr(manager->TopLevelDialogs);

				manager->MainWindow->SetModalMode(true);

				while (itr.MoveNext())
					itr.Get()->SetModalMode(true);
			}

			manager->TopLevelModalDialogs.AddTail(this);
		}
	}

	return retVal;
}

/*************************************
 * Close
 *************************************/
SHVBool SHVControlContainer::Close()
{
SHVBool retVal;
SHVGUIManagerImpl* manager = (SHVGUIManagerImpl*)GetManager();

	SHVASSERT(GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeMainWindow || 
		GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeTabPage || 
		GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeDialog || 
		GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeModalDialog );

	retVal = ( IsCreated() ? GetImplementor()->Destroy(this) : SHVBool::True );

	if (!IsCreated())
	{
		// if we are in modal mode, set modal mode
		if (GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeDialog)
		{
		SHVListPos pos = manager->TopLevelDialogs.Find(this);

			ModalMode = ModalNone;

			if (pos)
				manager->TopLevelDialogs.RemoveAt(pos);
		}
		else if (GetImplementor()->GetSubType(this) == SHVControlContainer::SubTypeModalDialog)
		{
		SHVListPos pos = manager->TopLevelModalDialogs.Find(this);

			ModalMode = ModalNone;

			if (pos)
				manager->TopLevelModalDialogs.RemoveAt(pos);

			if (!manager->TopLevelModalDialogs.GetCount())
			{
			SHVListIterator<SHVControlContainer*> itr(manager->TopLevelDialogs);

				manager->MainWindow->SetModalMode(false);

				while (itr.MoveNext())
					itr.Get()->SetModalMode(false);
			}
		}
	}

	return retVal;
}

/*************************************
 * SetSize
 *************************************/
SHVControlContainer* SHVControlContainer::SetSize(int widthInFontUnits, int heightInFontUnits, PosModes mode)
{
SHVFontRef font = GetFont();
	GetImplementor()->SetSize(this, font->LFUToWidth(widthInFontUnits), font->LFUToHeight(heightInFontUnits), mode);
	return this;
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVControlContainer::GetData(SHVDataBinder* data)
{
	if (data)
	{
		return data->SetValue(GetTitle());
	}

	return SHVBool::False;
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVControlContainer::SetData(SHVDataBinder* data)
{
	if (data)
	{
		SetTitle(data->GetValue());
		return SHVBool::True;
	}

	return SHVBool::False;
}

/*************************************
 * SetMinimumSize
 *************************************/
SHVControlContainer* SHVControlContainer::SetMinimumSize(int widthInFontUnits, int heightInFontUnits)
{
SHVFontRef font = GetFont();
	GetImplementor()->SetMinimumSize(this,font->LFUToWidth(widthInFontUnits),font->LFUToHeight(heightInFontUnits));
	return this;
}

/*************************************
 * SetLayoutEngine
 *************************************/
void SHVControlContainer::SetLayoutEngine(SHVControlLayout* engine)
{
	LayoutEngine = engine;

	GetImplementor()->SetResizable(engine ? true : false);
}

/*************************************
 * GetLayoutEngine
 *************************************/
SHVControlLayout* SHVControlContainer::GetLayoutEngine()
{
	return LayoutEngine;
}

/*************************************
 * Clear
 *************************************/
void SHVControlContainer::Clear()
{
	for (size_t controls = CalculateControlCount(); controls;)
	{
		// Setting parent to NULL will destroy the internal control and remove it from us
		GetControl(--controls)->SetParent(NULL);
	}
}

/*************************************
 * ContainsControl
 *************************************/
SHVBool SHVControlContainer::ContainsControl(SHVControl* cntrl)
{
SHVBool retVal(SHVBool::False);

	for (size_t i=CalculateControlCount();i && !retVal;)
	{
		if (GetControl(--i) == cntrl)
			retVal = SHVBool::True;
	}

	return retVal;
}

/*************************************
 * PreDestroy
 *************************************/
SHVBool SHVControlContainer::PreDestroy()
{
	if (!PreDestroySubscriber.IsNull())
	{
		SHVEventDataBool allowPredestroy(SHVBool::True);
		PreDestroySubscriber->EmitNow(GetModuleList(),new SHVEventData<SHVEventDataBool>(allowPredestroy, NULL, EventPreDestroy));
		return allowPredestroy.GetValue();
	}
	else
		return SHVBool(SHVBool::True);
}

/*************************************
 * SubscribePreDestroy
 *************************************/
void SHVControlContainer::SubscribePreDestroy(SHVEventSubscriberBase* subs)
{
	PreDestroySubscriber = subs;
}

/*************************************
 * AddControl
 *************************************/
SHVBool SHVControlContainer::AddControl(SHVControl* cntrl)
{
	if (ContainsControl(cntrl))
		return SHVBool::True;

	Controls.Add(cntrl->CreateRef(),ControlGrowSize);
	return SHVBool::True;
}

/*************************************
 * RemoveControl
 *************************************/
SHVBool SHVControlContainer::RemoveControl(SHVControl* cntrl)
{
size_t count = Controls.CalculateCount();
size_t i;
SHVBool retVal(SHVBool::False);

	if (cntrl == NULL)
		return retVal;

	for (i=count;i>0 && !retVal;)
	{
		if (Controls[--i] == cntrl)
			retVal = SHVBool::True;
	}

	// if we found it, remove it from the vector
	if (retVal)
	{
		Controls.Replace(i,NULL);
		for (i++;i<count;i++)
		{
			Controls.Replace(i-1, Controls.Replace(i,NULL));
		}

		Controls.Truncate(ControlGrowSize);

		cntrl->DestroyRef();
	}

	return retVal;
}

/*************************************
 * SetModalMode
 *************************************/
///\cond INTERNAL
void SHVControlContainer::SetModalMode(bool enable)
{
	if (enable && ModalMode == ModalNone)
	{
	ModalModes newModalMode = ( GetFlag(SHVControl::FlagDisabled) ? ModalGlobalSelfDisabled : ModalGlobalDisabled );

		// only set disabled if enabled
		if (newModalMode == ModalGlobalDisabled)
		{
			SetFlag(SHVControl::FlagDisabled);
		}

		ModalMode = newModalMode;
	}
	else if (!enable && ModalMode != ModalNone)
	{
	ModalModes oldModalMode = ModalMode;

		ModalMode = ModalNone;

		// only remove disabled if enabled
		if (oldModalMode == ModalGlobalDisabled)
		{
			SetFlag(SHVControl::FlagDisabled,false);
		}
	}
}
///\endcond


//=========================================================================================================
// SHVControlImplementerContainer
//=========================================================================================================

/*************************************
 * UpdateSetFlag
 *************************************/
void SHVControlImplementerContainer::UpdateSetFlag(SHVControl* owner, int& flag, bool enable)
{
	// update the internal modal disable stuff
	if ( ((SHVControlContainer*)owner)->ModalMode )
	{
		if ( (flag&SHVControl::FlagDisabled) )
		{
			((SHVControlContainer*)owner)->ModalMode = ( enable ? SHVControlContainer::ModalGlobalSelfDisabled
																: SHVControlContainer::ModalGlobalDisabled );

			flag -= SHVControl::FlagDisabled;
		}
	}
}

/*************************************
 * GetDisabledFlag
 *************************************/
bool SHVControlImplementerContainer::GetDisabledFlag(SHVControl* owner, int flag)
{
	// returns false if the container is globally disabled and locally enabled and the check is for disabled
	if  ( (flag&SHVControl::FlagDisabled) &&
			   ((SHVControlContainer*)owner)->ModalMode == SHVControlContainer::ModalGlobalDisabled )
	{
		return false;
	}

	return true;
}

/*************************************
 * CreateMenu
 *************************************/
SHVMenu* SHVControlImplementerContainer::CreateMenu(SHVControlContainer* owner, SHVEventSubscriberBase* subscriber)
{
	if (subscriber)
		subscriber->ValidateRefCount();
	return NULL;
}
