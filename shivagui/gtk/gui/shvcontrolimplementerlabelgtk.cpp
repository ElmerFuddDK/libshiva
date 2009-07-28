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
#include "../../../include/utils/shvstringutf8.h"

#include "shvcontrolimplementerlabelgtk.h"
//#include "utils/shvdrawgtk.h"


//=========================================================================================================
// SHVControlImplementerLabelGtk
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerLabelGtk::SHVControlImplementerLabelGtk(int subType) : SHVControlImplementerGtkWidget<SHVControlImplementerLabelCustomDraw>()
{
	SubType = subType;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerLabelGtk::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && parent && parent->IsCreated())
	{
		SetHandle(gtk_label_new(NULL));
		gtk_container_add(GTK_CONTAINER (parent->GetNative()), GetHandle());
		
		if (SubType == SHVControlLabel::SubTypeCustomDraw)
		{
			// Add some handler stuff that can do the custom draw thing
		}
		owner->SetFont(NULL,true);
		owner->SetFlag(flags);
		
		return IsCreated();
	}
	
	return SHVBool::False;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerLabelGtk::GetSubType(SHVControl* owner)
{
	return SubType;
}

/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlImplementerLabelGtk::GetText()
{
SHVStringUTF8 retVal;

	if (IsCreated())
	{
		retVal = gtk_label_get_text(GTK_LABEL (GetHandle()));
	}

	return retVal.ToStrT();
}

/*************************************
 * SetText
 *************************************/
void SHVControlImplementerLabelGtk::SetText(SHVControlLabel* owner, const SHVStringC& text, bool autoSize)
{
	SHVASSERT(IsCreated());

	if (IsCreated())
	{
		gtk_label_set_text(GTK_LABEL (GetHandle()), text.ToStrUTF8().GetSafeBuffer());
		
		if (autoSize)
		{
		SHVRect rect(GetRect(owner));
		SHVFontRef font = GetFont(owner);

			rect.SetWidth(font->CalculateTextWidth(text));

			SetRect(owner,rect);
		}
	}
}

/*************************************
 * SubscribeDraw
 *************************************/
void SHVControlImplementerLabelGtk::SubscribeDraw(SHVEventSubscriberBase* subscriber)
{
	Subscriber = subscriber;
}
