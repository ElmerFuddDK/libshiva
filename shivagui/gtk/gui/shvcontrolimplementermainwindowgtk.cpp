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

#include "../../../include/platformspc.h"

#include "shvcontrolimplementermainwindowgtk.h"
#include "shvmainthreadeventdispatchergtk.h"
#include "shvgtk.h"
#include "utils/shvfontgtk.h"
#include "utils/shvcolorgtk.h"



//=========================================================================================================
// SHVControlImplementerMainWindowGtk - Main window implementation
//=========================================================================================================


/*************************************
 * Constructor
 *************************************/
SHVControlImplementerMainWindowGtk::SHVControlImplementerMainWindowGtk(SHVMainThreadEventDispatcherGtk* dispatcher) : SHVControlImplementerContainer()
{
	Dispatcher = dispatcher;
	Handle = NULL;
	MainWindow = NULL;
	Visible = false;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerMainWindowGtk::GetSubType(SHVControl* owner)
{
	SHVUNUSED_PARAM(owner);
	return SHVControlContainer::SubTypeMainWindow;
}

/*************************************
 * IsCreated
 *************************************/
SHVBool SHVControlImplementerMainWindowGtk::IsCreated()
{
	return (Handle ? SHVBool::True : SHVBool::False);
}

/*************************************
 * Create(parent)
 *************************************/
SHVBool SHVControlImplementerMainWindowGtk::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
SHVBool retVal(!IsCreated() && parent == NULL);

	if (retVal)
	{
		MainWindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	
		gtk_window_set_default_size (GTK_WINDOW (MainWindow), 0, 0);
		gtk_window_set_resizable (GTK_WINDOW (MainWindow), FALSE);
		
		Handle = gtk_fixed_new();
		gtk_widget_set_size_request (Handle, 300, 240);
		gtk_container_add (GTK_CONTAINER (MainWindow), Handle);
		
		g_signal_connect (G_OBJECT (MainWindow), "size-allocate",
						  G_CALLBACK (SHVControlImplementerMainWindowGtk::on_size_allocate), owner);
		
		if (!Color.IsNull())
		{
			gtk_widget_modify_bg(MainWindow,GTK_STATE_NORMAL,SHVColorGtk::GetNative(Color));
		}
		
		SetFlag(owner,flags,true);
	}
	
	return retVal;
}

/*************************************
 * Reparent
 *************************************/
SHVBool SHVControlImplementerMainWindowGtk::Reparent(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	SHVUNUSED_PARAM(owner);
	SHVUNUSED_PARAM(parent);
	SHVUNUSED_PARAM(flags);
	return SHVBool::False;
}

/*************************************
 * Destroy
 *************************************/
SHVBool SHVControlImplementerMainWindowGtk::Destroy(SHVControl* owner)
{
	SHVUNUSED_PARAM(owner);
	return !IsCreated();
}

/*************************************
 * GetRect
 *************************************/
SHVRect SHVControlImplementerMainWindowGtk::GetRect(SHVControl* owner)
{
SHVRect rect;
	SHVUNUSED_PARAM(owner);
	if (IsCreated())
	{
	int width,height;
	gboolean resizable = gtk_window_get_resizable (GTK_WINDOW (MainWindow));
		
		if (resizable)
			gtk_window_get_size(GTK_WINDOW(MainWindow),&width,&height);
		else
			gtk_widget_get_size_request (Handle, &width,&height);
		rect.SetWidth(width);
		rect.SetHeight(height);
	}
	return rect;
}

/*************************************
 * SetRect
 *************************************/
void SHVControlImplementerMainWindowGtk::SetRect(SHVControl* owner, const SHVRect& rect)
{
	SHVUNUSED_PARAM(owner);
	if (IsCreated())
	{
	gboolean resizable = gtk_window_get_resizable (GTK_WINDOW (MainWindow));
		
		if (resizable)
			gtk_window_resize (GTK_WINDOW(MainWindow), rect.GetWidth(), rect.GetHeight());
		else
			gtk_widget_set_size_request (Handle, rect.GetWidth(), rect.GetHeight());
	}
}

