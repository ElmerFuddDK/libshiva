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

#include "shvmenugtkcontainer.h"
#include "utils/shvfontgtk.h"

#include "shvgtk.h"


//=========================================================================================================
// SHVMenuGtkContainer
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVMenuGtkContainer::SHVMenuGtkContainer(SHVGUIManagerGtk* manager, SHVEventSubscriberBase* subscriber, SHVControlContainer* parent)
	: Manager(manager), Subscriber(subscriber), Parent(parent)
{
	SHVASSERT(Parent->IsCreated());
	MenuBar = NULL;
	Shown = false;
	EnsureMenuCreated();
}

/*************************************
 * Destructor
 *************************************/
SHVMenuGtkContainer::~SHVMenuGtkContainer()
{
	Destroy();
}

/*************************************
 * Destroy
 *************************************/
void SHVMenuGtkContainer::Destroy()
{
	SubMenus.Clear();
	if (MenuBar)
	{
		gtk_widget_destroy(MenuBar);
		MenuBar = NULL;
	}
}

/*************************************
 * GetType
 *************************************/
SHVMenu::Types SHVMenuGtkContainer::GetType()
{
	return SHVMenu::TypeControlContainer;
}

/*************************************
 * GetContainerMode
 *************************************/
SHVMenu::ContainerModes SHVMenuGtkContainer::GetContainerMode()
{
	return SHVMenu::ContainerNormal;
}

/*************************************
 * GetCompactButtonCount
 *************************************/
SHVInt SHVMenuGtkContainer::GetCompactButtonCount()
{
	return SHVInt();
}

/*************************************
 * AddStringItem
 *************************************/
void SHVMenuGtkContainer::AddStringItem(SHVInt id, const SHVStringC name, int flags)
{
	EnsureMenuCreated();
	
	if (MenuBar)
	{
	GtkWidget* item = gtk_menu_item_new_with_label (name.ToStrUTF8().GetSafeBuffer());

		if (flags&FlagDisabled)
			g_object_set(G_OBJECT (item), "sensitive", FALSE, NULL);

		gtk_menu_shell_append(GTK_MENU_SHELL (MenuBar), item);
		RegisterIDMap(item,id);
		g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (SHVMenuGtkContainer::on_activate), this);
		gtk_widget_show(item);
	}
}

/*************************************
 * AddSeparator
 *************************************/
void SHVMenuGtkContainer::AddSeparator()
{
	EnsureMenuCreated();

	if (MenuBar)
	{
	GtkWidget* item = gtk_separator_menu_item_new();

		gtk_menu_shell_append(GTK_MENU_SHELL (MenuBar), item);
		gtk_widget_show(item);
	}
}

/*************************************
 * AddSubMenu
 *************************************/
SHVMenu* SHVMenuGtkContainer::AddSubMenu(const SHVStringC name)
{
SHVMenuGtkSub* retVal = NULL;
	EnsureMenuCreated();
	
	if (MenuBar)
	{
	GtkWidget* menuLabel;
		
		menuLabel = gtk_menu_item_new_with_label(name.ToStrUTF8().GetSafeBuffer());
		retVal = new SHVMenuGtkSub(Manager,this);
		gtk_menu_item_set_submenu(GTK_MENU_ITEM (menuLabel), retVal->EnsureMenuCreated());
		
		gtk_menu_shell_append(GTK_MENU_SHELL (MenuBar), menuLabel);
		gtk_widget_show (menuLabel);
		
		SubMenus.Add(retVal);
	}
	
	return retVal;
}	

/*************************************
 * Show
 *************************************/
void SHVMenuGtkContainer::Show(PopupTypes type, SHVPoint offset)
{
GtkWidget* parent = Gtk::GetHandle(Parent);
	
	SHVUNUSED_PARAM(type);
	SHVUNUSED_PARAM(offset);
	
	if (MenuBar && parent && !Shown)
	{
		Shown = true;
		gtk_widget_show_all(MenuBar);
		if (Parent->GetFlag(SHVControl::FlagVisible))
			Parent->ResizeControls();
	}
}

/*************************************
 * EmitEvent
 *************************************/
void SHVMenuGtkContainer::EmitEvent(SHVInt id)
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
bool SHVMenuGtkContainer::IsShown()
{
	return Shown;
}

///\cond INTERNAL
/*************************************
 * EnsureMenuCreated
 *************************************/
void SHVMenuGtkContainer::EnsureMenuCreated()
{
GtkWidget* parent = Gtk::GetHandle(Parent);
	
	if (parent && !MenuBar)
	{
		MenuBar = gtk_menu_bar_new ();
		gtk_container_add(GTK_CONTAINER (parent), MenuBar);
		gtk_fixed_move(GTK_FIXED (parent), MenuBar, 0, 0);
		gtk_menu_bar_set_pack_direction(GTK_MENU_BAR (MenuBar), GTK_PACK_DIRECTION_LTR);
	}
}

/*************************************
 * GetHeight
 *************************************/
int SHVMenuGtkContainer::GetHeight()
{
gint w,h;
	gtk_widget_get_size_request(MenuBar,&w,&h);
	
	// If gtk doesn't provide a height we will set it to 15 LFU's
	if (h<0)
	{
	GtkStyle* style = gtk_widget_get_style(MenuBar);
		
		SHVASSERT(style); // should always be true since this shouldn't be called if we are not created
		
		if (style)
		{
		SHVFontGtkRef font;
	
			font = new SHVFontGtk(style->font_desc,gtk_widget_get_pango_context(MenuBar),false);
			h = font->LFUToHeight(15);
			gtk_widget_set_size_request(MenuBar,w,h);
		}
	}
	return h;
}

/*************************************
 * Move
 *************************************/
void SHVMenuGtkContainer::Move(SHVRect& rect)
{
gint w,h;
	gtk_widget_get_size_request(MenuBar,&w,&h);
	gtk_widget_set_size_request(MenuBar,rect.GetWidth(),h);
}

/*************************************
 * on_activate
 *************************************/
void SHVMenuGtkContainer::on_activate(GtkMenuItem* menuitem, gpointer data)
{
SHVMenuGtkContainer* self = (SHVMenuGtkContainer*)data;
	self->EmitEventByMap(menuitem);
}
///\endcond
