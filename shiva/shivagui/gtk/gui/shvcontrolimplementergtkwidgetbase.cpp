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

#include "shvcontrolimplementergtkwidgetbase.h"
#include "shvgtk.h"
#include "utils/shvfontgtk.h"
//#include "utils/shvdrawgtk.h"


//=========================================================================================================
// SHVControlImplementerGtkWidgetBase - implementation of base implementer methods
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerGtkWidgetBase::SHVControlImplementerGtkWidgetBase()
{
	Window = NULL;
}

/*************************************
 * Destructor
 *************************************/
SHVControlImplementerGtkWidgetBase::~SHVControlImplementerGtkWidgetBase()
{
	SHVASSERT(!IsCreated());
}

/*************************************
 * IsCreated
 *************************************/
SHVBool SHVControlImplementerGtkWidgetBase::IsCreated()
{
	return (Window != NULL);
}

/*************************************
 * Reparent
 *************************************/
SHVBool SHVControlImplementerGtkWidgetBase::Reparent(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	
	SHVASSERT(IsCreated() && parent->IsCreated());

	if (IsCreated())
	{
	GtkContainer* oldParent;
		g_object_get(Window,"parent",&oldParent,NULL);
		if (oldParent)
		{
			gtk_container_remove(oldParent, Window);
		}
		gtk_container_add(GTK_CONTAINER (parent->GetNative()), Window);
		SetFlag(owner, flags);
	}

	return IsCreated();
}

/*************************************
 * Destroy
 *************************************/
SHVBool SHVControlImplementerGtkWidgetBase::Destroy(SHVControl* owner)
{
SHVBool retVal(IsCreated());

	SHVUNUSED_PARAM(owner);

	if (retVal)
	{
		gtk_widget_destroy(Window);
		Window = NULL;
	}

	return retVal;
}

/*************************************
 * GetRect
 *************************************/
SHVRect SHVControlImplementerGtkWidgetBase::GetRect(SHVControl* owner)
{
SHVRect retVal;

	SHVASSERT(IsCreated() && owner && owner->GetImplementor()->GetNative() == GetHandle());

	if (IsCreated())
	{
	GtkFixed* parent;
	gint w,h;
		g_object_get(Window,"parent",&parent,NULL);
		if (parent)
		{
		GList* lst = NULL;
			for (lst = parent->children; lst; lst = lst->next)
			{
				if ( ((GtkFixedChild*)lst->data)->widget == Window )
				{
					retVal.SetX( ((GtkFixedChild*)lst->data)->x );
					retVal.SetY( ((GtkFixedChild*)lst->data)->y );
					break;
				}
			}
		}
		gtk_widget_get_size_request(Window,&w,&h);
		retVal.SetWidth(w);
		retVal.SetHeight(h);
	}

	return retVal;
}

/*************************************
 * SetRect
 *************************************/
void SHVControlImplementerGtkWidgetBase::SetRect(SHVControl* owner, const SHVRect& rect)
{
	SHVASSERT(IsCreated() && owner && owner->GetImplementor()->GetNative() == GetHandle());

	if (IsCreated())
	{
	GtkFixed* parent;
		g_object_get(Window,"parent",&parent,NULL);
		if (parent)
		{
			gtk_fixed_move(parent, Window, rect.GetX(), rect.GetY());
		}
		gtk_widget_set_size_request(Window,rect.GetWidth() > 0 ? rect.GetWidth() : 0,rect.GetHeight() > 0 ? rect.GetHeight() : 0);
	}
}

/*************************************
 * SetFlag
 *************************************/
SHVBool SHVControlImplementerGtkWidgetBase::SetFlag(SHVControl* owner, int flag, bool enable)
{
SHVBool retVal(IsCreated());

	if (retVal && (flag & SHVControl::FlagVisible))
	{
		if (enable != GetFlag(owner,SHVControl::FlagVisible))
		{
			if (enable)
				gtk_widget_show_now(Window);
			else
				gtk_widget_hide(Window);
		}
	}

	if (retVal && (flag & SHVControl::FlagDisabled))
	{
	gboolean oldSensitive;
	gboolean sensitive = ( enable ? FALSE : TRUE );
		g_object_get(G_OBJECT (Window), "sensitive", &oldSensitive, NULL);
		if (oldSensitive != sensitive)
			g_object_set(G_OBJECT (Window), "sensitive", sensitive, NULL);
	}

	return retVal;
}

/*************************************
 * GetFlag
 *************************************/
bool SHVControlImplementerGtkWidgetBase::GetFlag(SHVControl* owner, int flag)
{
bool retVal(IsCreated());

	SHVUNUSED_PARAM(owner);

	if (retVal && (flag & SHVControl::FlagVisible))
	{
	gboolean b;
		g_object_get(Window,"visible",&b, NULL);
		retVal = b;
	}

	if (retVal && (flag & SHVControl::FlagDisabled))
	{
	gboolean sensitive;
		g_object_get(G_OBJECT (Window), "sensitive", &sensitive, NULL);
		retVal = retVal && (sensitive ? false : true);
	}

	return retVal;
}

/*************************************
 * GetFont
 *************************************/
SHVFont* SHVControlImplementerGtkWidgetBase::GetFont(SHVControl* owner)
{
	SHVUNUSED_PARAM(owner);
	return Font;
}

/*************************************
 * SetFont
 *************************************/
SHVBool SHVControlImplementerGtkWidgetBase::SetFont(SHVControl* owner, SHVFont* font, SHVInt newHeight)
{
SHVBool retVal(IsCreated());

	if (retVal && font)
	{
		Font = (SHVFontGtk*)font;

		gtk_widget_modify_font(Window,Font->GetFont());

		if (!newHeight.IsNull())
		{
		SHVRect rect(GetRect(owner));
			SetRect(owner, SHVRect(rect.GetLeft(),rect.GetTop(),rect.GetRight(),newHeight-rect.GetTop()));
		}
	}

	return retVal;
}

/*************************************
 * CalculateMinSize
 *************************************/
SHVPoint SHVControlImplementerGtkWidgetBase::CalculateMinSize(SHVControl* owner, int widthInChars, int heightInChars)
{

	SHVASSERT(IsCreated());

	if (widthInChars || heightInChars)
	{
	SHVFontRef font = GetFont(owner);

		if (font.IsNull())
			font = owner->GetManager()->GetFont(SHVGUIManager::CfgFontNormal);

		return SHVPoint( widthInChars * font->GetApproximateWidth(), heightInChars * font->GetCellHeight() );
	}

	return SHVPoint();
}

/*************************************
 * GetHandle
 *************************************/
GtkWidget* SHVControlImplementerGtkWidgetBase::GetHandle()
{
	return Window;
}

/*************************************
 * SetHandle
 *************************************/
void SHVControlImplementerGtkWidgetBase::SetHandle(GtkWidget* handle)
{
	if (Window == handle)
		return;

	SHVASSERT(!IsCreated());
	Window = handle;
}
