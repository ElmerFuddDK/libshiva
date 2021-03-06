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
#include "../../../include/platformspc.h"

#include "../../../include/framework/shveventdata.h"

#include "shvguimanagerwin32.h"
#include "shvwin32.h"
#include "utils/shvdrawwin32.h"
#include "utils/shvbitmapwin32.h"
#include "shvmenuwin32.h"
#include "../../../include/gui/shvregisterbitmap.h"

#include "shvcontrolimplementercontainerwindowwin32.h"
#include "shvcontrolimplementerdialogwin32.h"
#include "shvcontrolimplementerlabelwin32.h"
#include "shvcontrolimplementereditwin32.h"
#include "shvcontrolimplementerbuttonwin32.h"
#include "shvcontrolimplementertabwin32.h"
#include "shvcontrolimplementerprogressbarwin32.h"
#include "shvcontrolimplementerdatetimewin32.h"
#include "shvcontrolimplementercomboboxwin32.h"
#include "shvcontrolimplementerlistviewwin32.h"
#include "shvcontrolimplementercheckboxwin32.h"
#include "../../../include/gui/shvcontrolcreator.h"
#include "../../../include/gui/shvcontrolcreatorsubtype.h"
#include "../../utilsimpl/shvregionimpl.h"

#include "../../gui/shvformimpl.h"

#define __CMDOFFSET 100

#ifndef GWL_HINSTANCE
# define GWL_HINSTANCE -6
#endif


//=========================================================================================================
// SHVGUIManagerWin32 - Module for registering control types, and instantiating them
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVGUIManagerWin32::SHVGUIManagerWin32(SHVModuleList& modules) : SHVGUIManagerImpl(modules)
{
SHVFontWin32Ref font;
HINSTANCE hInstance = (HINSTANCE)GetConfig().FindPtr(CfgInstanceHandle)->ToPtr();

	// Register control classes
	SHVControlImplementerContainerWindowWin32::RegisterClass(this,hInstance);
	SHVControlImplementerDialogWin32::RegisterClass(this,hInstance);

	// Register controls
	RegisterFactory(SHVControl::TypeContainer,SHVControlContainer::SubTypeDefault,
		new SHVControlCreatorSubType<SHVControlContainer,SHVControlImplementerContainerWindowWin32,SHVControlContainer::SubTypeDefault>());
	RegisterFactory(SHVControl::TypeContainer,SHVControlContainer::SubTypeCustomDraw,
		new SHVControlCreatorSubType<SHVControlContainer,SHVControlImplementerContainerWindowWin32,SHVControlContainer::SubTypeCustomDraw>());

	RegisterFactory(SHVControl::TypeContainer,SHVControlContainer::SubTypeModalDialog,
		new SHVControlCreatorSubType<SHVControlContainer,SHVControlImplementerDialogWin32,SHVControlContainer::SubTypeModalDialog>());
	RegisterFactory(SHVControl::TypeContainer,SHVControlContainer::SubTypeDialog,
		new SHVControlCreatorSubType<SHVControlContainer,SHVControlImplementerDialogWin32,SHVControlContainer::SubTypeDialog>());

	RegisterFactory(SHVControl::TypeLabel,SHVControlLabel::SubTypeDefault,
		new SHVControlCreatorSubType<SHVControlLabel,SHVControlImplementerLabelWin32,SHVControlLabel::SubTypeDefault>());
	RegisterFactory(SHVControl::TypeLabel,SHVControlLabel::SubTypeCustomDraw,
		new SHVControlCreatorSubType<SHVControlLabel,SHVControlImplementerLabelWin32,SHVControlLabel::SubTypeCustomDraw>());

	RegisterFactory(SHVControl::TypeEdit,SHVControlEdit::SubTypeSingleLine,
		new SHVControlCreatorSubType<SHVControlEdit,SHVControlImplementerEditWin32,SHVControlEdit::SubTypeSingleLine>());
	RegisterFactory(SHVControl::TypeEdit,SHVControlEdit::SubTypeMultiLine,
		new SHVControlCreatorSubType<SHVControlEdit,SHVControlImplementerEditWin32,SHVControlEdit::SubTypeMultiLine>());

	RegisterFactory(SHVControl::TypeButton,SHVControlButton::SubTypeDefault,
		new SHVControlCreator<SHVControlButton,SHVControlImplementerButtonWin32>());

	RegisterFactory(SHVControl::TypeProgressBar,SHVControlTab::SubTypeDefault,
		new SHVControlCreator<SHVControlProgressBar,SHVControlImplementerProgressBarWin32>());

	RegisterFactory(SHVControl::TypeTab,SHVControlTab::SubTypeDefault,
		new SHVControlCreator<SHVControlTab,SHVControlImplementerTabWin32>());

	RegisterFactory(SHVControl::TypeDateTime,SHVControlDateTime::SubTypeLongDate,
		new SHVControlCreatorSubType<SHVControlDateTime,SHVControlImplementerDateTimeWin32,SHVControlDateTime::SubTypeLongDate>());
	RegisterFactory(SHVControl::TypeDateTime,SHVControlDateTime::SubTypeShortDate,
		new SHVControlCreatorSubType<SHVControlDateTime,SHVControlImplementerDateTimeWin32,SHVControlDateTime::SubTypeShortDate>());
	RegisterFactory(SHVControl::TypeDateTime,SHVControlDateTime::SubTypeTime,
		new SHVControlCreatorSubType<SHVControlDateTime,SHVControlImplementerDateTimeWin32,SHVControlDateTime::SubTypeTime>());

	RegisterFactory(SHVControl::TypeComboBox,SHVControlComboBox::SubTypeEdit,
		new SHVControlCreatorSubType<SHVControlComboBox,SHVControlImplementerComboBoxWin32,SHVControlComboBox::SubTypeEdit>());
	RegisterFactory(SHVControl::TypeComboBox,SHVControlComboBox::SubTypeList,
		new SHVControlCreatorSubType<SHVControlComboBox,SHVControlImplementerComboBoxWin32,SHVControlComboBox::SubTypeList>());

	RegisterFactory(SHVControl::TypeListView,SHVControlListView::SubTypeNormal,
		new SHVControlCreatorSubType<SHVControlListView,SHVControlImplementerListViewWin32,SHVControlListView::SubTypeNormal>());
	RegisterFactory(SHVControl::TypeListView,SHVControlListView::SubTypeCustomDraw,
		new SHVControlCreatorSubType<SHVControlListView,SHVControlImplementerListViewWin32,SHVControlListView::SubTypeCustomDraw>());

	RegisterFactory(SHVControl::TypeCheckbox,SHVControlCheckbox::SubTypeNormal,
		new SHVControlCreatorSubType<SHVControlCheckbox,SHVControlImplementerCheckboxWin32,SHVControlCheckbox::SubTypeNormal>());
	RegisterFactory(SHVControl::TypeCheckbox,SHVControlCheckbox::SubTypeTristate,
		new SHVControlCreatorSubType<SHVControlCheckbox,SHVControlImplementerCheckboxWin32,SHVControlCheckbox::SubTypeTristate>());


	// Register fonts
	font = SHVFontWin32::CreateSystemFont();
	GetConfig().SetRef(CfgFontNormal,font);
	GetConfig().SetRef(CfgFontNormalBold,font->CreateCopy(100,SHVFont::StyleBold));
	GetConfig().SetRef(CfgFontLarge,font->CreateCopy(120));
	GetConfig().SetRef(CfgFontLargeBold,font->CreateCopy(120,SHVFont::StyleBold));


	// Set standard transparency color
	GetConfig().SetRef(CfgColorTransparent,CreateColor(0xFF,0x00,0xFF));

	WaitCursor = 0;
}

