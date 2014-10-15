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

#include "shvcontrolimplementercontainerwindowgtk.h"
#include "shvgtk.h"
#include "utils/shvfontgtk.h"
#include "utils/shvcolorgtk.h"
#include "utils/shvdrawgtk.h"
#include "../../../include/framework/shveventdata.h"



//=========================================================================================================
// SHVControlImplementerContainerWindowGtk
//=========================================================================================================


/*************************************
 * Constructor
 *************************************/
SHVControlImplementerContainerWindowGtk::SHVControlImplementerContainerWindowGtk(int subType) : SHVControlImplementerGtkWidget<SHVControlImplementerContainerCustomDraw>()
{
	SubType = subType;
}

/*************************************
 * Destructor
 *************************************/
SHVControlImplementerContainerWindowGtk::~SHVControlImplementerContainerWindowGtk()
{
	SHVASSERT(!IsCreated());
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerContainerWindowGtk::GetSubType(SHVControl* owner)
{
	SHVUNUSED_PARAM(owner);
	return SubType;
}

/*************************************
 * Create(parent)
 *************************************/
SHVBool SHVControlImplementerContainerWindowGtk::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && ((SubType == SHVControlContainer::SubTypeTabPage && !parent) || (parent && parent->IsCreated())))
	{
		SetHandle(gtk_fixed_new());
		if (parent)
			gtk_container_add(GTK_CONTAINER (parent->GetNative()), GetHandle());
		
		g_signal_connect (G_OBJECT (GetHandle()), "size-allocate",
						  G_CALLBACK (SHVControlImplementerContainerWindowGtk::on_size_allocate), owner);
		g_signal_connect (G_OBJECT (GetHandle()), "delete-event",
						  G_CALLBACK (SHVControlImplementerContainerWindowGtk::on_delete_event), owner);
		g_signal_connect (G_OBJECT (GetHandle()), "expose-event",
						G_CALLBACK (SHVControlImplementerContainerWindowGtk::expose_event), owner);
		
		owner->SetFont(NULL,true);
		owner->SetFlag(flags);
		
		return IsCreated();
	}
	
	return SHVBool::False;
}

/*************************************
 * Destroy
 *************************************/
SHVBool SHVControlImplementerContainerWindowGtk::Destroy(SHVControl* owner)
{
	if (IsCreated())
		((SHVControlContainer*)owner)->Clear();
	return SHVControlImplementerGtkWidget<SHVControlImplementerContainerCustomDraw>::Destroy(owner);
}

/*************************************
 * GetRegionRect
 *************************************/
SHVRect SHVControlImplementerContainerWindowGtk::GetRegionRect()
{
SHVRect rect;
	if (IsCreated())
	{
	int width,height;
		gtk_widget_get_size_request (GetHandle(), &width,&height);
		rect.SetWidth(width);
		rect.SetHeight(height);
	}
	return rect;
}

/*************************************
 * SetSize
 *************************************/
void SHVControlImplementerContainerWindowGtk::SetSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels, SHVControlContainer::PosModes mode)
{
	SHVUNUSED_PARAM(owner);
	SHVUNUSED_PARAM(mode);
	if (IsCreated())
	{
		gtk_widget_set_size_request (GetHandle(), widthInPixels, heightInPixels);
	}
}

/*************************************
 * GetTitle
 *************************************/
SHVStringBuffer SHVControlImplementerContainerWindowGtk::GetTitle(SHVControlContainer* control)
{
SHVString retVal;

	SHVUNUSED_PARAM(control);

	if (IsCreated())
	{
		retVal = Title;
	}

	return retVal.ReleaseBuffer();
}

/*************************************
 * SetTitle
 *************************************/
void SHVControlImplementerContainerWindowGtk::SetTitle(SHVControlContainer* control, const SHVStringC& title)
{
	SHVUNUSED_PARAM(control);
	
	if (IsCreated())
	{
		Title = title;
	}
}

/*************************************
 * GetColor
 *************************************/
SHVColor* SHVControlImplementerContainerWindowGtk::GetColor(SHVControlContainer* owner)
{
	SHVUNUSED_PARAM(owner);
	return Color;
}

