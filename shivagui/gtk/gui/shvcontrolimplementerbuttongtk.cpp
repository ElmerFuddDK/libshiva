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
#include "../../../include/utils/shvstringutf8.h"

#include "shvcontrolimplementerbuttongtk.h"
#include "shvgtk.h"


//=========================================================================================================
// SHVControlImplementerButtonGtk - button implementation
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerButtonGtk::SHVControlImplementerButtonGtk() : SHVControlImplementerGtkWidget<SHVControlImplementerButton>()
{
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerButtonGtk::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && parent && parent->IsCreated())
	{
		SetHandle(gtk_button_new_with_label(NULL));
		gtk_container_add(GTK_CONTAINER (parent->GetNative()), GetHandle());
		
		g_signal_connect (G_OBJECT (GetHandle()), "clicked",
						  G_CALLBACK (SHVControlImplementerButtonGtk::on_clicked), owner);
		owner->SetFont(NULL,true);
		owner->SetFlag(flags);
		
		return IsCreated();
	}
	
	return SHVBool::False;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerButtonGtk::GetSubType(SHVControl* owner)
{
	SHVUNUSED_PARAM(owner);
	return SHVControl::SubTypeDefault;
}

/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlImplementerButtonGtk::GetText()
{
SHVStringUTF8 retVal;

	if (IsCreated())
	{
		retVal = gtk_button_get_label(GTK_BUTTON (GetHandle()));
	}

	return retVal.ToStrT();
}

/*************************************
 * SetText
 *************************************/
void SHVControlImplementerButtonGtk::SetText(SHVControlButton* owner, const SHVStringC& text, bool autoSize)
{
	SHVUNUSED_PARAM(owner);
	
	SHVASSERT(IsCreated());

	if (IsCreated())
	{
		gtk_button_set_label(GTK_BUTTON (GetHandle()), text.ToStrUTF8().GetSafeBuffer());
		
		if (autoSize)
		{
		SHVRect rect(GetRect(owner));
		SHVFontRef font = GetFont(owner);

			rect.SetWidth((font->CalculateTextWidth(text)*160)/100);

			SetRect(owner,rect);
		}
	}
}

///\cond INTERNAL
/*************************************
 * WndProc
 *************************************/
void SHVControlImplementerButtonGtk::on_clicked(GtkButton *button, gpointer user_data)
{
SHVControlButton* owner = (SHVControlButton*)user_data;
//SHVControlImplementerLabelGtk* self = (SHVControlImplementerLabelGtk*)owner->GetImplementor();
	owner->PerformClicked();
}
///\endcond