/*************************************
 * Register
 *************************************/
SHVBool SHVGUIManagerWin32::Register()
{
	Modules.EventSubscribe(__EVENT_GLOBAL_WAITCURSORSHOW,new SHVEventSubscriber(this,&Modules,true));
	Modules.EventSubscribe(__EVENT_GLOBAL_WAITCURSORHIDE,new SHVEventSubscriber(this,&Modules));

	return SHVGUIManagerImpl::Register();
}

/*************************************
 * Unregister
 *************************************/
void SHVGUIManagerWin32::Unregister()
{
	SHVGUIManagerImpl::Unregister();
	MenuMap.Clear();
}

/*************************************
 * CreateFont
 *************************************/
SHVFont* SHVGUIManagerWin32::CreateFont(const SHVStringC name, int height, int styles)
{
	return new SHVFontWin32(name,height,styles);
}

/*************************************
 * CreateColor
 *************************************/
SHVColor* SHVGUIManagerWin32::CreateColor(SHVColor::ColorVal r, SHVColor::ColorVal g, SHVColor::ColorVal b)
{
	return new SHVColorWin32(r,g,b);
}

/*************************************
 * CreatePen
 *************************************/
SHVPen* SHVGUIManagerWin32::CreatePen(SHVColor* color, int style, int width)
{
	return new SHVPenWin32((SHVColorWin32*)color,style,width);
}

/*************************************
 * CreateBrush
 *************************************/
SHVBrush* SHVGUIManagerWin32::CreateBrush(SHVColor* color, int style)
{
	return new SHVBrushWin32((SHVColorWin32*)color,style);
}

