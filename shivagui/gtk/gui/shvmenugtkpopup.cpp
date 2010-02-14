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
#include "../../../include/utils/shvstringutf8.h"

#include "shvguimanagergtk.h"
#include "utils/shvfontgtk.h"

#include "shvgtk.h"


//=========================================================================================================
// SHVMenuGtkPopup
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVMenuGtkPopup::SHVMenuGtkPopup(SHVGUIManagerGtk* manager, SHVEventSubscriberBase* subscriber, SHVControl* parent)
	: Manager(manager), Subscriber(subscriber), Parent(parent)
{
	SHVASSERT(Parent->IsCreated());
	Menu = NULL;
	Shown = false;
	EnsureMenuCreated();
}

/*************************************
 * Destructor
 *************************************/
SHVMenuGtkPopup::~SHVMenuGtkPopup()
{
	Destroy();
}

/*************************************
 * Destroy
 *************************************/
void SHVMenuGtkPopup::Destroy()
{
	SubMenus.Clear();
	if (Menu)
	{
		gtk_widget_destroy(Menu);
		Menu = NULL;
	}
}

/*************************************
 * GetType
 *************************************/
SHVMenu::Types SHVMenuGtkPopup::GetType()
{
	return SHVMenu::TypePopup;
}

/*************************************
 * GetContainerMode
 *************************************/
SHVMenu::ContainerModes SHVMenuGtkPopup::GetContainerMode()
{
	return SHVMenu::ContainerInvalid;
}

/*************************************
 * GetCompactButtonCount
 *************************************/
SHVInt SHVMenuGtkPopup::GetCompactButtonCount()
{
	return SHVInt();
}

/*************************************
 * AddStringItem
 *************************************/
void SHVMenuGtkPopup::AddStringItem(SHVInt id, const SHVStringC name, int flags)
{
	EnsureMenuCreated();
	
	if (Menu)
	{
	GtkWidget* item = gtk_menu_item_new_with_label (name.ToStrUTF8().GetSafeBuffer());

		if (flags&FlagDisabled)
			g_object_set(G_OBJECT (item), "sensitive", FALSE, NULL);

		gtk_menu_append(GTK_MENU (Menu), item);
		RegisterIDMap(item,id);
		g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (SHVMenuGtkPopup::on_activate), this);
		gtk_widget_show(item);
	}
}

/*************************************
 * AddSeparator
 *************************************/
void SHVMenuGtkPopup::AddSeparator()
{
	EnsureMenuCreated();

	if (Menu)
	{
	GtkWidget* item = gtk_separator_menu_item_new();

		gtk_menu_append(GTK_MENU (Menu), item);
		gtk_widget_show(item);
	}
}

/*************************************
 * AddSubMenu
 *************************************/
SHVMenu* SHVMenuGtkPopup::AddSubMenu(const SHVStringC name)
{
SHVMenuGtkSub* retVal = NULL;
	EnsureMenuCreated();
	
	if (Menu)
	{
	GtkWidget* menuLabel;
		
		menuLabel = gtk_menu_item_new_with_label(name.ToStrUTF8().GetSafeBuffer());
		retVal = new SHVMenuGtkSub(Manager,this);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM (menuLabel), retVal->EnsureMenuCreated());
		
		gtk_menu_append(GTK_MENU (Menu), menuLabel);
		gtk_widget_show (menuLabel);
		
		SubMenus.Add(retVal);
	}
	
	return retVal;
}	

/*************************************
 * Show
 *************************************/
void SHVMenuGtkPopup::Show(PopupTypes type, SHVPoint offset)
{
GtkWidget* parent = Gtk::GetHandle(Parent);
	
	if (Menu && parent && !Shown)
	{
	GtkMenuPositionFunc func = ( type > SHVMenu::PopupMouse ? &SHVMenuGtkPopup::menu_position : NULL);
	gpointer data = (func ? this : NULL);
		
		Shown = true;
		ShowType = type;
		Offset = offset;
		
		Manager->ActivePopupMenus.Add(this);
		gtk_widget_show_all(Menu);
		gtk_menu_popup(GTK_MENU (Menu), NULL, NULL,
				func, data,
				/*button*/ 0,
				gtk_get_current_event_time());
	}
}

