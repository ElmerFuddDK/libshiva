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

#include "shvcontrolimplementereditgtk.h"
#include "utils/shvdrawgtk.h"


//=========================================================================================================
// SHVControlImplementerEditGtk
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerEditGtk::SHVControlImplementerEditGtk() : SHVControlImplementerGtkWidget<SHVControlImplementerEdit>()
{
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerEditGtk::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && parent && parent->IsCreated())
	{
		SetHandle(gtk_entry_new());
		gtk_container_add(GTK_CONTAINER (parent->GetNative()), GetHandle());
		
		owner->SetFont(NULL,true);
		owner->SetFlag(flags);
		
		return IsCreated();
	}
	
	return SHVBool::False;
}

/*************************************
 * SetFlag
 *************************************/
SHVBool SHVControlImplementerEditGtk::SetFlag(SHVControl* owner, int flag, bool enable)
{
SHVBool retVal(SHVControlImplementerGtkWidget<SHVControlImplementerEdit>::SetFlag(owner,flag,enable));

	if (retVal)
	{
		if ((flag & SHVControlEdit::FlagReadonly))
		{
			gtk_editable_set_editable(GTK_EDITABLE (GetHandle()), enable ? FALSE : TRUE);
		}

		if ((flag & SHVControlEdit::FlagFlat))
		{
			//ModifyStyleEx((enable ? 0 : WS_EX_CLIENTEDGE), (enable ? WS_EX_CLIENTEDGE : 0));
		}
	}

	return retVal;
}

/*************************************
 * GetFlag
 *************************************/
bool SHVControlImplementerEditGtk::GetFlag(SHVControl* owner, int flag)
{
bool retVal(SHVControlImplementerGtkWidget<SHVControlImplementerEdit>::GetFlag(owner,flag));
	
	if (retVal)
	{
		if ((flag & SHVControlEdit::FlagReadonly))
		{
			retVal = retVal && (gtk_editable_get_editable(GTK_EDITABLE (GetHandle())) ? false : true);
		}

		if ((flag & SHVControlEdit::FlagFlat))
		{
			//retVal = retVal && ( (exStyle & WS_EX_CLIENTEDGE) ? false : true);
		}
	}

	return retVal;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerEditGtk::GetSubType(SHVControl* owner)
{
	SHVUNUSED_PARAM(owner);
	return SHVControlEdit::SubTypeSingleLine;
}

/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlImplementerEditGtk::GetText()
{
SHVStringUTF8 retVal;

	if (IsCreated())
	{
		retVal = gtk_entry_get_text(GTK_ENTRY (GetHandle()));
	}

	return retVal.ToStrT();
}

/*************************************
 * SetText
 *************************************/
void SHVControlImplementerEditGtk::SetText(const SHVStringC& text)
{
	SHVASSERT(IsCreated());

	if (IsCreated())
	{
		gtk_entry_set_text(GTK_ENTRY (GetHandle()), text.ToStrUTF8().GetSafeBuffer());
	}
}

/*************************************
 * GetLimit
 *************************************/
int SHVControlImplementerEditGtk::GetLimit()
{
int retVal;
	
	SHVASSERT(IsCreated());

	if (IsCreated())
	{
		retVal = gtk_entry_get_max_length(GTK_ENTRY (GetHandle()));
	}
	
	return retVal;
}

/*************************************
 * SetLimit
 *************************************/
void SHVControlImplementerEditGtk::SetLimit(int limit)
{
	SHVASSERT(IsCreated());

	if (IsCreated())
	{
		gtk_entry_set_max_length(GTK_ENTRY (GetHandle()), limit);
	}
}

/*************************************
 * SetHeight
 *************************************/
void SHVControlImplementerEditGtk::SetHeight(SHVControlEdit* owner, int lines)
{
	SHVUNUSED_PARAM(owner);

	SHVASSERT(false); // should never happen since it is single line
}

/*************************************
 * SetSelection
 *************************************/
void SHVControlImplementerEditGtk::SetSelection(SHVControlEdit* owner, int pos, SHVInt selectFrom, SHVControlEdit::ScrollModes scroll)
{
	SHVUNUSED_PARAM(owner);
	SHVUNUSED_PARAM(scroll);

	if (IsCreated())
	{
		if (selectFrom.IsNull())
			selectFrom = pos;
	
		gtk_entry_select_region(GTK_ENTRY (GetHandle()), selectFrom, pos);
	}
}