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
#ifdef __SHIVA_WINCE
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
 * AddStringItem
 *************************************/
void SHVMenuWin32::AddStringItem(SHVInt id, const SHVStringC name)
{
	EnsureMenuCreated();
	if (hMenu)
	{
	int cmdID = Manager->CreateCommandID(hMenuTopLevel,id);
		::AppendMenu(hMenu,MF_STRING,cmdID,name.GetSafeBuffer());
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
			break;
		default:
			SHVASSERT(false);
			return;
		}
	}
}

/*************************************
 * EnsureMenuCreated
 *************************************/
void SHVMenuWin32::EnsureMenuCreated()
{
	if (!hMenu)
	{
		hMenuTopLevel = hMenu = ( Type == TypePopup ? ::CreatePopupMenu() :  ::CreateMenu() );
	}
}

/*************************************
 * EnsureMenuCreated
 *************************************/
void SHVMenuWin32::EmitEvent(SHVInt id)
{
	if (!Subscriber.IsNull())
	{
		Subscriber->EmitNow(Manager->GetModuleList(),new SHVEvent(NULL,SHVControl::EventMenu,id,Parent));
	}
	if (Type == TypePopup && hMenu)
	{
	HMENU tmpMenu = hMenu;
	SHVMenuRef refSelf = this; // make sure we don't destroy ourselves in this process

		SHVASSERT(hMenuTopLevel == hMenu);

		hMenu = hMenuTopLevel = NULL;
		Manager->RemoveCommandIDs(tmpMenu);
		Manager->MenuMap.Remove(tmpMenu);
		::DestroyMenu(tmpMenu);
	}
}
