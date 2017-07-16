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

#include "shvdrawgtk.h"
#include "shvpengtk.h"
#include "../../../../include/gui/shvguimanager.h"
#include "shvbitmapgtk.h"
#include "../shvgtk.h"



//=========================================================================================================
// SHVDrawGtk - drawing class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVDrawGtk::SHVDrawGtk(SHVGUIManager* manager, GdkGC* gc, GdkWindow* window, SHVControl* control)
: SHVDraw(manager), Control(control), Widget(Gtk::GetHandle(control)), WindowArea(control->GetRect())
{
SHVControlContainer* parent = control->GetParent();
SHVRect parentRect;

	// Adjust for the offset caused by any window containers
	while (parent)
	{
		switch(parent->GetImplementor()->GetSubType(parent))
		{
		case SHVControlContainer::SubTypeWindow:
		case SHVControlContainer::SubTypeCustomDraw:
		case SHVControlContainer::SubTypeTabPage:
			parentRect = parent->GetRect();
			WindowArea.MoveBy(parentRect.GetX(),parentRect.GetY());
			parent = parent->GetParent();
			break;
		default:
			parent = NULL;
		}
	}

	GC = gc;
	Window = window;
	SetClipRect();
}

/*************************************
 * Destructor
 *************************************/
SHVDrawGtk::~SHVDrawGtk()
{
	gdk_gc_set_clip_rectangle(GC,NULL);
	CloseDC();
}

/*************************************
 * GetClientRect
 *************************************/
SHVRect SHVDrawGtk::GetClientRect(SHVControl* control)
{
SHVRect rect;
	
	SHVUNUSED_PARAM(control);
	
	rect.SetWidth(WindowArea.GetWidth());
	rect.SetHeight(WindowArea.GetHeight());

	return rect;
}

/*************************************
 * DrawLine
 *************************************/
void SHVDrawGtk::DrawLine(SHVPoint from, SHVPoint to, SHVColor* color)
{
GdkGCValues values;
	from.Move(WindowArea.GetX(),WindowArea.GetY());
	to.Move(WindowArea.GetX(),WindowArea.GetY());
	gdk_gc_get_values(GC,&values);
	if (color)
		gdk_gc_set_foreground(GC,SHVDrawGtk::GetColor(color));
	gdk_draw_line(GDK_DRAWABLE (Window),GC,from.x,from.y,to.x,to.y);
	if (color)
		color->ValidateRefCount();
	gdk_gc_set_foreground(GC,&values.foreground);
}
void SHVDrawGtk::DrawLine(SHVPoint from, SHVPoint to, SHVPen* pen)
{
GdkGCValues values;
	
	from.Move(WindowArea.GetX(),WindowArea.GetY());
	to.Move(WindowArea.GetX(),WindowArea.GetY());
	
	SHVPenGtk::Apply(pen,GC,values);
	gdk_draw_line(GDK_DRAWABLE (Window),GC,from.x,from.y,to.x,to.y);
	SHVPenGtk::Reset(pen,GC,values);
}

/*************************************
 * DrawXORLine
 *************************************/
void SHVDrawGtk::DrawXORLine(SHVPoint from, SHVPoint to)
{
GdkGCValues values;
SHVColorRef color = GUIManager->CreateColor(0xFF,0xFF,0xFF);
	from.Move(WindowArea.GetX(),WindowArea.GetY());
	to.Move(WindowArea.GetX(),WindowArea.GetY());
	gdk_gc_set_function(GC,GDK_XOR);
	gdk_gc_get_values(GC,&values);
	gdk_gc_set_foreground(GC,SHVDrawGtk::GetColor(color));
	gdk_draw_line(GDK_DRAWABLE (Window),GC,from.x,from.y,to.x,to.y);
	gdk_gc_set_function(GC,GDK_COPY);
	gdk_gc_set_foreground(GC,&values.foreground);
}

/*************************************
 * DrawPixel
 *************************************/
void SHVDrawGtk::DrawPixel(SHVPoint where, SHVColor* color)
{
GdkGCValues values;
	where.Move(WindowArea.GetX(),WindowArea.GetY());
	gdk_gc_get_values(GC,&values);
	gdk_gc_set_foreground(GC,SHVDrawGtk::GetColor(color));
	gdk_draw_point(GDK_DRAWABLE (Window),GC,where.x,where.y);
	if (color)
		color->ValidateRefCount();
	gdk_gc_set_foreground(GC,&values.foreground);
}

/*************************************
 * DrawRect
 *************************************/
