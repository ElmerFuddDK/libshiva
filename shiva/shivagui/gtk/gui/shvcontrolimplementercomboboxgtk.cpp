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

#include "shvcontrolimplementercomboboxgtk.h"
#include "utils/shvdrawgtk.h"


//=========================================================================================================
// SHVControlImplementerComboBoxGtk
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerComboBoxGtk::SHVControlImplementerComboBoxGtk(int subType)
	: SHVControlImplementerGtkWidget<SHVControlImplementerComboBox>(), SubType(subType)
{
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerComboBoxGtk::GetSubType(SHVControl* owner)
{
	SHVUNUSED_PARAM(owner);
	return SubType;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerComboBoxGtk::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && parent && parent->IsCreated())
	{
#ifdef GTK_TYPE_COMBO_BOX_TEXT
		if (SubType == SHVControlComboBox::SubTypeEdit)
			SetHandle(gtk_combo_box_text_new_with_entry());
		else
			SetHandle(gtk_combo_box_text_new());
#else
		if (SubType == SHVControlComboBox::SubTypeEdit)
			SetHandle(gtk_combo_box_entry_new_text());
		else
			SetHandle(gtk_combo_box_new_text());
#endif
		gtk_container_add(GTK_CONTAINER (parent->GetNative()), GetHandle());

		g_signal_connect (G_OBJECT (GetHandle()), "changed",
						  G_CALLBACK (SHVControlImplementerComboBoxGtk::on_changed), owner);

		owner->SetFont(NULL,true);
		owner->SetFlag(flags);
		
		return IsCreated();
	}
	
	return SHVBool::False;
}

/*************************************
 * SetFlag
 *************************************/
SHVBool SHVControlImplementerComboBoxGtk::SetFlag(SHVControl* owner, int flag, bool enable)
{
SHVBool retVal(SHVControlImplementerGtkWidget<SHVControlImplementerComboBox>::SetFlag(owner,flag,enable));

/*	if (retVal)
	{
		if ((flag & SHVControlComboBox::Flag))
		{
			gtk_editable_set_editable(GTK_EDITABLE (GetHandle()), enable ? FALSE : TRUE);
		}

		if ((flag & SHVControlEdit::FlagFlat))
		{
			//ModifyStyleEx((enable ? 0 : WS_EX_CLIENTEDGE), (enable ? WS_EX_CLIENTEDGE : 0));
		}
	}*/

	return retVal;
}

/*************************************
 * GetFlag
 *************************************/
bool SHVControlImplementerComboBoxGtk::GetFlag(SHVControl* owner, int flag)
{
bool retVal(SHVControlImplementerGtkWidget<SHVControlImplementerComboBox>::GetFlag(owner,flag));
	
/*	if (retVal)
	{
		if ((flag & SHVControlEdit::FlagReadonly))
		{
			retVal = retVal && (gtk_editable_get_editable(GTK_EDITABLE (GetHandle())) ? false : true);
		}

		if ((flag & SHVControlEdit::FlagFlat))
		{
			//retVal = retVal && ( (exStyle & WS_EX_CLIENTEDGE) ? false : true);
		}
	}*/

	return retVal;
}

/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlImplementerComboBoxGtk::GetText(SHVControlComboBox* owner)
{
SHVStringUTF8 retVal;

	SHVUNUSED_PARAM(owner);
	if (IsCreated())
	{
#ifdef GTK_TYPE_COMBO_BOX_TEXT
		retVal = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT (GetHandle()));
#else
		retVal = gtk_combo_box_get_active_text(GTK_COMBO_BOX (GetHandle()));
#endif
	}

	return retVal.ToStrT();
}

/*************************************
 * SetText
 *************************************/
void SHVControlImplementerComboBoxGtk::SetText(SHVControlComboBox* owner, const SHVStringC& text)
{
	SHVUNUSED_PARAM(owner);
	if (IsCreated() && SubType == SHVControlComboBox::SubTypeEdit) // Allow overriding the entry text
	{
	GtkWidget* entry = gtk_bin_get_child(GTK_BIN (GetHandle()));
		SHVASSERT(entry);
		if (entry)
			gtk_entry_set_text(GTK_ENTRY (entry), text.ToStrUTF8().GetSafeBuffer());
	}
}

