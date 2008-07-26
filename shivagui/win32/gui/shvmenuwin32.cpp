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

#include "shvmenuwin32.h"
#include "shvwin32.h"

#ifndef TPM_LEFTBUTTON
# define TPM_LEFTBUTTON 0
#endif

#ifndef TPM_RIGHTBUTTON
# define TPM_RIGHTBUTTON 0
#endif

#ifndef MF_DISABLED
# define MF_DISABLED 0
#endif

///\todo Implement control container menu support in windows CE
#if defined(__SHIVA_POCKETPC)
# include <aygshell.h>
#elif defined(__SHIVA_WINCE)
HMENU GetMenu(HWND hWnd)
{
	return NULL;
}
void SetMenu(HWND hWnd, HMENU hMenu)
{
}
#endif

//=========================================================================================================
// SHVMenuWin32
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVMenuWin32::SHVMenuWin32(SHVGUIManagerWin32* manager, SHVMenu::Types type, SHVEventSubscriberBase* subscriber, SHVControl* parent)
	: Manager(manager), Type(type), Subscriber(subscriber), Parent(parent)
{
	hMenuTopLevel = NULL;
	hMenu = NULL;
}

/*************************************
 * Destructor
 *************************************/
SHVMenuWin32::~SHVMenuWin32()
{
	if (hMenu && (Type == TypePopup || Type == TypeControlContainer))
	{
		if (hMenu == hMenuTopLevel)
			Manager->RemoveCommandIDs(hMenu);
		::DestroyMenu(hMenu);
	}
}

/*************************************
 * GetType
 *************************************/
SHVMenu::Types SHVMenuWin32::GetType()
{
	return Type;
}

/*************************************
 * GetContainerMode
 *************************************/
SHVMenu::ContainerModes SHVMenuWin32::GetContainerMode()
{
	if (Type != SHVMenu::TypeControlContainer)
		return SHVMenu::ContainerInvalid;
#ifdef __SHIVA_POCKETPC
	return SHVMenu::ContainerCompact;
#else
	return SHVMenu::ContainerNormal;
#endif
}

/*************************************
 * GetCompactButtonCount
 *************************************/
SHVInt SHVMenuWin32::GetCompactButtonCount()
{
#ifdef __SHIVA_POCKETPC
	return 2;
#else
	return SHVInt();
#endif
}

/*************************************
 * AddStringItem
 *************************************/
void SHVMenuWin32::AddStringItem(SHVInt id, const SHVStringC name, int flags)
{
	EnsureMenuCreated();
	if (hMenu)
	{
	int disabledflag = ( (flags&FlagDisabled) ? MF_DISABLED|MF_GRAYED : 0);
	int cmdID = Manager->CreateCommandID(hMenuTopLevel,id);
		::AppendMenu(hMenu,MF_STRING|disabledflag,cmdID,name.GetSafeBuffer());
	}
}

/*************************************
 * AddSeparator
 *************************************/
void SHVMenuWin32::AddSeparator()
{
	EnsureMenuCreated();
	if (hMenu)
		::AppendMenu(hMenu,MF_SEPARATOR,0,_T(""));
}

/*************************************
 * AddSubMenu
 *************************************/
SHVMenu* SHVMenuWin32::AddSubMenu(const SHVStringC name)
{
	EnsureMenuCreated();
	if (hMenu)
	{
	SHVMenuWin32* retVal = new SHVMenuWin32(Manager,SHVMenu::TypeSub, NULL, Parent);
		retVal->hMenu = ::CreatePopupMenu();
		retVal->hMenuTopLevel = hMenuTopLevel;
		::AppendMenu(hMenu,MF_POPUP,(UINT_PTR)retVal->hMenu,name.GetSafeBuffer());
		return retVal;
	}
	else
	{
		return NULL;
	}
}	

/*************************************
 * Show
 *************************************/
