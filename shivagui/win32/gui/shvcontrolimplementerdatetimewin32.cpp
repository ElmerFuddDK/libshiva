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

#include "shvcontrolimplementerdatetimewin32.h"
#include "shvwin32.h"

#include <commctrl.h>


//=========================================================================================================
// SHVControlImplementerDateTimeWin32 - date time implementation
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerDateTimeWin32::SHVControlImplementerDateTimeWin32(int subType)
	: SHVControlImplementerWin32<SHVControlImplementerDateTime>(), SubType(subType)
{
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerDateTimeWin32::GetSubType(SHVControl* owner)
{
	return SubType;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerDateTimeWin32::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && parent && parent->IsCreated())
	{
	int styles;
	int lfuwidth;

		switch (SubType)
		{
		default:
			SHVASSERT(false);
		case SHVControlDateTime::SubTypeLongDate:
			styles = DTS_LONGDATEFORMAT;
			lfuwidth = 140;
			break;
		case SHVControlDateTime::SubTypeShortDate:
			styles = DTS_SHORTDATEFORMAT;
			lfuwidth = 90;
			break;
		case SHVControlDateTime::SubTypeTime:
			styles = DTS_TIMEFORMAT;
			lfuwidth = 70;
			break;
		}

		styles |= WS_CHILD|Win32::MapFlags(flags);

		SetHandle(CreateWindowEx(0, DATETIMEPICK_CLASS, _T("DateTime"), styles,
			0, 0, 0, 0, Win32::GetHandle(parent), NULL, Win32::GetInstance(owner), NULL));
	
		if (IsCreated())
		{
		SHVFontRef font;
			OrigProc = (WNDPROC)GetWindowLongPtr(GetHandle(),GWLP_WNDPROC);
			SetWindowLongPtr(GetHandle(),GWLP_USERDATA,(LONG_PTR)owner);
			SetWindowLongPtr(GetHandle(),GWLP_WNDPROC,(LONG_PTR)&SHVControlImplementerDateTimeWin32::WndProc);

			owner->SetFont(NULL,true);

			font = GetFont(owner);

			if (!font.IsNull())
			{
				owner->SetRect(SHVRect(0,0,font->LFUToWidth(lfuwidth),font->LFUToHeight(14)));
			}
		}

		return IsCreated();
	}
	
	return SHVBool::False;
}

/*************************************
 * GetTime
 *************************************/
SHVTime SHVControlImplementerDateTimeWin32::GetTime(SHVControlDateTime* owner)
{
SHVTime retVal;
SYSTEMTIME st;

	retVal.SetLocalTime(true,false);
	if (::SendMessage(GetHandle(),DTM_GETSYSTEMTIME,0,(LPARAM)&st) == GDT_VALID)
	{
		retVal.SetYear(st.wYear);
		retVal.SetMonth(st.wMonth);
		retVal.SetDay(st.wDay);

		retVal.SetHour(st.wHour);
		retVal.SetMinute(st.wMinute);
		retVal.SetSecond(st.wSecond);
	}

	return retVal;
}

/*************************************
 * SetTime
 *************************************/
void SHVControlImplementerDateTimeWin32::SetTime(SHVControlDateTime* owner, const SHVTime& time)
{
	if (time.IsNull())
	{
		::SendMessage(GetHandle(),DTM_SETSYSTEMTIME,GDT_NONE,NULL);
	}
	else
	{
	SYSTEMTIME st;
	SHVTime t(time);

		if (!t.IsLocalTime())
			t.SetLocalTime(true);

		st.wYear = t.GetYear();
		st.wMonth = t.GetMonth();
		st.wDayOfWeek = -1;
		st.wDay = t.GetDay();
		st.wHour = t.GetHour();
		st.wMinute = t.GetMinute();
		st.wSecond = t.GetSecond();
		st.wMilliseconds = 0;

		::SendMessage(GetHandle(),DTM_SETSYSTEMTIME,GDT_VALID,(LPARAM)&st);
	}
}


///\cond INTERNAL
/*************************************
 * WndProc
 *************************************/
LRESULT SHVControlImplementerDateTimeWin32::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
SHVControlDateTime* owner = (SHVControlDateTime*)GetWindowLongPtr(hWnd,GWLP_USERDATA);
SHVControlImplementerDateTimeWin32* self = (owner ? (SHVControlImplementerDateTimeWin32*)owner->GetImplementor() : NULL);
LRESULT retVal = 0;

	retVal = CallWindowProc(self->OrigProc,hWnd,message,wParam,lParam);

	return retVal;
}
///\endcond