/*************************************
 * SetFlag
 *************************************/
SHVBool SHVControlImplementerMainWindowGtk::SetFlag(SHVControl* owner, int flag, bool enable)
{
SHVBool retVal(IsCreated());
	SHVUNUSED_PARAM(owner);

	if (retVal && (flag & SHVControl::FlagVisible))
	{
		if (enable && !Visible)
		{
			Visible = true;
			gtk_widget_show(Handle);
			gtk_widget_show_now(MainWindow);
		}
	}

	return retVal;
}

/*************************************
 * GetFlag
 *************************************/
bool SHVControlImplementerMainWindowGtk::GetFlag(SHVControl* owner, int flag)
{
bool retVal(IsCreated());
	SHVUNUSED_PARAM(owner);

	if (retVal && (flag & SHVControl::FlagVisible))
	{
		retVal = Visible;
	}

	return retVal;
}

/*************************************
 * GetFont
 *************************************/
SHVFont* SHVControlImplementerMainWindowGtk::GetFont(SHVControl* owner)
{
	SHVUNUSED_PARAM(owner);
	if (Font.IsNull())
	{
	GtkStyle* style = gtk_widget_get_style(Handle);

		// We can use the context from the main window since it will always exist during the programs lifetime
		Font = new SHVFontGtk(style->font_desc,gtk_widget_get_pango_context(Handle),false);
	}
	return Font;
}

/*************************************
 * SetFont
 *************************************/
SHVBool SHVControlImplementerMainWindowGtk::SetFont(SHVControl* owner, SHVFont* font, bool resetHeight)
{
	SHVUNUSED_PARAM(owner);
	SHVUNUSED_PARAM(resetHeight);
	if (IsCreated())
	{
		Font = (SHVFontGtk*)font;
		gtk_widget_modify_font(Handle,Font->GetFont());
	}
	font->ValidateRefCount();
	return IsCreated();
}

/*************************************
 * GetNative
 *************************************/
void* SHVControlImplementerMainWindowGtk::GetNative()
{
	return Handle;
}

/*************************************
 * GetMainWndHandle
 *************************************/
GtkWidget* SHVControlImplementerMainWindowGtk::GetMainWndHandle()
{
	return MainWindow;
}

/*************************************
 * GetRegionRect
 *************************************/
SHVRect SHVControlImplementerMainWindowGtk::GetRegionRect()
{
SHVRect rect;
	if (IsCreated())
	{
	int width,height;
	gboolean resizable = gtk_window_get_resizable (GTK_WINDOW (MainWindow));
		
		if (resizable)
			gtk_window_get_size(GTK_WINDOW(MainWindow),&width,&height);
		else
			gtk_widget_get_size_request (Handle, &width,&height);
		rect.SetWidth(width);
		rect.SetHeight(height);
	}
	return rect;
}

/*************************************
 * SetSize
 *************************************/
void SHVControlImplementerMainWindowGtk::SetSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels, SHVControlContainer::PosModes mode)
{
	SHVUNUSED_PARAM(owner);
	SHVUNUSED_PARAM(mode);
	if (IsCreated())
	{
	gboolean resizable = gtk_window_get_resizable (GTK_WINDOW (MainWindow));
		
		if (resizable)
			gtk_window_resize (GTK_WINDOW(MainWindow), widthInPixels, heightInPixels);
		else
			gtk_widget_set_size_request (Handle, widthInPixels, heightInPixels);
	}
}

/*************************************
 * GetTitle
 *************************************/
SHVStringBuffer SHVControlImplementerMainWindowGtk::GetTitle(SHVControlContainer* control)
{
SHVString retVal;

	SHVUNUSED_PARAM(control);

	if (IsCreated())
	{
		retVal = SHVStringC(gtk_window_get_title(GTK_WINDOW (MainWindow)));
	}

	return retVal.ReleaseBuffer();
}

/*************************************
 * SetTitle
 *************************************/