/*************************************
 * CreateRegion
 *************************************/
SHVRegion* SHVGUIManagerWin32::CreateRegion(SHVControlContainer* container)
{
	return new SHVRegionImpl(container);
}

/*************************************
 * CreateBitmap
 *************************************/
SHVBitmap* SHVGUIManagerWin32::CreateBitmap(SHVInt bitmapID)
{
SHVBitmap* retVal = NULL;
	if (Modules.GetConfig(SHVModuleList::CfgBitmapResourceMap).Contains(bitmapID))
	{
	SHVBitmapResourceMapWin32* map = (SHVBitmapResourceMapWin32*)Modules.GetConfig(SHVModuleList::CfgBitmapResourceMap).FindRef(bitmapID)->ToRef();
		retVal = new SHVBitmapWin32(map->hInstance,map->ResourceID);
	}

	return retVal;
}

/*************************************
 * CreatePopupMenu
 *************************************/
SHVMenu* SHVGUIManagerWin32::CreatePopupMenu(SHVEventSubscriberBase* subscriber, SHVControl* parent)
{
	return new SHVMenuWin32(this,SHVMenu::TypePopup,subscriber,parent);
}

/*************************************
 * ConstructFormImplementer
 *************************************/
SHVFormImplementer* SHVGUIManagerWin32::ConstructFormImplementer(SHVFormBase* owner, SHVGUIManager* manager, SHVControlContainer* controlContainer, SHVString8C entityName)
{
	return new SHVFormImpl(owner,manager,controlContainer,entityName);
}

/*************************************
 * ControlEventSubscribe
 *************************************/
SHVBool SHVGUIManagerWin32::ControlEventSubscribe(SHVInt controlEvent, SHVControl* control, SHVEventSubscriberBase* subscriber)
{
SHVBool retVal(SHVBool::True);
UINT message;

	switch (controlEvent)
	{
	case CtrlEventContainerDestroy:
		message = WM_DESTROY;
		retVal = ( control->GetType() == SHVControl::TypeContainer );
		break;
	case CtrlEventContainerRectChanged:
		message = 0;
		retVal = ( control->GetType() == SHVControl::TypeContainer );
		break;
	default:
		retVal = SHVBool::False;
		break;
	}

	if (retVal)
	{
	SHVListControlEvent* list = ControlEventMap.Find(control);

		if (!list)
		{
			ControlEventMap[control].AddTail(ControlEvent(subscriber,controlEvent,message));
		}
		else
		{
			list->AddTail(ControlEvent(subscriber,controlEvent,message));
		}
	}
	else if (subscriber)
	{
		subscriber->ValidateRefCount();
	}


	return retVal;
}

/*************************************
 * ClearControlEvents
 *************************************/
void SHVGUIManagerWin32::ClearControlEvents(SHVControl* control)
{
	ControlEventMap.Remove(control);
}

/*************************************
 * CreateDraw
 *************************************/
SHVDrawWin32* SHVGUIManagerWin32::CreateDraw(HDC dc)
{
	return new SHVDrawWin32(this,dc);
}

/*************************************
 * CreateDrawPaint
 *************************************/
SHVDrawPaintWin32* SHVGUIManagerWin32::CreateDrawPaint(HWND window)
{
	return new SHVDrawPaintWin32(this,window);
}

SHVColor* SHVGUIManagerWin32::CreateColor(COLORREF clr)
{
	return new SHVColorWin32(clr);
}

/*************************************
 * PreTranslateMessage
 *************************************/
bool SHVGUIManagerWin32::PreTranslateMessage(MSG* message)
{
bool retVal = false;
HWND hWnd = message->hwnd;

	if (message->message == WM_SHV_DISPATCHMESSAGES)
		return false;

	while (hWnd && !retVal)
	{
		retVal = ( ::SendMessage(hWnd,WM_SHV_PRETRANSLATEMESSAGE,(WPARAM)message,0) ? true : false );
		if (!retVal)
			hWnd = ::GetParent(hWnd);
	}

	if (!retVal)
	{
		switch (message->message)
		{
		case WM_COMMAND:
			// test if this command is from a menu
			if (!HIWORD(message->wParam))
			{
			int id = LOWORD(message->wParam);
			HMENU hmenu = (HMENU)CommandIDToHandle(id);
				if (hmenu && MenuMap.Find(hmenu))
				{
				SHVMenuRef menu = MenuMap[hmenu];
					((SHVMenuWin32*)(SHVMenu*)menu)->EmitEvent(CommandIDToValue(id));
					retVal = true;
				}
			}
			break;
		}
	}

	if (!retVal)
		EmitEvent(new SHVEventData<MSG*>(message,this,EventPreTranslateMessage));

	return retVal;
}