void SHVDrawGtk::DrawRect(SHVRect rect, SHVColor* color)
{
GdkGCValues values;
	
	gdk_gc_get_values(GC,&values);
	gdk_gc_set_foreground(GC,SHVDrawGtk::GetColor(color));

	rect.MoveBy(WindowArea.GetX(), WindowArea.GetY());
	
	gdk_draw_rectangle(GDK_DRAWABLE (Window), GC, FALSE, rect.GetX(),rect.GetY(),rect.GetWidth(),rect.GetHeight());

	if (color)
		color->ValidateRefCount();

	gdk_gc_set_foreground(GC,&values.foreground);
}

/*************************************
 * DrawEdgeAndShrink
 *************************************/
void SHVDrawGtk::DrawEdgeAndShrink(SHVRect& rect, SHVDraw::EdgeTypes type, int flags)
{
/*
RECT winRect(MapRect(rect));
UINT winType, winFlags;

	winFlags = BF_ADJUST;

	switch (type)
	{
	default:
		SHVASSERT(false); // unknown type
	case SHVDraw::EdgeTypeFrame:
		winType = EDGE_ETCHED;
		break;
	case SHVDraw::EdgeTypeRaised:
		winType = ( flags&SHVDraw::EdgeFlagThick ? EDGE_RAISED : BDR_RAISEDINNER );
		break;
	case SHVDraw::EdgeTypeSunken:
		winType = ( flags&SHVDraw::EdgeFlagThick ? EDGE_SUNKEN : BDR_SUNKENOUTER );
		break;
	case SHVDraw::EdgeTypeFlat:
		winType = ( flags&SHVDraw::EdgeFlagThick ? EDGE_RAISED : BDR_RAISEDINNER );
		winFlags |= BF_FLAT;
	}

	if (flags&SHVDraw::EdgeFlagLeft)
		winFlags |= BF_LEFT;
	if (flags&SHVDraw::EdgeFlagTop)
		winFlags |= BF_TOP;
	if (flags&SHVDraw::EdgeFlagRight)
		winFlags |= BF_RIGHT;
	if (flags&SHVDraw::EdgeFlagBottom)
		winFlags |= BF_BOTTOM;

	::DrawEdge(hDC,&winRect,winType,winFlags);

	rect = MapRect(winRect);
	*/
}

/*************************************
 * DrawRectFilled
 *************************************/
void SHVDrawGtk::DrawRectFilled(SHVRect rect, SHVColor* color)
{
GdkGCValues values;
	
	gdk_gc_get_values(GC,&values);
	gdk_gc_set_foreground(GC,SHVDrawGtk::GetColor(color));
	
	rect.MoveBy(WindowArea.GetX(), WindowArea.GetY());

	gdk_draw_rectangle(GDK_DRAWABLE (Window), GC, TRUE, rect.GetX(),rect.GetY(),rect.GetWidth(),rect.GetHeight());

	if (color)
		color->ValidateRefCount();

	gdk_gc_set_foreground(GC,&values.foreground);
}

/*************************************
 * DrawXORRect
 *************************************/
void SHVDrawGtk::DrawXORRect(SHVRect rect)
{
GdkGCValues values;
SHVColorRef color = GUIManager->CreateColor(0xFF,0xFF,0xFF);
	rect.MoveBy(WindowArea.GetX(), WindowArea.GetY());
	gdk_gc_set_function(GC,GDK_XOR);
	gdk_gc_get_values(GC,&values);
	gdk_gc_set_foreground(GC,SHVDrawGtk::GetColor(color));
	
	// This doesn't work since the first pixel is XOR'ed twice
	//gdk_draw_rectangle(GDK_DRAWABLE (Window), GC, FALSE, rect.GetX(),rect.GetY(),rect.GetWidth(),rect.GetHeight());
	
	gdk_draw_line(GDK_DRAWABLE (Window),GC, rect.GetLeft(), rect.GetTop(), rect.GetRight()-1, rect.GetTop());
	gdk_draw_line(GDK_DRAWABLE (Window),GC, rect.GetRight(), rect.GetTop(), rect.GetRight(), rect.GetBottom()-1);
	gdk_draw_line(GDK_DRAWABLE (Window),GC, rect.GetRight(), rect.GetBottom(), rect.GetLeft()+1, rect.GetBottom());
	gdk_draw_line(GDK_DRAWABLE (Window),GC, rect.GetLeft(), rect.GetBottom(), rect.GetLeft(), rect.GetTop()+1);
	
	gdk_gc_set_function(GC,GDK_COPY);
	gdk_gc_set_foreground(GC,&values.foreground);
}