/*************************************
 * GetSelected
 *************************************/
SHVInt SHVControlImplementerComboBoxGtk::GetSelected(SHVControlComboBox* owner)
{
SHVInt retVal;
	SHVUNUSED_PARAM(owner);
	if (IsCreated())
	{
	gint id = gtk_combo_box_get_active(GTK_COMBO_BOX (GetHandle()));
		if (id >= 0)
			retVal = id;
	}
	return retVal;
}

/*************************************
 * SetSelected
 *************************************/
void SHVControlImplementerComboBoxGtk::SetSelected(SHVControlComboBox* owner, SHVInt index)
{
	SHVUNUSED_PARAM(owner);
	if (IsCreated())
	{
		gtk_combo_box_set_active(GTK_COMBO_BOX (GetHandle()), index.IfNull(-1));
	}
}

/*************************************
 * GetItemCount
 *************************************/
size_t SHVControlImplementerComboBoxGtk::GetItemCount(SHVControlComboBox* owner)
{
	SHVUNUSED_PARAM(owner);
	return ItemObjects.CalculateCount();
}

/*************************************
 * ClearItems
 *************************************/
void SHVControlImplementerComboBoxGtk::ClearItems(SHVControlComboBox* owner)
{
	SHVUNUSED_PARAM(owner);
	if (IsCreated())
	{
#ifdef GTK_TYPE_COMBO_BOX_TEXT
		for (size_t i = GetItemCount(owner); i; i--)
			gtk_combo_box_text_remove(GTK_COMBO_BOX_TEXT (GetHandle()), 0);
		//gtk_combo_box_text_remove_all(GTK_COMBO_BOX_TEXT (GetHandle()));
#else
		for (size_t i = GetItemCount(owner); i; i--)
			gtk_combo_box_remove_text(GTK_COMBO_BOX (GetHandle()), 0);
#endif
		ItemObjects.Clear();
	}
}

/*************************************
 * GetItemText
 *************************************/
SHVStringBuffer SHVControlImplementerComboBoxGtk::GetItemText(SHVControlComboBox* owner, size_t index)
{
SHVString retVal;
	SHVUNUSED_PARAM(owner);
	if (IsCreated())
	{
		///\todo Implement GetItemText - either run through the model or cache all the text entries
	}
	return retVal.ReleaseBuffer();
}

/*************************************
 * GetItemData
 *************************************/
SHVRefObject* SHVControlImplementerComboBoxGtk::GetItemData(SHVControlComboBox* owner, size_t index)
{
	return (index < GetItemCount(owner) ? ItemObjects[index] : NULL);
}

/*************************************
 * AddItem
 *************************************/
void SHVControlImplementerComboBoxGtk::AddItem(SHVControlComboBox* owner, const SHVStringC str, SHVRefObject* data)
{
	SHVUNUSED_PARAM(owner);
	if (IsCreated())
	{
#ifdef GTK_TYPE_COMBO_BOX_TEXT
		gtk_combo_box_text_append_text( GTK_COMBO_BOX_TEXT (GetHandle()), str.ToStrUTF8().GetSafeBuffer());
#else
		gtk_combo_box_append_text(GTK_COMBO_BOX (GetHandle()), str.ToStrUTF8().GetSafeBuffer());
#endif
		ItemObjects.Add(data);
	}
	else if (data)
	{
		data->ValidateRefCount();
	}
}

/*************************************
 * SetDropdownHeight
 *************************************/
void SHVControlImplementerComboBoxGtk::SetDropdownHeight(SHVControlComboBox* owner, int lines)
{
	SHVUNUSED_PARAM(owner);
	SHVUNUSED_PARAM(lines);
	// Unavailable in GTK
}

///\cond INTERNAL
/*************************************
 * WndProc
 *************************************/
void SHVControlImplementerComboBoxGtk::on_changed(GtkComboBox* combobox, gpointer user_data)
{
SHVControlComboBox* owner = (SHVControlComboBox*)user_data;
//SHVControlImplementerComboBoxGtk* self = (SHVControlImplementerComboBoxGtk*)owner->GetImplementor();
	SHVUNUSED_PARAM(combobox);
	owner->PerformSelectedChanged();
}
///\endcond
