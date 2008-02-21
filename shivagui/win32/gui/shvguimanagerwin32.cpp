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

#include "shvcontrolimplementercontainerwindowwin32.h"
#include "shvcontrolimplementerdialogwin32.h"
#include "shvcontrolimplementerlabelwin32.h"
#include "shvcontrolimplementereditwin32.h"
#include "shvcontrolimplementerbuttonwin32.h"
#include "../../include/shvcontrolcreator.h"
#include "../../include/shvcontrolcreatorsubtype.h"
#include "../../utilsimpl/shvregionimpl.h"


//=========================================================================================================
// SHVGUIManagerWin32 - Module for registering control types, and instantiating them
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVGUIManagerWin32::SHVGUIManagerWin32(SHVModuleList& modules) : SHVGUIManagerImpl(modules)
{
SHVFontWin32Ref font;
HINSTANCE hInstance = (HINSTANCE)GetConfig().FindPtr(CfgInstanceHandle).ToPtr();

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


	// Register fonts
	font = SHVFontWin32::CreateSystemFont();
	GetConfig().SetRef(CfgFontNormal,font);
	GetConfig().SetRef(CfgFontNormalBold,font->CreateCopy(100,SHVFont::StyleBold));
	GetConfig().SetRef(CfgFontLarge,font->CreateCopy(120));
	GetConfig().SetRef(CfgFontLargeBold,font->CreateCopy(120,SHVFont::StyleBold));


	// Set standard transparency color
	GetConfig().SetRef(CfgColorTransparent,CreateColor(0xFF,0x00,0xFF));
}

/*************************************
 * Register
 *************************************/
SHVBool SHVGUIManagerWin32::Register()
{
	return SHVGUIManagerImpl::Register();
}

/*************************************
 * Unregister
 *************************************/
void SHVGUIManagerWin32::Unregister()
{
	SHVGUIManagerImpl::Unregister();
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
		EmitEvent(new SHVEventData<MSG*>(message,this,EventPreTranslateMessage));

	return retVal;
}
