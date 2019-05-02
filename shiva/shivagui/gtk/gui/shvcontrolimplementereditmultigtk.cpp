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

#include "shvcontrolimplementereditmultigtk.h"
#include "utils/shvdrawgtk.h"


//=========================================================================================================
// SHVControlImplementerEditMultiGtk
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerEditMultiGtk::SHVControlImplementerEditMultiGtk() : SHVControlImplementerGtkWidget<SHVControlImplementerEdit>()
{
	TextView = NULL;
	Lines = 1;
	SuppressChangeEvent = false;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerEditMultiGtk::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && parent && parent->IsCreated())
	{
		SetHandle(gtk_scrolled_window_new(NULL,NULL));
		gtk_container_add(GTK_CONTAINER (GetHandle()), TextView = gtk_text_view_new());
		gtk_container_add(GTK_CONTAINER (parent->GetNative()), GetHandle());
		gtk_text_view_set_wrap_mode(GTK_TEXT_VIEW (TextView), GTK_WRAP_WORD_CHAR);
		gtk_widget_show(TextView);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW (GetHandle()), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
		gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW (GetHandle()), GTK_SHADOW_ETCHED_IN);
		
		
		g_signal_connect (G_OBJECT (gtk_text_view_get_buffer(GTK_TEXT_VIEW(TextView))), "changed",
						  G_CALLBACK (SHVControlImplementerEditMultiGtk::on_changed), owner);
		
		owner->SetFont(NULL,true);
		owner->SetFlag(flags);
		
		return IsCreated();
	}
	
	return SHVBool::False;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerEditMultiGtk::Destroy(SHVControl* owner)
{
	if (TextView)
	{
		gtk_widget_destroy(TextView);
		TextView = NULL;
	}
	
	return SHVControlImplementerGtkWidget<SHVControlImplementerEdit>::Destroy(owner);
}

/*************************************
 * SetFlag
 *************************************/
SHVBool SHVControlImplementerEditMultiGtk::SetFlag(SHVControl* owner, int flag, bool enable)
{
SHVBool retVal(SHVControlImplementerGtkWidget<SHVControlImplementerEdit>::SetFlag(owner,flag,enable));

	if (retVal)
	{
		if ((flag & SHVControlEdit::FlagReadonly))
		{
		gboolean oldSensitive;
		gboolean sensitive = ( enable ? FALSE : TRUE );
			g_object_get(G_OBJECT (TextView), "sensitive", &oldSensitive, NULL);
			if (oldSensitive != sensitive)
				g_object_set(G_OBJECT (TextView), "sensitive", sensitive, NULL);
		}

		if ((flag & SHVControlEdit::FlagFlat))
		{
			gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW (GetHandle()), enable ? GTK_SHADOW_NONE : GTK_SHADOW_ETCHED_IN);
		}
	}

	return retVal;
}

/*************************************
 * GetFlag
 *************************************/
bool SHVControlImplementerEditMultiGtk::GetFlag(SHVControl* owner, int flag)
{
bool retVal(SHVControlImplementerGtkWidget<SHVControlImplementerEdit>::GetFlag(owner,flag));
	
	if (retVal)
	{
		if ((flag & SHVControlEdit::FlagReadonly))
		{
		gboolean sensitive;
			g_object_get(G_OBJECT (TextView), "sensitive", &sensitive, NULL);
			retVal = retVal && (sensitive ? false : true);
		}

		if ((flag & SHVControlEdit::FlagFlat))
		{
			retVal = retVal && ( gtk_scrolled_window_get_shadow_type(GTK_SCROLLED_WINDOW (GetHandle())) == GTK_SHADOW_NONE );
		}
	}

	return retVal;
}

/*************************************
 * SetFont
 *************************************/
SHVBool SHVControlImplementerEditMultiGtk::SetFont(SHVControl* owner, SHVFont* font, bool resetHeight)
{
SHVBool retVal(SHVControlImplementerGtkWidget<SHVControlImplementerEdit>::SetFont(owner,font,resetHeight));
	
	if (retVal && font)
	{
	SHVFontGtk* gtkFont = (SHVFontGtk*)font;
		gtk_widget_modify_font(TextView,gtkFont->GetFont());
	}
	
	return retVal;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerEditMultiGtk::GetSubType(SHVControl* owner)
{
	SHVUNUSED_PARAM(owner);
	return SHVControlEdit::SubTypeMultiLine;
}

/*************************************
 * GetText
 *************************************/
SHVStringBuffer SHVControlImplementerEditMultiGtk::GetText()
{
SHVStringUTF8 retVal;

	if (IsCreated())
	{
	GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (TextView));
	GtkTextIter start, end;
		gtk_text_buffer_get_bounds(buffer,&start,&end);
		retVal = SHVStringBufferUTF8::Encapsulate(gtk_text_buffer_get_text(buffer,&start,&end,TRUE));
	}

#if __SHVSTRINGDEFAULT == utf8
	return retVal.ReleaseBuffer();
#else
	return retVal.ToStrT();
#endif
}

