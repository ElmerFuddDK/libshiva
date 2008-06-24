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

#include "shvcontrolcontainerwrapperwin32.h"
#include "shvcontrolimplementerwin32.h"

//=========================================================================================================
// SHVControlContainerWrapperWin32 - wrapper for HWND's
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlContainerWrapperWin32::SHVControlContainerWrapperWin32(SHVGUIManager* manager, HWND hWnd) : SHVControlContainer(manager,new SHVControlImplementerContainerWrapperWin32())
{
	SetHandle(hWnd);
}

/*************************************
 * Destructor
 *************************************/
SHVControlContainerWrapperWin32::~SHVControlContainerWrapperWin32()
{
	SetHandle(NULL);
}

/*************************************
 * SetHandle
 *************************************/
SHVControlContainerWrapperWin32* SHVControlContainerWrapperWin32::SetHandle(HWND hWnd)
{
SHVControlImplementerContainerWrapperWin32* ctrlImpl = (SHVControlImplementerContainerWrapperWin32*)GetImplementor();

	ctrlImpl->Window = hWnd;

	return this;
}

/*************************************
 * GetType
 *************************************/
int SHVControlContainerWrapperWin32::GetType()
{
	return SHVControl::TypeContainer;
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVControlContainerWrapperWin32::GetData(SHVDataBinder* data)
{
	return SHVBool::False;
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVControlContainerWrapperWin32::SetData(SHVDataBinder* data)
{
	return SHVBool::False;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlContainerWrapperWin32::Create(int flags)
{
	return SHVBool::False;
}



//=========================================================================================================
// SHVControlImplementerWrapperWin32
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerContainerWrapperWin32::SHVControlImplementerContainerWrapperWin32() : SHVControlImplementerWin32<SHVControlImplementerContainer>()
{
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerContainerWrapperWin32::GetSubType(SHVControl* owner)
{
	return SHVControlContainer::SubTypeDefault;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerContainerWrapperWin32::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	return SHVBool::False;
}


/*************************************
 * GetRegionRect
 *************************************/
SHVRect SHVControlImplementerContainerWrapperWin32::GetRegionRect()
{
RECT nativeRect;

	SHVASSERT(IsCreated());

	::GetClientRect(GetHandle(),&nativeRect);

	return SHVRect(nativeRect.left,nativeRect.top,nativeRect.right,nativeRect.bottom);
}

/*************************************
 * SetSize
 *************************************/
void SHVControlImplementerContainerWrapperWin32::SetSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels, SHVControlContainer::PosModes mode)
{
}

/*************************************
 * GetTitle
 *************************************/
SHVStringBuffer SHVControlImplementerContainerWrapperWin32::GetTitle(SHVControlContainer* owner)
{
SHVString retVal;

	return retVal.ReleaseBuffer();
}

/*************************************
 * SetTitle
 *************************************/
void SHVControlImplementerContainerWrapperWin32::SetTitle(SHVControlContainer* owner, const SHVStringC& title)
{
}

/*************************************
 * GetColor
 *************************************/
SHVColor* SHVControlImplementerContainerWrapperWin32::GetColor(SHVControlContainer* owner)
{
	return NULL;
}

/*************************************
 * SetColor
 *************************************/
void SHVControlImplementerContainerWrapperWin32::SetColor(SHVControlContainer* owner, SHVColor* color)
{
}

/*************************************
 * SetMinimumSize
 *************************************/
void SHVControlImplementerContainerWrapperWin32::SetMinimumSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels)
{
}

/*************************************
 * GetMinimumSizeInPixels
 *************************************/
SHVPoint SHVControlImplementerContainerWrapperWin32::GetMinimumSizeInPixels(SHVControlContainer* owner)
{
	return SHVPoint(0,0);
}

/*************************************
 * SetResizable
 *************************************/
void SHVControlImplementerContainerWrapperWin32::SetResizable(bool resizable)
{
}
