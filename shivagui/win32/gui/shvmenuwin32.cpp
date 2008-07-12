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

#ifdef __SHIVA_POCKETPC
	CmdBar = NULL;
	TopLevelNeedSeparator = false;
#endif
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
 * AddStringItem
 *************************************/
void SHVMenuWin32::AddStringItem(SHVInt id, const SHVStringC name)
{
	EnsureMenuCreated();
	if (hMenu)
	{
	int cmdID = Manager->CreateCommandID(hMenuTopLevel,id);
		::AppendMenu(hMenu,MF_STRING,cmdID,name.GetSafeBuffer());
#ifdef __SHIVA_POCKETPC
		TopLevelNeedSeparator = (Type == TypeControlContainer);
#endif
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
#ifdef __SHIVA_POCKETPC
	TopLevelNeedSeparator = false;
#endif
}

/*************************************
 * AddSubMenu
 *************************************/
SHVMenu* SHVMenuWin32::AddSubMenu(const SHVStringC name)
{
	// a hack that makes the first sub menu the main menu
#ifdef __SHIVA_POCKETPC
	if (!hMenu && Type == TypeControlContainer)
	{
		EnsureMenuCreated();
		TopLevelMenuName = name;
		return this;
	}
	else if (hMenu && TopLevelNeedSeparator)
	{
		AddSeparator();
		TopLevelNeedSeparator = false;
	}
#endif

	EnsureMenuCreated();
	if (hMenu)
	{
	SHVMenuWin32* retVal = new SHVMenuWin32(Manager,SHVMenu::TypeSub, NULL, Parent);
		retVal->hMenu = ::CreatePopupMenu();
		retVal->hMenuTopLevel = hMenuTopLevel;
#ifdef __SHIVA_POCKETPC
		retVal->CmdBar = CmdBar;
#endif
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
			if (!CmdBar || !CmdBar->hCmdWnd)
			{
				Manager->RemoveCommandIDs(hMenu);
				Manager->MenuMap.Remove(hMenu); // will destroy us
			}
			else
			{
				CmdBar->SetMenu(hParent, hMenu, TopLevelMenuName);
			}
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
#ifdef __SHIVA_POCKETPC
	else if (Type == TypeControlContainer && hMenu && hParent && Manager->MenuMap.Find(hMenu))
	{
	int cmd = ::TrackPopupMenu(hMenu,TPM_LEFTBUTTON|TPM_RIGHTBUTTON|TPM_LEFTALIGN|TPM_BOTTOMALIGN|TPM_RETURNCMD,offset.x,offset.y,0,hParent,NULL);
		if (!cmd)
			EmitEvent(SHVInt());
		else
			::PostMessage(Win32::GetHandle(Manager->GetMainWindow()),WM_COMMAND,cmd,NULL);
	}
#endif
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
// SHVMenuCommandBarPocketPC
//=========================================================================================================

/*************************************
 * Consrtructor
 *************************************/
SHVMenuCommandBarPocketPC::SHVMenuCommandBarPocketPC(HWND parent, HINSTANCE hinstance, SHVGUIManagerWin32* manager)
	: hInstance(hinstance), Manager(manager)
{
SHMENUBARINFO mbi;

	hMenu = NULL;

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

	SetMenu(NULL, NULL, NULL);
}


/*************************************
 * InitializeMenu
 *************************************/
void SHVMenuCommandBarPocketPC::InitializeMenu(SHVMenuWin32* menu)
{
	if (menu && menu->Type == SHVMenu::TypeControlContainer)
		menu->CmdBar = this;
}

/*************************************
 * Show
 *************************************/
bool SHVMenuCommandBarPocketPC::SetMenu(HWND parent, HMENU hmenu, const SHVStringC menuName) // returns true if the menu needs to be shown
{

	if (hMenu && hMenu == hmenu)
		return true;

	if (hMenu)
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
		Manager->RemoveCommandIDs(hMenu);
		Manager->MenuMap.Remove(hMenu);
		hMenu = NULL; // rely on the SHVMenuWin32 destructor to delete it
	}

	hMenu = hmenu;

	if (hMenu)
	{
	TBBUTTON tbButton;

		///\todo figure out why the buttons don't get shown on older pocket pc's

		memset(&tbButton, 0, sizeof(TBBUTTON));
		tbButton.iBitmap   = I_IMAGENONE;
		tbButton.fsState   = TBSTATE_ENABLED;
		tbButton.fsStyle   = TBSTYLE_BUTTON|TBSTYLE_AUTOSIZE;
		tbButton.dwData    = 0;
		tbButton.idCommand = 0;
		tbButton.iString   = ::SendMessage(hCmdWnd, TB_ADDSTRING,NULL,(LPARAM)menuName.GetSafeBuffer());
		::SendMessage(hCmdWnd,TB_INSERTBUTTON,::SendMessage(hCmdWnd,TB_BUTTONCOUNT,0,0),(LPARAM)&tbButton);

		memset(&tbButton, 0, sizeof(TBBUTTON));
		tbButton.iBitmap   = I_IMAGENONE;
		tbButton.fsState   = TBSTATE_ENABLED;
		tbButton.fsStyle   = TBSTYLE_BUTTON|TBSTYLE_AUTOSIZE;
		tbButton.dwData    = 0;
		tbButton.idCommand = 1;
		tbButton.iString   = ::SendMessage(hCmdWnd, TB_ADDSTRING,NULL,(LPARAM)_T("Close")); ///\todo Add some way to obtain a better close button text
		::SendMessage(hCmdWnd,TB_INSERTBUTTON,::SendMessage(hCmdWnd,TB_BUTTONCOUNT,0,0),(LPARAM)&tbButton);

	}

	return false;
}

/*************************************
 * OnCommandMsg
 *************************************/
bool SHVMenuCommandBarPocketPC::OnCommandMsg(HWND hWnd, WPARAM wParam, LPARAM lParam) // returns true if it thinks this message is for this command bar
{
	if (!HIWORD(wParam))
	{
		if (LOWORD(wParam) == 0)
		{
			if (hMenu && Manager->MenuMap.Find(hMenu))
			{
			RECT rect;
				::GetWindowRect(hCmdWnd,&rect);
				Manager->MenuMap[hMenu]->Show(SHVMenu::PopupDefault,SHVPoint(rect.left,rect.top));
			}
		}
		else if (LOWORD(wParam) == 1)
		{
			::PostMessage(hWnd,WM_CLOSE,0,0);
		}
		return true;
	}
	return false;
}
#endif
///\endcond
