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

#include "shvcontrolimplementereditwin32.h"
#include "shvwin32.h"
#include "utils/shvfontwin32.h"


//=========================================================================================================
// SHVControlImplementerEditWin32 - Edit box implementation
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerEditWin32::SHVControlImplementerEditWin32(int subType) : SHVControlImplementerWin32<SHVControlImplementerEdit>(), SubType(subType)
{
	Lines = 1;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerEditWin32::GetSubType(SHVControl* owner)
{
	return SubType;
}

/*************************************
 * SetFlag
 *************************************/
SHVBool SHVControlImplementerEditWin32::SetFlag(SHVControl* owner, int flag, bool enable)
{
SHVBool retVal(SHVControlImplementerWin32<SHVControlImplementerEdit>::SetFlag(owner,flag,enable));

	if (retVal)
	{
		if ((flag & SHVControlEdit::FlagReadonly))
			::SendMessage(GetHandle(),EM_SETREADONLY,(enable ? TRUE : FALSE),0);

		if ((flag & SHVControlEdit::FlagFlat))
			ModifyStyleEx((enable ? 0 : WS_EX_CLIENTEDGE), (enable ? WS_EX_CLIENTEDGE : 0));
	}

	return retVal;
}

/*************************************
 * GetFlag
 *************************************/
bool SHVControlImplementerEditWin32::GetFlag(SHVControl* owner, int flag)
{
bool retVal(SHVControlImplementerWin32<SHVControlImplementerEdit>::GetFlag(owner,flag));
	
	if (retVal)
	{
	int style = ::GetWindowLong(GetHandle(),GWL_STYLE);
	int exStyle = ::GetWindowLong(GetHandle(),GWL_EXSTYLE);

		if ((flag & SHVControlEdit::FlagReadonly))
			retVal = retVal && ( (style & ES_READONLY) ? true : false);

		if ((flag & SHVControlEdit::FlagFlat))
			retVal = retVal && ( (exStyle & WS_EX_CLIENTEDGE) ? false : true);
	}

	return retVal;
}

/*************************************
 * Create(parent)
 *************************************/
SHVBool SHVControlImplementerEditWin32::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
SHVBool retVal(parent && owner && !IsCreated());

	SHVASSERT(owner && owner->GetImplementor() == this);

	if (retVal)
	{
	int style = WS_BORDER|ES_LEFT|WS_TABSTOP|WS_CHILD|Win32::MapFlags(flags);
	int exStyle = WS_EX_CLIENTEDGE;

		switch (GetSubType(owner))
		{
		case SHVControlEdit::SubTypeMultiLine:
			style |= ES_MULTILINE|WS_VSCROLL|ES_WANTRETURN|ES_AUTOVSCROLL;
			break;
		case SHVControlEdit::SubTypeSingleLine:
		default:
			style |= ES_AUTOHSCROLL;
			break;
		}

		if ((flags & SHVControlEdit::FlagReadonly))
			style |= ES_READONLY;
		if ((flags & SHVControlEdit::FlagFlat))
			exStyle = 0;
		
		SetHandle(::CreateWindowEx(exStyle,_T("EDIT"), _T(""), style,
			0, 0, 0, 0, Win32::GetHandle(parent), NULL, Win32::GetInstance(owner), NULL));

		retVal = IsCreated();

		owner->SetFont(NULL,true);
	}

	return retVal;
}

/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlImplementerEditWin32::GetText()
{
	return SHVControlImplementerWin32Base::GetText();
}

/*************************************
 * SetText
 *************************************/
void SHVControlImplementerEditWin32::SetText(const SHVStringC& text)
{
	SHVControlImplementerWin32Base::SetText(text);
}

/*************************************
 * GetLimit
 *************************************/
int SHVControlImplementerEditWin32::GetLimit()
{
	SHVASSERT(IsCreated());

	return (int)::SendMessage(GetHandle(),EM_GETLIMITTEXT,0,0);
}

/*************************************
 * SetLimit
 *************************************/
void SHVControlImplementerEditWin32::SetLimit(int limit)
{
	SHVASSERT(IsCreated());

	::SendMessage(GetHandle(),EM_SETLIMITTEXT,(WPARAM)limit,0);
}

/*************************************
 * SetHeight
 *************************************/
void SHVControlImplementerEditWin32::SetHeight(SHVControlEdit* owner, int lines)
{
	Lines = lines;

	if (IsCreated())
	{
	SHVFontRef font = GetFont(owner);
		SHVControlImplementerWin32<SHVControlImplementerEdit>::SetFont(owner,font,true);
	}
}

/*************************************
 * CalculateNewHeight
 *************************************/
int SHVControlImplementerEditWin32::CalculateNewHeight(SHVControl* owner, SHVFont* font)
{
	return SHVControlImplementerWin32<SHVControlImplementerEdit>::CalculateNewHeight(owner,font)
		+ font->GetCellHeight()*(Lines-1);
}