/*************************************
 * OnEvent
 *************************************/
void SHVGUIManagerWin32::OnEvent(SHVEvent* event)
{
	if (SHVEventString::Equals(event,__EVENT_GLOBAL_WAITCURSORSHOW))
	{
		if (WaitCursor++ == 0)
		{
		POINT point;
			if (::GetCursorPos(&point))
			{
			HWND wnd = ::WindowFromPoint(point);
				if (wnd && (HINSTANCE)GetWindowLongPtr(wnd,GWL_HINSTANCE) == Win32::GetInstance(GetMainWindow()))
				{
					::SendMessage(wnd,WM_SETCURSOR,(WPARAM)wnd,MAKEWORD(0,1));
				}
			}
		}
	}
	else if (SHVEventString::Equals(event,__EVENT_GLOBAL_WAITCURSORHIDE))
	{
		SHVASSERT(WaitCursor > 0);
		if (WaitCursor > 0)
		{
		
			WaitCursor--;
			if (WaitCursor == 0)
			{
			POINT point;
				if (::GetCursorPos(&point))
				{
				HWND wnd = ::WindowFromPoint(point);
					if (wnd && (HINSTANCE)GetWindowLongPtr(wnd,GWL_HINSTANCE) == Win32::GetInstance(GetMainWindow()))
					{
						::SendMessage(wnd,WM_SETCURSOR,(WPARAM)wnd,MAKEWORD(0,1));
					}
				}
			}
		}
	}
}

/*************************************
 * EmitControlEvent
 *************************************/
void SHVGUIManagerWin32::EmitControlEvent(SHVControl* control, SHVInt controlEvent)
{
SHVListControlEvent* list = ControlEventMap.Find(control);

	if (list)
	{
	SHVEventRef event = new SHVEvent(this,EventControl,controlEvent,control);
	bool lock = Modules.LockEvent();

		if (lock)
		{
		SHVEventQueue* queue;
		SHVList<SHVEventQueue*> queuelist;
		SHVListIterator<SHVEventQueue*> queueItr(queuelist);
		SHVListIteratorControlEvent itr(*list);

			// emit the event
			while (itr.MoveNext())
			{
				if (itr.Get().Event == controlEvent)
				{
					queue = itr.Get().Subscriber->Emit(Modules,event);
		
					if (queue)
					{
						if (!queuelist.Find(queue))
							queuelist.AddTail(queue);
					}
				}
			}

			// signal event queues
			while (queueItr.MoveNext())
				queueItr.Get()->SignalDispatcher();


			Modules.UnlockEvent();
		}
	}
}

/*************************************
 * CreateCommandID
 *************************************/
int SHVGUIManagerWin32::CreateCommandID(HANDLE handle, SHVInt value)
{
size_t index;
size_t handleCount = CommandIDMap.CalculateCount();

	for (index = 0; index < handleCount; index++)
	{
		if (!CommandIDMap[index])
		{
			CommandIDMap.Replace(index,new CommandID(handle,value));
			break;
		}
	}

	if (index == handleCount)
	{
		CommandIDMap.Add(new CommandID(handle,value));
	}

	return int(index + __CMDOFFSET);
}

/*************************************
 * CommandIDToHandle
 *************************************/
HANDLE SHVGUIManagerWin32::CommandIDToHandle(int cmdID)
{
size_t index = size_t(cmdID >= __CMDOFFSET ? cmdID-__CMDOFFSET : SIZE_T_MAX);
	return (index < CommandIDMap.CalculateCount() && CommandIDMap[index] ? CommandIDMap[index]->Handle : NULL);
}

/*************************************
 * CommandIDToValue
 *************************************/
SHVInt SHVGUIManagerWin32::CommandIDToValue(int cmdID)
{
size_t index = (cmdID >= __CMDOFFSET ? size_t(cmdID-__CMDOFFSET) : SIZE_T_MAX);
	return (index < CommandIDMap.CalculateCount() && CommandIDMap[index] ? CommandIDMap[index]->Value : SHVInt());
}

/*************************************
 * RemoveCommandIDs
 *************************************/
void SHVGUIManagerWin32::RemoveCommandIDs(HANDLE handle)
{
size_t index;
size_t handleCount = CommandIDMap.CalculateCount();

	for (index = 0; index < handleCount; index++)
	{
		if (CommandIDMap[index] && CommandIDMap[index]->Handle == handle)
		{
			CommandIDMap.Replace(index,NULL);
		}
	}

	CommandIDMap.Truncate();
}