/*************************************
 * EmitEvent
 *************************************/
void SHVMenuGtkPopup::EmitEvent(SHVInt id)
{
SHVMenuRef refSelf = this; // make sure we aren't destroyed in this process
	if (!Subscriber.IsNull())
	{
		Subscriber->EmitNow(Manager->GetModuleList(),new SHVEvent(NULL,SHVControl::EventMenu,id,Parent));
	}
}

/*************************************
 * IsShown
 *************************************/
bool SHVMenuGtkPopup::IsShown()
{
	return Shown;
}

///\cond INTERNAL
/*************************************
 * EnsureMenuCreated
 *************************************/
void SHVMenuGtkPopup::EnsureMenuCreated()
{
GtkWidget* parent = Gtk::GetHandle(Parent);
	
	if (parent && !Menu)
	{
		Menu = gtk_menu_new();
		g_signal_connect (G_OBJECT (Menu), "selection-done", G_CALLBACK (SHVMenuGtkPopup::on_selection_done), this);
	}
}

/*************************************
 * on_activate
 *************************************/
void SHVMenuGtkPopup::on_activate(GtkMenuItem* menuitem, gpointer data)
{
SHVMenuGtkPopup* self = (SHVMenuGtkPopup*)data;
	self->EmitEventByMap(menuitem);
}

/*************************************
 * on_selection_done
 *************************************/
void SHVMenuGtkPopup::on_selection_done(GtkMenuShell* menushell, gpointer data)
{
SHVMenuGtkPopup* self = (SHVMenuGtkPopup*)data;
	
	SHVUNUSED_PARAM(menushell);
	
	for (size_t i=self->Manager->ActivePopupMenus.CalculateCount(); i>0;)
	{
		i--;
		if (self->Manager->ActivePopupMenus[i] == self)
		{
			self->Manager->ActivePopupMenus.Remove(i);
			self->Manager->ActivePopupMenus.Compress();
			break;
		}
	}
}

/*************************************
 * menu_position
 *************************************/
void SHVMenuGtkPopup::menu_position(GtkMenu* menu, gint* x, gint* y, gboolean* push_in, gpointer data)
{
SHVMenuGtkPopup* self = (SHVMenuGtkPopup*)data;
SHVRect rect(self->Parent->GetRect());
gint windowx = 0, windowy = 0;
gint menuwidth = 0, menuheight = 0;
GdkWindow* parentwindow = gtk_widget_get_window(Gtk::GetHandle(self->Parent));
	
	SHVUNUSED_PARAM(menu);
	*push_in = FALSE;
	
	// dialogs and the main window are easy to calculate the position on
	if (self->Parent->GetType() == SHVControl::TypeContainer && self->Parent->GetParent() == NULL)
	{
		gdk_window_get_origin(parentwindow,&windowx,&windowy);
	}
	// else try to figure out the screen offset of the child window
	else
	{
	SHVControl* parent = self->Parent;
	SHVRect parentRect(parent->GetRect());
	GdkWindow* rootwindow;
	gint tmpwindowx = 0, tmpwindowy = 0;
		windowx = parentRect.GetX();
		windowy = parentRect.GetY();
		for (parent = parent->GetParent();parent->GetParent();parent = parent->GetParent())
		{
			parentRect = parent->GetRect();
			windowx += parentRect.GetX();
			windowy += parentRect.GetY();
		}
		rootwindow = gtk_widget_get_window(Gtk::GetHandle(parent));
		gdk_window_get_origin(rootwindow,&tmpwindowx,&tmpwindowy);
		windowx += tmpwindowx;
		windowy += tmpwindowy;
	}
	
	switch (self->ShowType)
	{
	case SHVMenu::PopupBelowWindow:
		*x = windowx;
		*y = windowy + rect.GetHeight();
		break;
	case SHVMenu::PopupCenterWindow:
		///\todo Figure out a way to get the menu window size and properly center it
		*x = windowx + (rect.GetWidth()-menuwidth)/2;
		*y = windowy + (rect.GetHeight()-menuheight)/2;
		break;
	default:
		SHVASSERT(false); // shouldn't happen
		break;
	}
}
///\endcond