/*************************************
 * SetColor
 *************************************/
void SHVControlImplementerContainerWindowGtk::SetColor(SHVControlContainer* owner, SHVColor* color)
{
	SHVUNUSED_PARAM(owner);
	
	Color = (SHVColorGtk*)color;
	
	if (IsCreated())
		gtk_widget_queue_draw(GetHandle());
}

/*************************************
 * SetMinimumSize
 *************************************/
void SHVControlImplementerContainerWindowGtk::SetMinimumSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels)
{
	SHVUNUSED_PARAM(owner);
	MinSize.x = widthInPixels;
	MinSize.y = heightInPixels;
}

/*************************************
 * GetMinimumSizeInPixels
 *************************************/
SHVPoint SHVControlImplementerContainerWindowGtk::GetMinimumSizeInPixels(SHVControlContainer* owner)
{
	SHVUNUSED_PARAM(owner);
	return MinSize;
}

/*************************************
 * SetResizable
 *************************************/
void SHVControlImplementerContainerWindowGtk::SetResizable(bool resizable)
{
	SHVUNUSED_PARAM(resizable);
}

/*************************************
 * SetResizable
 *************************************/
void SHVControlImplementerContainerWindowGtk::SubscribeDraw(SHVEventSubscriberBase* subscriber)
{
	Subscriber = subscriber;

	if (IsCreated())
		gtk_widget_queue_draw(GetHandle());
}


///\cond INTERNAL
/*************************************
 * on_size_allocate
 *************************************/
void SHVControlImplementerContainerWindowGtk::on_size_allocate(GtkWidget * widget, GtkAllocation *allocation, gpointer data)
{
SHVControlContainer* owner = (SHVControlContainer*)data;
SHVControlImplementerContainerWindowGtk* self = (SHVControlImplementerContainerWindowGtk*)owner->GetImplementor();

	if (self->GetHandle() && self->GetHandle() == widget)
	{
	SHVRect newRect(self->SubType == SHVControlContainer::SubTypeTabPage
					? SHVRect(allocation->x,allocation->y,allocation->x+allocation->width,allocation->y+allocation->height)
					: self->GetRect(owner));
		if (newRect != self->SizedRect)
		{
			self->SizedRect = newRect;
			owner->ResizeControls();
		}
	}
}

/*************************************
 * on_delete_event
 *************************************/
gboolean SHVControlImplementerContainerWindowGtk::on_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
SHVControlContainerRef owner = (SHVControlContainer*)data;
//SHVControlImplementerContainerWindowGtk* self = (SHVControlImplementerContainerWindowGtk*)owner->GetImplementor();
gboolean retVal = FALSE;
	
	SHVUNUSED_PARAM(widget);
	SHVUNUSED_PARAM(event);
	
	if (owner->PreDestroy())
	{
		owner->Close();
		//retVal = TRUE;
	}
	
	return retVal;
}

/*************************************
 * expose_event
 *************************************/
gboolean SHVControlImplementerContainerWindowGtk::expose_event(GtkWidget *widget, GdkEvent* event, gpointer user_data)
{
SHVControlContainer* owner = (SHVControlContainer*)user_data;
SHVControlImplementerContainerWindowGtk* self = (SHVControlImplementerContainerWindowGtk*)owner->GetImplementor();
GdkEventExpose* exposeEvent = (GdkEventExpose*)event;
SHVDrawGtkRef draw = new SHVDrawGtk(owner->GetManager(),gdk_gc_new(GDK_DRAWABLE (exposeEvent->window)),exposeEvent->window, owner);

	SHVUNUSED_PARAM(widget);
	
	if (!self->Subscriber.IsNull())
	{
	SHVControlRef refToSelf = owner; // ensure the validity of the object through this function
		self->Subscriber->EmitNow(owner->GetModuleList(),new SHVEventData<SHVDrawEventData>(SHVDrawEventData(draw),NULL,SHVControl::EventDraw,NULL,owner));
	}
	else if (!self->Color.IsNull())
	{
		draw->DrawRectFilled(draw->GetClientRect(owner),self->Color);
	}
	
	return FALSE;
}
///\endcond
