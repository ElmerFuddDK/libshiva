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
#include "utils/shvdrawgtk.h"


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
			g_signal_connect (G_OBJECT (GetHandle()), "expose-event",
							  G_CALLBACK (SHVControlImplementerLabelGtk::expose_event), owner);
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

///\cond INTERNAL
/*************************************
 * expose_event
 *************************************/
gboolean SHVControlImplementerLabelGtk::expose_event(GtkWidget *widget, GdkEvent* event, gpointer user_data)
{
SHVControl* owner = (SHVControl*)user_data;
SHVControlImplementerLabelGtk* self = (SHVControlImplementerLabelGtk*)owner->GetImplementor();
GdkEventExpose* exposeEvent = (GdkEventExpose*)event;
SHVDrawGtkRef draw = new SHVDrawGtk(owner->GetManager(),gdk_gc_new(GDK_DRAWABLE (exposeEvent->window)),exposeEvent->window, owner);

	if (!self->Subscriber.IsNull())
	{
	SHVControlRef refToSelf = owner; // ensure the validity of the object through this function
		self->Subscriber->EmitNow(owner->GetModuleList(),new SHVEventData<SHVDrawEventData>(SHVDrawEventData(draw),NULL,SHVControl::EventDraw,NULL,owner));
	}
	else
	{
		draw->DrawText(self->GetText(),draw->GetClientRect(owner),NULL,SHVDraw::TextVCenter|SHVDraw::TextLeft|SHVDraw::TextEndEllipsis);
	}
	
	return TRUE;
}
///\endcond