void SHVMenuWin32::Show(PopupTypes type, SHVPoint offset)
{
HWND hParent = Win32::GetHandle(Parent);
HMENU oldMenu;
int flags;
	if (hMenu && hParent && !Manager->MenuMap.Find(hMenu))
	{
		Manager->MenuMap[hMenu] = this;
		switch (Type)
		{
		case TypePopup:
			flags = TPM_LEFTBUTTON | TPM_RIGHTBUTTON | TPM_LEFTALIGN | TPM_RETURNCMD;
			switch (type)
			{
			case PopupBelowWindow:
				{
				RECT rct;
					::GetWindowRect(hParent,&rct);
					offset.x += rct.left;
					offset.y += rct.bottom;
				}
				break;
			case PopupCenterWindow:
				{
				RECT rct;
					::GetWindowRect(hParent,&rct);
					offset.x += rct.left + (rct.right-rct.left)/2;
					offset.y += rct.top + (rct.bottom-rct.top)/2;
					flags |= TPM_CENTERALIGN | TPM_VCENTERALIGN;
				}
				break;
			default:
			case PopupMouse:
			case PopupDefault:
				{
				DWORD msgPos = ::GetMessagePos();
					offset.x += LOWORD(msgPos);
					offset.y += HIWORD(msgPos);
				}
				break;
			}
			{
			int cmd = ::TrackPopupMenu(hMenu,flags,offset.x,offset.y,0,hParent,NULL);
				if (!cmd)
					EmitEvent(SHVInt());
				else
					::PostMessage(Win32::GetHandle(Manager->GetMainWindow()),WM_COMMAND,cmd,NULL);
			}
			break;
		case TypeControlContainer:
#ifdef __SHIVA_POCKETPC
			SHVUNUSED_PARAM(oldMenu);
			SHVASSERT(false); // should never occur
#else
			oldMenu = ::GetMenu(hParent);
			if (oldMenu)
			{
			SHVMenuWin32* oldMenuWin32 = (SHVMenuWin32*)(SHVMenu*)Manager->MenuMap[oldMenu];

				Manager->RemoveCommandIDs(oldMenu);
				oldMenuWin32->hMenuTopLevel = oldMenuWin32->hMenu = NULL;
				Manager->MenuMap.Remove(oldMenu);
				::DestroyMenu(oldMenu);
			}
			::SetMenu(hParent,hMenu);
#endif
			break;
		default:
			SHVASSERT(false);
			return;
		}
	}
}

/*************************************
 * EmitEvent
 *************************************/
void SHVMenuWin32::EmitEvent(SHVInt id)
{
SHVMenuRef refSelf = this; // make sure we aren't destroyed in this process
	if (!Subscriber.IsNull())
	{
		Subscriber->EmitNow(Manager->GetModuleList(),new SHVEvent(NULL,SHVControl::EventMenu,id,Parent));
	}
	if (Type == TypePopup && hMenu)
	{
	HMENU tmpMenu = hMenu;

		SHVASSERT(hMenuTopLevel == hMenu);

		hMenu = hMenuTopLevel = NULL;
		Manager->RemoveCommandIDs(tmpMenu);
		Manager->MenuMap.Remove(tmpMenu);
		::DestroyMenu(tmpMenu);
	}
}

///\cond INTERNAL
/*************************************
 * EnsureMenuCreated
 *************************************/
void SHVMenuWin32::EnsureMenuCreated()
{
	if (!hMenu)
	{
#ifdef __SHIVA_POCKETPC
		hMenuTopLevel = hMenu = ::CreatePopupMenu();
#else
		hMenuTopLevel = hMenu = ( Type == TypePopup ? ::CreatePopupMenu() :  ::CreateMenu() );
#endif
	}
}


#ifdef __SHIVA_POCKETPC
//=========================================================================================================
// SHVMenuContainerPocketPC
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVMenuContainerPocketPC::SHVMenuContainerPocketPC(SHVGUIManagerWin32* manager, SHVEventSubscriberBase* subscriber, SHVControl* parent)
	: SHVMenuWin32(manager,SHVMenu::TypeControlContainer,subscriber,parent)
{
	CmdBar = NULL;
	Button1Enabled = Button2Enabled = true;
}

/*************************************
 * Destructor
 *************************************/
SHVMenuContainerPocketPC::~SHVMenuContainerPocketPC()
{
	if (!Menu1.IsNull())
	{
		Manager->RemoveCommandIDs(Menu1->hMenu);
		Menu1 = NULL;
	}
	if (!Menu2.IsNull())
	{
		Manager->RemoveCommandIDs(Menu2->hMenu);
		Menu2 = NULL;
	}
}

/*************************************
 * AddStringItem
 *************************************/
void SHVMenuContainerPocketPC::AddStringItem(SHVInt id, const SHVStringC name, int flags)
{
	if (Button1MenuName.IsNull())
	{
		Menu1 = NULL;
		Button1MenuName = name;
		Button1Enabled = !(flags&FlagDisabled);
		Button1ID = id;
	}
	else if (Button2MenuName.IsNull())
	{
		Menu2 = NULL;
		Button2MenuName = name;
		Button2Enabled = !(flags&FlagDisabled);
		Button2ID = id;
	}
	else
	{
		SHVASSERT(false); // oh dear, no more buttons! - we'll never get grandfathers sweater done
	}
}

