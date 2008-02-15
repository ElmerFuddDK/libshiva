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

#include "shvcontrolimplementerlabelwin32.h"
#include "shvwin32.h"
#include "utils/shvdrawwin32.h"


/*************************************
 * Constructor
 *************************************/
SHVControlImplementerLabelWin32::SHVControlImplementerLabelWin32(int subType) : SHVControlImplementerWin32<SHVControlImplementerLabelCustomDraw>()
{
	SubType = subType;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerLabelWin32::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && parent && parent->IsCreated())
	{
		SetHandle(CreateWindow(_T("STATIC"), _T(""), WS_CHILD|Win32::MapFlags(flags),
			0, 0, 0, 0, Win32::GetHandle(parent), NULL, Win32::GetInstance(owner), NULL));
	
		if (IsCreated())
		{
			OrigProc = (WNDPROC)GetWindowLongPtr(GetHandle(),GWLP_WNDPROC);
			SetWindowLongPtr(GetHandle(),GWLP_USERDATA,(LONG_PTR)owner);
			SetWindowLongPtr(GetHandle(),GWLP_WNDPROC,(LONG_PTR)&SHVControlImplementerLabelWin32::WndProc);

			SetFont(owner,owner->GetManager()->GetFont(SHVGUIManager::CfgFontNormal),true);
		}

		return IsCreated();
	}
	
	return SHVBool::False;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerLabelWin32::GetSubType(SHVControl* owner)
{
	return SubType;
}

/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlImplementerLabelWin32::GetText()
{
SHVString retVal;

	SHVASSERT(IsCreated());

	retVal.SetBufferSize( GetWindowTextLength(GetHandle())+1 );
	GetWindowText(GetHandle(),(TCHAR*)retVal.GetBuffer(), (int)retVal.GetBufferLen());

	return retVal.ReleaseBuffer();
}

/*************************************
 * SetText
 *************************************/
void SHVControlImplementerLabelWin32::SetText(SHVControlLabel* owner, const SHVStringC& text, bool autoSize)
{
	SHVASSERT(IsCreated());

	SetWindowText(GetHandle(),text.GetSafeBuffer());

	if (autoSize)
	{
	SHVRect rect(GetRect(owner));
	SHVFontRef font = GetFont(owner);

		rect.SetWidth(font->CalculateTextWidth(text));

		SetRect(owner,rect);
	}
}

/*************************************
 * SubscribeDraw
 *************************************/
void SHVControlImplementerLabelWin32::SubscribeDraw(SHVEventSubscriberBase* subscriber)
{
	Subscriber = subscriber;
}

///\cond INTERNAL
/*************************************
 * WndProc
 *************************************/
LRESULT CALLBACK SHVControlImplementerLabelWin32::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
SHVControlLabel* owner = (SHVControlLabel*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
SHVControlImplementerLabelWin32* self = (owner ? (SHVControlImplementerLabelWin32*)owner->GetImplementor() : NULL);

	switch (message) 
	{
	case WM_SIZE:
		{
		LRESULT result = CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);
			::InvalidateRect(hWnd,NULL,TRUE);
			return result;
		}
	case WM_ERASEBKGND:
		{
		bool drawn = false;
			if (owner)
			{
			SHVDrawWin32Ref draw = Win32::CreateDraw(owner,(HDC)wParam);
			SHVColorRef color = owner->GetParent()->GetColor();

				if (!color.IsNull())
				{
					drawn = true;
					draw->DrawRectFilled(draw->GetClientRect(owner),color);
				}
			}
			
			if (drawn)
				return 1;
			else
				return DefWindowProc(hWnd, message, wParam, lParam);
		}
	case WM_PAINT:
		if (owner)
		{
		SHVDrawWin32Ref draw = Win32::CreateDrawPaint(owner);

			if (!self->Subscriber.IsNull())
			{
				self->Subscriber->EmitNow(owner->GetModuleList(),new SHVEventData<SHVDrawRef>((SHVDraw*)draw,NULL,SHVControl::EventDraw,NULL,owner));
			}
			else
			{
				draw->DrawText(self->GetText(),draw->GetClientRect(owner),SHVDraw::TextVCenter|SHVDraw::TextLeft|SHVDraw::TextEndEllipsis);
			}

			break;
		}
		// else continue
	default:
		return CallWindowProc(self->OrigProc,hWnd, message, wParam, lParam);
	}
	return 0;
}
///\endcond
