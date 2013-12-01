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
#include "../../include/platformspc.h"

#include "../../include/gui/shvcontroledit.h"

//=========================================================================================================
// SHVControlEdit - base edit control class
//=========================================================================================================

/*************************************
 * Destructor
 *************************************/
SHVControlEdit::~SHVControlEdit()
{
}

/*************************************
 * GetType
 *************************************/
int SHVControlEdit::GetType()
{
	return SHVControl::TypeEdit;
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVControlEdit::GetData(SHVDataBinder* data)
{
	if (data)
	{
		return data->SetValue(GetText());
	}

	return SHVBool::False;
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVControlEdit::SetData(SHVDataBinder* data)
{
	if (data)
	{
		SetText(data->GetValue());
		return SHVBool::True;
	}

	return SHVBool::False;
}

/*************************************
 * PerformChanged
 *************************************/
void SHVControlEdit::PerformChanged()
{
	if (!Changed.IsNull())
	{
		Changed->EmitNow(GetModuleList(),new SHVEvent(NULL,EventChanged,SHVInt(),this));
	}
}

/*************************************
 * SubscribeChanged
 *************************************/
void SHVControlEdit::SubscribeChanged(SHVEventSubscriberBase* subscriber)
{
	Changed = subscriber;
}