/*************************************
 * DrawPolyline
 *************************************/
void SHVDrawGtk::DrawPolyline(SHVColor* color, int points, ...)
{
GdkGCValues values;
GdkPoint* pointArray = (GdkPoint*)::malloc(sizeof(GdkPoint)*points);
SHVPoint point;
SHVVA_LIST args;
	
	gdk_gc_get_values(GC,&values);
	if (color)
		gdk_gc_set_foreground(GC,SHVDrawGtk::GetColor(color));

	SHVVA_START(args,points);
	for (int i=0; i<points;i++)
	{
		point = SHVVA_ARG(args,SHVPointVal);
		point.Move(WindowArea.GetX(),WindowArea.GetY());
		pointArray[i].x = point.x;
		pointArray[i].y = point.y;
	}
	SHVVA_END(args);
	
	gdk_draw_lines(GDK_DRAWABLE (Window), GC, pointArray, points);
	
	if (color)
		color->ValidateRefCount();
	
	gdk_gc_set_foreground(GC,&values.foreground);
	
	::free(pointArray);
}

/*************************************
 * DrawPolygon
 *************************************/
void SHVDrawGtk::DrawPolygon(SHVColor* color, int points, ...)
{
GdkGCValues values;
GdkPoint* pointArray = (GdkPoint*)::malloc(sizeof(GdkPoint)*points);
SHVPoint point;
SHVVA_LIST args;
	
	gdk_gc_get_values(GC,&values);
	if (color)
		gdk_gc_set_foreground(GC,SHVDrawGtk::GetColor(color));

	SHVVA_START(args,points);
	for (int i=0; i<points;i++)
	{
		point = SHVVA_ARG(args,SHVPointVal);
		point.Move(WindowArea.GetX(),WindowArea.GetY());
		pointArray[i].x = point.x;
		pointArray[i].y = point.y;
	}
	SHVVA_END(args);
	
	gdk_draw_polygon(GDK_DRAWABLE (Window), GC, TRUE, pointArray, points);
	
	if (color)
		color->ValidateRefCount();
	
	gdk_gc_set_foreground(GC,&values.foreground);
	
	::free(pointArray);
}

/*************************************
 * DrawBitmap
 *************************************/
void SHVDrawGtk::DrawBitmap(SHVBitmap* bitmap, SHVPoint position, int width, int height, SHVColor* transparentColor)
{
	if(bitmap && bitmap->IsCreated())
	{
	GdkDrawable* hBitmap = ((SHVBitmapGtk*)bitmap)->Bitmap;

		position.Move(WindowArea.GetX(),WindowArea.GetY());

		if(width == 0 && height == 0)
		{
			width = bitmap->GetWidth();
			height = bitmap->GetHeight();
		}
		
		if(width != bitmap->GetWidth() || height != bitmap->GetHeight())
		{
		GdkPixbuf* scaledPixBuf;
		GdkPixbuf* pixBuf;
			
			// TODO: Do something about transparency when scaling plix
			
			pixBuf = gdk_pixbuf_get_from_drawable(NULL,hBitmap,NULL,0,0,0,0,bitmap->GetWidth(),bitmap->GetHeight());
			scaledPixBuf = gdk_pixbuf_scale_simple(pixBuf,width,height, transparentColor ? GDK_INTERP_NEAREST : GDK_INTERP_BILINEAR);
			gdk_draw_pixbuf(GDK_DRAWABLE (Window),GC,scaledPixBuf,0,0,position.x,position.y,-1,-1,GDK_RGB_DITHER_NONE,0,0);
			
			g_object_unref(pixBuf);
			g_object_unref(scaledPixBuf);
		}
		else
		{
			if (transparentColor &&  ((SHVBitmapGtk*)bitmap)->RealiseTransparentBitmap(transparentColor))
			{
				hBitmap = ((SHVBitmapGtk*)bitmap)->Bitmap;
				gdk_gc_set_clip_mask(GC,((SHVBitmapGtk*)bitmap)->TransparencyMask);
				gdk_gc_set_clip_origin(GC,position.x,position.y);
			}
			
			gdk_draw_drawable(GDK_DRAWABLE (Window),GC,hBitmap,0,0,position.x,position.y,width,height);
			
			if (transparentColor)
			{
				gdk_gc_set_clip_mask(GC,NULL);
				SetClipRect();
			}
		}
		
	}

	bitmap->ValidateRefCount();
	if (transparentColor)
		transparentColor->ValidateRefCount();
}
void SHVDrawGtk::DrawBitmap(SHVBitmap* bitmap, SHVPoint position, SHVColor* transparentColor)
{
	DrawBitmap(bitmap,position,0,0,transparentColor);
}

