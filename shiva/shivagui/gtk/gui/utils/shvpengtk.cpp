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

#include "shvpengtk.h"


//=========================================================================================================
// SHVColorGtk
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVPenGtk::SHVPenGtk(SHVColorGtk* color, int style, int width) : Color(color), Width(width)
{
	switch (style)
	{
	case StyleDefault:
	default:
		Pen = GDK_LINE_SOLID;
	}

	Color = color;
}

/*************************************
 * Destructor
 *************************************/
SHVPenGtk::~SHVPenGtk()
{
	Width = 0;
}

/*************************************
 * GetColor
 *************************************/
SHVColor* SHVPenGtk::GetColor()
{
	return Color;
}

/*************************************
 * Apply
 *************************************/
void SHVPenGtk::Apply(SHVPen* pen, GdkGC* GC, GdkGCValues& values)
{
	if (pen)
	{
	SHVPenGtk* self = (SHVPenGtk*)pen;
		gdk_gc_get_values(GC,&values);
		
		gdk_gc_set_line_attributes(GC,self->Width,self->Pen,values.cap_style,values.join_style);
		
		if (!self->Color.IsNull())
			gdk_gc_set_foreground(GC,self->Color->GetColor());
	}
}

/*************************************
 * Reset
 *************************************/
void SHVPenGtk::Reset(SHVPen* pen, GdkGC* GC, GdkGCValues& values)
{
	if (pen)
	{
	SHVPenGtk* self = (SHVPenGtk*)pen;
		
		gdk_gc_set_line_attributes(GC,values.line_width,values.line_style,values.cap_style,values.join_style);
		
		if (!self->Color.IsNull())
			gdk_gc_set_foreground(GC,&values.foreground);
	
		pen->ValidateRefCount();
	}
}