/*************************************
 * AddSeparator
 *************************************/
void SHVMenuContainerPocketPC::AddSeparator()
{
	// ho humm
}

/*************************************
 * AddSubMenu
 *************************************/
SHVMenu* SHVMenuContainerPocketPC::AddSubMenu(const SHVStringC name)
{
SHVMenu* retVal;
	if (Button1MenuName.IsNull())
	{
		retVal = Menu1 = new SHVMenuWin32(this->Manager,SHVMenu::TypeControlContainer,Subscriber,Parent);
		Button1MenuName = name;
		Button1Enabled = true;
		Button1ID.SetToNull();
	}
	else if (Button2MenuName.IsNull())
	{
		retVal = Menu2 = new SHVMenuWin32(this->Manager,SHVMenu::TypeControlContainer,Subscriber,Parent);
		Button2MenuName = name;
		Button2Enabled = true;
		Button2ID.SetToNull();
	}
	else if (!Menu1.IsNull())
	{
		retVal = Menu1->AddSubMenu(name);
	}
	else if (!Menu2.IsNull())
	{
		retVal = Menu2->AddSubMenu(name);
	}
	else
	{
		SHVASSERT(false); // nothing to put the menu on ... shame oh shame
		retVal = NULL;
	}

	return retVal;
}

/*************************************
 * Show
 *************************************/
void SHVMenuContainerPocketPC::Show(PopupTypes type, SHVPoint offset)
{
HWND hParent = Win32::GetHandle(Parent);
HMENU oldMenu;
	if (hParent)
	{
		SHVUNUSED_PARAM(oldMenu);
		if (CmdBar && CmdBar->hCmdWnd)
		{
			if (!Menu1.IsNull())
				Manager->MenuMap[Menu1->hMenu] = Menu1;
			if (!Menu2.IsNull())
				Manager->MenuMap[Menu2->hMenu] = Menu2;
			CmdBar->SetMenu(hParent, this);
		}
	}
}

/*************************************
 * PerformButton
 *************************************/
void SHVMenuContainerPocketPC::PerformButton(int num, SHVPoint offset)
{
SHVMenuWin32* menu = NULL;
SHVInt cmdID;

	if (num == 0)
	{
		if (!Menu1.IsNull())
			menu = Menu1;
		else
			cmdID = Button1ID;
	}
	else if (num == 1)
	{
		if (!Menu2.IsNull())
			menu = Menu2;
		else
			cmdID = Button2ID;
	}

	if (menu)
	{
	HWND hParent = Win32::GetHandle(Parent);
	int cmd = ::TrackPopupMenu(menu->hMenu,TPM_LEFTBUTTON|TPM_RIGHTBUTTON|(num == 0 ? TPM_LEFTALIGN : TPM_RIGHTALIGN)|TPM_BOTTOMALIGN|TPM_RETURNCMD,offset.x,offset.y,0,hParent,NULL);
		if (!cmd)
			EmitEvent(SHVInt());
		else
			::PostMessage(Win32::GetHandle(Manager->GetMainWindow()),WM_COMMAND,cmd,NULL);
	}
	else
	{
		EmitEvent(cmdID);
	}
}


//=========================================================================================================
// SHVMenuCommandBarPocketPC
//=========================================================================================================

/*************************************
 * Consrtructor
 *************************************/
SHVMenuCommandBarPocketPC::SHVMenuCommandBarPocketPC(HWND parent, HINSTANCE hinstance, SHVGUIManagerWin32* manager)
	: hInstance(hinstance), Manager(manager)
{
SHMENUBARINFO mbi;

	memset(&mbi, 0, sizeof(SHMENUBARINFO));
	mbi.cbSize     = sizeof(SHMENUBARINFO);
	mbi.hwndParent = parent;
	mbi.nToolBarId = 100;
	mbi.hInstRes   = hInstance;
	mbi.nBmpId     = 0;
	mbi.cBmpImages = 0;
	mbi.dwFlags    = SHCMBF_EMPTYBAR;

	SHCreateMenuBar(&mbi);
	hCmdWnd = mbi.hwndMB;

}

/*************************************
 * Destructor
 *************************************/
