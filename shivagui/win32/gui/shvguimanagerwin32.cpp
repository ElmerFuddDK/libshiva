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

#include "shvguimanagerwin32.h"
#include "shvwin32.h"

#include "shvcontrolimplementerlabelwin32.h"
#include "shvcontrolimplementereditwin32.h"
#include "shvcontrolimplementerbuttonwin32.h"
#include "../../include/shvcontrolcreator.h"
#include "../../include/shvcontrolcreatorsubtype.h"


//=========================================================================================================
// SHVGUIManagerWin32 - Module for registering control types, and instantiating them
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVGUIManagerWin32::SHVGUIManagerWin32(SHVModuleList& modules) : SHVGUIManagerImpl(modules)
{
	RegisterFactory(SHVControl::TypeLabel,SHVControlLabel::SubTypeDefault,
		new SHVControlCreator<SHVControlLabel,SHVControlImplementerLabelWin32>());

	RegisterFactory(SHVControl::TypeEdit,SHVControlEdit::SubTypeSingleLine,
		new SHVControlCreatorSubType<SHVControlEdit,SHVControlImplementerEditWin32,SHVControlEdit::SubTypeSingleLine>());
	RegisterFactory(SHVControl::TypeEdit,SHVControlEdit::SubTypeMultiLine,
		new SHVControlCreatorSubType<SHVControlEdit,SHVControlImplementerEditWin32,SHVControlEdit::SubTypeMultiLine>());

	RegisterFactory(SHVControl::TypeButton,SHVControlButton::SubTypeDefault,
		new SHVControlCreator<SHVControlButton,SHVControlImplementerButtonWin32>());
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

	return retVal;
}