/*************************************
 * DrawBitmapCentered
 *************************************/
void SHVDrawGtk::DrawBitmapCentered(SHVBitmap* bitmap, SHVRect rDest, int width, int height, SHVColor* transparentColor)
{
	if (bitmap)
	{
	SHVPoint p;

		if (width == 0)
			width = bitmap->GetWidth();
		if (height == 0)
			height = bitmap->GetHeight();

		p.x = (rDest.GetWidth()-width)/2 + rDest.GetLeft();
		p.y = (rDest.GetHeight()-height)/2 + rDest.GetTop();

		DrawBitmap(bitmap,p,width,height,transparentColor);
	}
	if (transparentColor)
		transparentColor->ValidateRefCount();
}

/*************************************
 * DrawBitmapCentered
 *************************************/
void SHVDrawGtk::DrawBitmapCentered(SHVBitmap* bitmap, SHVRect rDest, SHVColor* transparentColor)
{
	DrawBitmapCentered(bitmap,rDest,0,0,transparentColor);
}

/*************************************
 * DrawText
 *************************************/
void SHVDrawGtk::DrawText(const SHVStringC txt, SHVRect rect, SHVColor* color, int options)
{
	DrawText(NULL,txt,rect,color,options);
}
void SHVDrawGtk::DrawText(SHVFont* font, const SHVStringC txt, SHVRect rect, SHVColor* color, int options)
{
GdkGCValues values;
SHVStringUTF8 str(txt.ToStrUTF8());
PangoLayout* layout;
gint x,y;
	
	rect.MoveBy(WindowArea.GetX(), WindowArea.GetY());
	SetClipRect(&rect);
	
	gdk_gc_get_values(GC,&values);
	
	if (color)
		gdk_gc_set_foreground(GC,SHVDrawGtk::GetColor(color));
	
	
	// Set up text and ellipsis mode
	
	if ( (options&SHVDraw::TextEndEllipsis) || !(options&SHVDraw::TextMultiLine) )
		str.Replace("\n"," ");
	
	if (options&SHVDraw::TextEndEllipsis)
	{
		layout = gtk_widget_create_pango_layout(Widget,str.GetSafeBuffer());
		pango_layout_set_ellipsize(layout,PANGO_ELLIPSIZE_END);
		pango_layout_set_wrap(layout,PANGO_WRAP_WORD_CHAR);
		pango_layout_set_width(layout, pango_units_from_double(rect.GetWidth()));
	}
	else
	{
		layout = gtk_widget_create_pango_layout(Widget,str.GetSafeBuffer());
	}
	
	
	// Set up font
	if (font)
	{
		pango_layout_set_font_description(layout,((SHVFontGtk*)font)->GetFont());
	}
	
	
	// Set up alignment
	
	pango_layout_get_pixel_size(layout,&x,&y);
	
	if ( (options&SHVDraw::TextHCenter) == SHVDraw::TextHCenter )
		rect.SetLeft( (rect.GetWidth()-x)/2 + rect.GetLeft() );
	else if (options&SHVDraw::TextRight)
		rect.SetLeft( (rect.GetWidth()-x) + rect.GetLeft() );

	if ( (options&SHVDraw::TextVCenter) == SHVDraw::TextVCenter )
		rect.SetTop( (rect.GetHeight()-y)/2 + rect.GetTop() );
	else if (options&SHVDraw::TextRight)
		rect.SetTop( (rect.GetHeight()-y) + rect.GetTop() );
	
	pango_layout_context_changed(layout);
	
	
	// draw the text
	
	gdk_draw_layout(GDK_DRAWABLE (Window), GC, rect.GetX(), rect.GetY(), layout);

	if (color)
	{
		color->ValidateRefCount();
		gdk_gc_set_foreground(GC,&values.foreground);
	}
	
	SetClipRect();
	g_object_unref(layout);
}

/*************************************
 * SetClipRect
 *************************************/
void SHVDrawGtk::SetClipRect(SHVRect* rect)
{
GdkRectangle grct(SHVDrawGtk::MapRect(rect ? *rect : WindowArea));
	grct.width++;
	grct.height++;
	gdk_gc_set_clip_rectangle(GC,&grct);
}