SHVMenuCommandBarPocketPC::~SHVMenuCommandBarPocketPC()
{
	if (hCmdWnd)
		CommandBar_Destroy(hCmdWnd);

	SetMenu(NULL, NULL);
}


/*************************************
 * InitializeMenu
 *************************************/
void SHVMenuCommandBarPocketPC::InitializeMenu(SHVMenuContainerPocketPC* menu)
{
	if (menu && menu->GetType() == SHVMenu::TypeControlContainer)
		menu->CmdBar = this;
}

/*************************************
 * Show
 *************************************/
void SHVMenuCommandBarPocketPC::SetMenu(HWND parent, SHVMenuContainerPocketPC* menu)
{

	if (menu && Menu == menu)
		return;

	if (!Menu.IsNull())
	{
	SHMENUBARINFO mbi;

		// Reset the command bar
		SHVVERIFY(::DestroyWindow(hCmdWnd));
		memset(&mbi, 0, sizeof(SHMENUBARINFO));
		mbi.cbSize     = sizeof(SHMENUBARINFO);
		mbi.hwndParent = parent;
		mbi.nToolBarId = 100;
		mbi.hInstRes   = hInstance;
		mbi.nBmpId     = 0;
		mbi.cBmpImages = 0;
		mbi.dwFlags    = SHCMBF_EMPTYBAR;

		SHCreateMenuBar(&mbi);
		hCmdWnd = mbi.hwndMB;


		// Remove the old menu stuff, thus destroying it
		Menu = NULL; // rely on the SHVMenuContainerPocketPC destructor to delete everything
	}

	Menu = menu;

	if (!Menu.IsNull())
	{
	TBBUTTON tbButton;

		///\todo figure out why the buttons don't get shown on older pocket pc's

		memset(&tbButton, 0, sizeof(TBBUTTON));
		tbButton.iBitmap   = I_IMAGENONE;
		tbButton.fsState   = (!Menu->Button1MenuName.IsNull() && Menu->Button1Enabled ? TBSTATE_ENABLED : 0);
		tbButton.fsStyle   = TBSTYLE_BUTTON|TBSTYLE_AUTOSIZE;
		tbButton.dwData    = 0;
		tbButton.idCommand = 0;
		tbButton.iString   = ::SendMessage(hCmdWnd, TB_ADDSTRING,NULL,(LPARAM)(Menu->Button1MenuName.IsEmpty() ? _T(" ") : Menu->Button1MenuName.GetSafeBuffer()));
		::SendMessage(hCmdWnd,TB_INSERTBUTTON,::SendMessage(hCmdWnd,TB_BUTTONCOUNT,0,0),(LPARAM)&tbButton);

		memset(&tbButton, 0, sizeof(TBBUTTON));
		tbButton.iBitmap   = I_IMAGENONE;
		tbButton.fsState   = (!Menu->Button2MenuName.IsNull() && Menu->Button2Enabled ? TBSTATE_ENABLED : 0);
		tbButton.fsStyle   = TBSTYLE_BUTTON|TBSTYLE_AUTOSIZE;
		tbButton.dwData    = 0;
		tbButton.idCommand = 1;
		tbButton.iString   = ::SendMessage(hCmdWnd, TB_ADDSTRING,NULL,(LPARAM)(Menu->Button2MenuName.IsEmpty() ? _T(" ") : Menu->Button2MenuName.GetSafeBuffer()));
		::SendMessage(hCmdWnd,TB_INSERTBUTTON,::SendMessage(hCmdWnd,TB_BUTTONCOUNT,0,0),(LPARAM)&tbButton);

	}
}

/*************************************
 * OnCommandMsg
 *************************************/
bool SHVMenuCommandBarPocketPC::OnCommandMsg(HWND hWnd, WPARAM wParam, LPARAM lParam) // returns true if it thinks this message is for this command bar
{
HWND hParent;

	if (!lParam)
		return false;

	hParent = ::GetParent((HWND)lParam); // the parent to the control sending the command message is one of our buttons

	if (!HIWORD(wParam) && hParent == hCmdWnd)
	{
	int butID = LOWORD(wParam);
		if (!Menu.IsNull() && (butID == 0 || butID == 1))
		{
		RECT rect;
			::GetWindowRect(hCmdWnd,&rect);
			Menu->PerformButton(butID,SHVPoint(butID == 0 ? rect.left : rect.right,rect.top));
		}
		return true;
	}
	return false;
}
#endif
///\endcond