void SHVControlImplementerMainWindowGtk::SetTitle(SHVControlContainer* control, const SHVStringC& title)
{
	SHVUNUSED_PARAM(control);
	
	if (IsCreated())
	{
		gtk_window_set_title (GTK_WINDOW (MainWindow), title.GetSafeBuffer());
	}
}

/*************************************
 * GetColor
 *************************************/
SHVColor* SHVControlImplementerMainWindowGtk::GetColor(SHVControlContainer* owner)
{
	SHVUNUSED_PARAM(owner);
	return Color;
}

/*************************************
 * SetColor
 *************************************/
void SHVControlImplementerMainWindowGtk::SetColor(SHVControlContainer* owner, SHVColor* color)
{
	SHVUNUSED_PARAM(owner);
	Color = (SHVColorGtk*)color;

	if (IsCreated())
	{
		gtk_widget_modify_bg(MainWindow,GTK_STATE_NORMAL,SHVColorGtk::GetNative(Color));
	}
}

/*************************************
 * SetMinimumSize
 *************************************/
void SHVControlImplementerMainWindowGtk::SetMinimumSize(SHVControlContainer* owner, int widthInPixels, int heightInPixels)
{
gboolean resizable = gtk_window_get_resizable (GTK_WINDOW (MainWindow));
	SHVUNUSED_PARAM(owner);
	if (resizable)
		gtk_widget_set_size_request(MainWindow,widthInPixels,heightInPixels);
	else
		gtk_window_set_default_size(GTK_WINDOW (MainWindow), widthInPixels, heightInPixels);
}

/*************************************
 * GetMinimumSizeInPixels
 *************************************/
SHVPoint SHVControlImplementerMainWindowGtk::GetMinimumSizeInPixels(SHVControlContainer* owner)
{
gint w,h;
gboolean resizable = gtk_window_get_resizable (GTK_WINDOW (MainWindow));
	SHVUNUSED_PARAM(owner);
	if (resizable)
		gtk_widget_get_size_request(MainWindow,&w,&h);
	else
		gtk_window_get_default_size(GTK_WINDOW (MainWindow), &w,&h);
	return SHVPoint(w,h);
}

/*************************************
 * SetResizable
 *************************************/
void SHVControlImplementerMainWindowGtk::SetResizable(bool resizable)
{
int width,height;
int minWidth,minHeight;
gboolean oldResizable = gtk_window_get_resizable(GTK_WINDOW (MainWindow));
	
	if (oldResizable == resizable)
		return;
	
	if (oldResizable)
	{
		gtk_window_get_size(GTK_WINDOW(MainWindow),&width,&height);
		gtk_widget_get_size_request(MainWindow,&minWidth,&minHeight);
	}
	else
	{
		gtk_widget_get_size_request (Handle, &width,&height);
		gtk_window_get_default_size(GTK_WINDOW (MainWindow), &minWidth,&minHeight);
	}
	
	if (!resizable)
	{
		gtk_widget_set_size_request (Handle, width, height);
		gtk_window_set_default_size(GTK_WINDOW (MainWindow),minWidth,minHeight);
	}
	
	gtk_window_set_resizable (GTK_WINDOW (MainWindow), resizable);
	
	if (resizable)
	{
		gtk_widget_set_size_request (Handle, minWidth, minHeight);
		gtk_window_resize (GTK_WINDOW(MainWindow), width, height);
	}
}

///\cond INTERNAL
/*************************************
 * on_size_allocate
 *************************************/
void SHVControlImplementerMainWindowGtk::on_size_allocate(GtkWidget * widget, GtkAllocation *allocation, gpointer data)
{
SHVControlContainer* owner = (SHVControlContainer*)data;
SHVControlImplementerMainWindowGtk* self = (SHVControlImplementerMainWindowGtk*)owner->GetImplementor();

	SHVUNUSED_PARAM(allocation);
	
	if (self->MainWindow && self->MainWindow == widget)
	{
	SHVRect newRect(self->GetRect(NULL));
		if (newRect != self->SizedRect)
		{
			self->SizedRect = newRect;
			owner->ResizeControls();
		}
	}
}
///\endcond
