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

#include "shvcontrolimplementercheckboxgtk.h"
#include "shvgtk.h"


//=========================================================================================================
// SHVControlImplementerCheckboxGtk - checkbox implementation
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerCheckboxGtk::SHVControlImplementerCheckboxGtk(int subType) : SHVControlImplementerGtkWidget<SHVControlImplementerCheckbox>()
{
	SubType = subType;
	InToggled = false;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerCheckboxGtk::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && parent && parent->IsCreated())
	{
		SetHandle(gtk_check_button_new_with_label(NULL));
		gtk_container_add(GTK_CONTAINER (parent->GetNative()), GetHandle());
		
		g_signal_connect (G_OBJECT (GetHandle()), "toggled",
						  G_CALLBACK (SHVControlImplementerCheckboxGtk::on_toggled), owner);
		owner->SetFont(NULL,true);
		owner->SetFlag(flags);
		
		return IsCreated();
	}
	
	return SHVBool::False;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerCheckboxGtk::GetSubType(SHVControl* owner)
{
	SHVUNUSED_PARAM(owner);
	return SubType;
}

/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlImplementerCheckboxGtk::GetText()
{
SHVString retVal;

	if (IsCreated())
	{
		retVal = SHVStringUTF8C(gtk_button_get_label(GTK_BUTTON (GetHandle()))).ToStrT();
	}

	return retVal.ReleaseBuffer();
}

/*************************************
 * SetText
 *************************************/
void SHVControlImplementerCheckboxGtk::SetText(SHVControlCheckbox* owner, const SHVStringC& text, bool autoSize)
{
	SHVUNUSED_PARAM(owner);
	
	SHVASSERT(IsCreated());

	if (IsCreated())
	{
		gtk_button_set_label(GTK_BUTTON (GetHandle()), text.AsStrUTF8C().GetSafeBuffer());
		
		if (autoSize)
		{
		SHVRect rect(GetRect(owner));
		SHVFontRef font = GetFont(owner);

			rect.SetWidth((font->CalculateTextWidth(text)*160)/100);

			SetRect(owner,rect);
		}
	}
}

/*************************************
 * GetChecked
 *************************************/
SHVInt SHVControlImplementerCheckboxGtk::GetChecked()
{
SHVInt retVal;

	SHVASSERT(IsCreated());

	if (IsCreated())
	{
		if (!gtk_toggle_button_get_inconsistent(GTK_TOGGLE_BUTTON (GetHandle())))
		{
			if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON (GetHandle())))
				retVal = SHVControlCheckbox::StateChecked;
			else
				retVal = SHVControlCheckbox::StateUnchecked;
		}
	}
	
	return retVal;
}

/*************************************
 * SetChecked
 *************************************/
void SHVControlImplementerCheckboxGtk::SetChecked(SHVControlCheckbox* owner, SHVInt state)
{
	SHVUNUSED_PARAM(owner);
	
	SHVASSERT(IsCreated());

	if (IsCreated())
	{
		if (state.IsNull())
		{
			gtk_toggle_button_set_inconsistent(GTK_TOGGLE_BUTTON (GetHandle()), true);
		}
		else
		{
			gtk_toggle_button_set_inconsistent(GTK_TOGGLE_BUTTON (GetHandle()), false);
			gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON (GetHandle()), state != SHVControlCheckbox::StateUnchecked);
		}
	}
}

///\cond INTERNAL
/*************************************
 * on_toggled
 *************************************/
void SHVControlImplementerCheckboxGtk::on_toggled(GtkToggleButton *togglebutton, gpointer user_data)
{
SHVControlCheckbox* owner = (SHVControlCheckbox*)user_data;
SHVControlImplementerCheckboxGtk* self = (SHVControlImplementerCheckboxGtk*)owner->GetImplementor();
SHVInt state(owner->GetChecked());

	if (self->InToggled)
		return;
	
	self->InToggled = true;

	if (state.IsNull())
		self->SetChecked(owner,SHVControlCheckbox::StateChecked);
	else if (state.IfNull(-1) == SHVControlCheckbox::StateChecked && self->SubType == SHVControlCheckbox::SubTypeTristate)
		self->SetChecked(owner,SHVInt());

	self->InToggled = false;

	owner->PerformChanged();
	SHVTRACE("Checked is %d\n", owner->GetChecked().IfNull(-1));

}
///\endcond