/*************************************
 * SetText
 *************************************/
void SHVControlImplementerEditMultiGtk::SetText(const SHVStringC& text)
{
	SHVASSERT(IsCreated());

	if (IsCreated())
	{
	GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (TextView));
		
		SuppressChangeEvent = true;
		gtk_text_buffer_set_text(buffer,text.AsStrUTF8C().GetSafeBuffer(),-1);
	}
}

/*************************************
 * GetLimit
 *************************************/
int SHVControlImplementerEditMultiGtk::GetLimit()
{
int retVal = -1;
	
	SHVASSERT(IsCreated());

	if (IsCreated())
	{
		// STUB
		retVal = 0xFFFFFF;
	}
	
	return retVal;
}

/*************************************
 * SetLimit
 *************************************/
void SHVControlImplementerEditMultiGtk::SetLimit(int limit)
{
	SHVASSERT(IsCreated());

	if (IsCreated())
	{
		// STUB
	}
}

/*************************************
 * SetHeight
 *************************************/
void SHVControlImplementerEditMultiGtk::SetHeight(SHVControlEdit* owner, int lines)
{
	SHVUNUSED_PARAM(owner);
	
	Lines = lines;

	if (IsCreated())
	{
	SHVFontRef font = GetFont(owner);
		SHVControlImplementerGtkWidget<SHVControlImplementerEdit>::SetFont(owner,font,true);
	}
}

/*************************************
 * SetSelection
 *************************************/
void SHVControlImplementerEditMultiGtk::SetSelection(SHVControlEdit* owner, int pos, SHVInt selectFrom, SHVControlEdit::ScrollModes scroll)
{
	SHVUNUSED_PARAM(owner);
	
	if (IsCreated())
	{
	GtkTextBuffer* buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW (TextView));
	GtkTextIter start, end;
	GtkTextMark* mark;
	gdouble align;
	gboolean doalign = FALSE;

		if ((mark = gtk_text_buffer_get_mark(buffer,"scrollmark")))
		{
			gtk_text_buffer_delete_mark(buffer, mark);
			mark = NULL;
		}
		gtk_text_buffer_get_iter_at_offset(buffer, &end, pos);
		mark = gtk_text_buffer_create_mark(buffer, "scrollmark", &end, TRUE);
		
		if (selectFrom.IsNull())
			gtk_text_buffer_get_iter_at_offset(buffer, &start, pos);
		else
			gtk_text_buffer_get_iter_at_offset(buffer, &start, selectFrom);
		
		if (scroll == SHVControlEdit::ScrollTop)
		{
			doalign = TRUE;
			align = 0.0;
		}
		else if (scroll == SHVControlEdit::ScrollBottom)
		{
			doalign = TRUE;
			align = 1.0;
		}

		gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW (TextView), mark, 0.0, doalign, 0.0, align);
		gtk_text_buffer_select_range(buffer, &start, &end);
	}
}

/*************************************
 * CalculateNewHeight
 *************************************/
int SHVControlImplementerEditMultiGtk::CalculateNewHeight(SHVControl* owner, SHVFont* font)
{
	return SHVControlImplementerGtkWidget<SHVControlImplementerEdit>::CalculateNewHeight(owner,font)
		+ font->GetCellHeight()*(Lines-1);
}

///\cond INTERNAL
/*************************************
 * on_changed
 *************************************/
void SHVControlImplementerEditMultiGtk::on_changed(GtkTextBuffer* widget, gpointer user_data)
{
SHVControlEdit* owner = (SHVControlEdit*)user_data;
SHVControlImplementerEditMultiGtk* self = (SHVControlImplementerEditMultiGtk*)owner->GetImplementor();
	SHVUNUSED_PARAM(widget);
	if (!self->SuppressChangeEvent)
		owner->PerformChanged();
	else
		self->SuppressChangeEvent = false;
}
///\endcond
