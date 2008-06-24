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

#include "shvcontrolwrapperwin32.h"
#include "shvcontrolimplementerwin32.h"
#include "utils/shvdrawwin32.h"

//=========================================================================================================
// SHVControlWrapperWin32 - wrapper for HWND's
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlWrapperWin32::SHVControlWrapperWin32(SHVGUIManager* manager, HWND hWnd) : SHVControl(manager,new SHVControlImplementerWrapperWin32())
{
	SetHandle(hWnd);
}

/*************************************
 * Destructor
 *************************************/
SHVControlWrapperWin32::~SHVControlWrapperWin32()
{
	SetHandle(NULL);
}

/*************************************
 * SetHandle
 *************************************/
SHVControlWrapperWin32* SHVControlWrapperWin32::SetHandle(HWND hWnd)
{
SHVControlImplementerWrapperWin32* ctrlImpl = (SHVControlImplementerWrapperWin32*)GetImplementor();
	ctrlImpl->Window = hWnd;
	return this;
}

/*************************************
 * GetType
 *************************************/
int SHVControlWrapperWin32::GetType()
{
	return SHVControl::TypeCustom;
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVControlWrapperWin32::GetData(SHVDataBinder* data)
{
	return SHVBool::False;
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVControlWrapperWin32::SetData(SHVDataBinder* data)
{
	return SHVBool::False;
}


//=========================================================================================================
// SHVControlImplementerWrapperWin32
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerWrapperWin32::SHVControlImplementerWrapperWin32() : SHVControlImplementerWin32<SHVControlImplementer>()
{
}

/*************************************
 * GetRect
 *************************************/
SHVRect SHVControlImplementerWrapperWin32::GetRect(SHVControl* owner)
{
RECT nativeRect;
HWND parent = ::GetParent(GetHandle());

	SHVASSERT(IsCreated() && owner);

	::GetWindowRect(GetHandle(),&nativeRect);

	if (parent)
	{
	POINT topleft, bottomright;

		topleft.x = nativeRect.left;
		topleft.y = nativeRect.top;
		bottomright.x = nativeRect.right;
		bottomright.y = nativeRect.bottom;

		ScreenToClient(parent,&topleft);
		ScreenToClient(parent,&bottomright);

		return SHVRect(topleft.x,topleft.y,bottomright.x,bottomright.y);
	}

	return SHVDrawWin32::MapRect(nativeRect);
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerWrapperWin32::GetSubType(SHVControl* owner)
{
	return SHVControl::SubTypeDefault;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerWrapperWin32::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	return SHVBool::False;
}
