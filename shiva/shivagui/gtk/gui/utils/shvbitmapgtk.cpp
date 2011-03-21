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

#include "../../../../include/platformspc.h"

#include "shvbitmapgtk.h"
#include "../shvgtk.h"


//=========================================================================================================
// SHVBitmapGtk
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVBitmapGtk::SHVBitmapGtk(SHVGUIManagerGtk* guiManager, char ** xpmResource) : SHVBitmap(), XpmResource(xpmResource)
{
	MainWnd = Gtk::GetMainWndHandle(guiManager->GetMainWindow());

	Bitmap = gdk_pixmap_create_from_xpm_d(gtk_widget_get_window(MainWnd),NULL,NULL,xpmResource);
	TransparencyMask = NULL;

	if (IsCreated())
	{
	gint w,h;
		gdk_drawable_get_size(Bitmap,&w,&h);
		Width = w;
		Height = h;
	}
}

/*************************************
 * Destructor
 *************************************/
SHVBitmapGtk::~SHVBitmapGtk()
{
	if (Bitmap)
		g_object_unref(Bitmap);
	if (TransparencyMask)
		g_object_unref(TransparencyMask);
}

/*************************************
 * IsCreated
 *************************************/
SHVBool SHVBitmapGtk::IsCreated()
{
	return (Bitmap ? SHVBool::True : SHVBool::False);
}

/*************************************
 * GetWidth
 *************************************/
SHVInt SHVBitmapGtk::GetWidth()
{
	return Width;
}

/*************************************
 * GetHeight
 *************************************/
SHVInt SHVBitmapGtk::GetHeight()
{
	return Height;
}

/*************************************
 * GetHeight
 *************************************/
bool SHVBitmapGtk::RealiseTransparentBitmap(SHVColor* color)
{
bool retVal = false;
	if (IsCreated() && color && (TransparentColor.IsNull() || color->GetRed() != TransparentColor->GetRed()
	                             || color->GetGreen() != TransparentColor->GetGreen()
	                             || color->GetBlue() != TransparentColor->GetBlue()))
	{
		if (Bitmap)
		{
			g_object_unref(Bitmap);
			Bitmap = NULL;
		}
		if (TransparencyMask)
		{
			g_object_unref(TransparencyMask);
			TransparencyMask = NULL;
		}
		
		TransparentColor = (SHVColorGtk*)color;
		Bitmap = gdk_pixmap_create_from_xpm_d(gtk_widget_get_window(MainWnd),&TransparencyMask,TransparentColor->GetColor(),XpmResource);
		retVal = (TransparencyMask ? true : false);
		
		gint w,h;
		gdk_drawable_get_size(TransparencyMask,&w,&h);
	}
	
	return retVal;
}
