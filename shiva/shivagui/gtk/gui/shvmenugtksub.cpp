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

#include "shvmenugtkroot.h"

#include "shvgtk.h"


//=========================================================================================================
// SHVMenuGtkSub
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVMenuGtkSub::SHVMenuGtkSub(SHVGUIManagerGtk* manager, SHVMenuGtkRoot* rootMenu)
	: Manager(manager), RootMenu(rootMenu)
{
	Menu = NULL;
}

/*************************************
 * Destructor
 *************************************/
SHVMenuGtkSub::~SHVMenuGtkSub()
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
SHVMenu::Types SHVMenuGtkSub::GetType()
{
	return SHVMenu::TypeSub;
}

/*************************************
 * GetContainerMode
 *************************************/
SHVMenu::ContainerModes SHVMenuGtkSub::GetContainerMode()
{
	return SHVMenu::ContainerInvalid;
}

/*************************************
 * GetCompactButtonCount
 *************************************/
SHVInt SHVMenuGtkSub::GetCompactButtonCount()
{
	return SHVInt();
}

/*************************************
 * AddStringItem
 *************************************/
void SHVMenuGtkSub::AddStringItem(SHVInt id, const SHVStringC name, int flags)
{
	EnsureMenuCreated();
	
	if (Menu)
	{
	GtkWidget* item = gtk_menu_item_new_with_label (name.ToStrUTF8().GetSafeBuffer());

		if (flags&FlagDisabled)
			g_object_set(G_OBJECT (item), "sensitive", FALSE, NULL);

		gtk_menu_append(GTK_MENU (Menu), item);
		RootMenu->RegisterIDMap(item,id);
		g_signal_connect (G_OBJECT (item), "activate", G_CALLBACK (SHVMenuGtkSub::on_activate), this);
		gtk_widget_show(item);
	}
}

/*************************************
 * AddSeparator
 *************************************/
void SHVMenuGtkSub::AddSeparator()
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
SHVMenu* SHVMenuGtkSub::AddSubMenu(const SHVStringC name)
{
SHVMenuGtkSub* retVal = NULL;
	EnsureMenuCreated();
	
	if (Menu)
	{
	GtkWidget* menuLabel;
		
		menuLabel = gtk_menu_item_new_with_label(name.ToStrUTF8().GetSafeBuffer());
		retVal = new SHVMenuGtkSub(Manager,RootMenu);
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
void SHVMenuGtkSub::Show(PopupTypes type, SHVPoint offset)
{
	SHVUNUSED_PARAM(type);
	SHVUNUSED_PARAM(offset);
	SHVASSERT(false); // This is not a popup menu - cannot be shown
}

/*************************************
 * EmitEvent
 *************************************/
void SHVMenuGtkSub::EmitEvent(SHVInt id)
{
SHVMenuRef refSelf = this; // make sure we aren't destroyed in this process
	RootMenu->EmitEvent(id);
}

///\cond INTERNAL
/*************************************
 * EnsureMenuCreated
 *************************************/
GtkWidget* SHVMenuGtkSub::EnsureMenuCreated()
{
	if (!Menu)
	{
		Menu = gtk_menu_new();
	}
	return Menu;
}

/*************************************
 * on_activate
 *************************************/
void SHVMenuGtkSub::on_activate(GtkMenuItem* menuitem, gpointer data)
{
SHVMenuGtkSub* self = (SHVMenuGtkSub*)data;
	self->RootMenu->EmitEventByMap(menuitem);
}
///\endcond
