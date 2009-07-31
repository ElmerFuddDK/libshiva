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

#include "../../../../include/utils/shvstringutf8.h"
#include "shvfontgtk.h"
#include "../shvgtk.h"

#include <pango/pango-font.h>


//=========================================================================================================
// SHVFontGtk
//=========================================================================================================

/*************************************
 * Constructors
 *************************************/
SHVFontGtk::SHVFontGtk(PangoFontDescription* font, PangoContext* context, bool owner)
		: Font(font), Context(context)
{
	SHVASSERT(Font);
	if (Font && !owner)
		Font = pango_font_description_copy(Font);
	Height = CellHeight = -1;
	ApproximateWidth = -1;
}

/*************************************
 * Destructor
 *************************************/
SHVFontGtk::~SHVFontGtk()
{
	if (Font)
		pango_font_description_free(Font);
}

/*************************************
 * CreateCopy
 *************************************/
SHVFont* SHVFontGtk::CreateCopy(int sizePercentage, int styles)
{
SHVFont* retVal = NULL;
PangoFontDescription* font = pango_font_description_copy(Font);

	if (font)
	{
		if (sizePercentage != 100)
		{
		gint height = pango_font_description_get_size(font);
			pango_font_description_set_size(font,(height*sizePercentage)/100);
		}

		if (styles != SHVFont::StyleUnchanged)
		{
			pango_font_description_set_weight(font, ( styles & SHVFont::StyleBold ? PANGO_WEIGHT_BOLD : PANGO_WEIGHT_NORMAL ));
			pango_font_description_set_style(font, ( styles & SHVFont::StyleItalic ? PANGO_STYLE_ITALIC : PANGO_STYLE_NORMAL ));
			SHVASSERT( !(styles & SHVFont::StyleUnderline) ); // we do not support underline yet!
			//pango_font_description_set_underlinethickness(font, ( styles & SHVFont::StyleUnderline ? 1 : 0 ));
		}

		retVal = new SHVFontGtk(font,Context,true);
	}

	return retVal;
}

/*************************************
 * GetHeight
 *************************************/
int SHVFontGtk::GetHeight()
{
	if (Height == -1 && Font)
	{
		SHVASSERT(pango_font_description_get_set_fields(Font)&PANGO_FONT_MASK_SIZE);
		Height = pango_font_description_get_size(Font);
		if (!pango_font_description_get_size_is_absolute(Font))
		{
			Height = PANGO_PIXELS(Height);
		}
	}
	return Height;
}

/*************************************
 * GetCellHeight
 *************************************/
int SHVFontGtk::GetCellHeight()
{
	if (CellHeight == -1 && Font)
	{
	PangoFontMetrics* metrics = pango_context_get_metrics(Context,Font,NULL);
		
		if (metrics)
		{
			CellHeight = pango_font_metrics_get_ascent(metrics) + pango_font_metrics_get_descent(metrics);
			if (!pango_font_description_get_size_is_absolute(Font))
			{
				CellHeight = PANGO_PIXELS(CellHeight);
			}
			pango_font_metrics_unref(metrics);
		}
	}
	return CellHeight;
}

/*************************************
 * GetApproximateWidth
 *************************************/
int SHVFontGtk::GetApproximateWidth()
{
	if (ApproximateWidth == -1)
		ApproximateWidth = CalculateTextWidth(_T("W"));
	return ApproximateWidth;
}

/*************************************
 * CalculateTextWidth
 *************************************/
int SHVFontGtk::CalculateTextWidth(const SHVStringC text)
{
int retVal = 0;
PangoLayout* layout = pango_layout_new(Context);
	
	if (layout)
	{
	int height;
		pango_layout_set_font_description(layout,Font);
		pango_layout_set_text(layout,text.ToStrUTF8().GetSafeBuffer(),-1);
		pango_layout_get_pixel_size(layout,&retVal,&height);
		g_object_unref(layout);
	}
	
	return retVal;
}

/*************************************
 * GetName
 *************************************/
SHVStringBuffer SHVFontGtk::GetName()
{
SHVString retVal;

	if  (Font)
	{
		retVal = pango_font_description_get_family(Font);
	}
	return retVal.ReleaseBuffer();
}

/*************************************
 * CopyFrom
 *************************************/
SHVFontGtk* SHVFontGtk::CopyFrom(PangoFontDescription* font, PangoContext* context)
{
	if (font)
		font = pango_font_description_copy(font);

	return (font ? new SHVFontGtk(font,context,true) : NULL);
}

/*************************************
 * CopyFrom
 *************************************/
SHVFontGtk* SHVFontGtk::CreateFromName(GtkWidget* mainWnd, const SHVStringC name, int height, int styles)
{
SHVFontGtk* retVal = NULL;
PangoFontDescription* desc;
SHVString8 styleStr;
	
	if (styles&SHVFont::StyleBold)
		styleStr += "Bold ";
	if (styles&SHVFont::StyleItalic)
		styleStr += "Italic ";
	SHVASSERT( !(styles & SHVFont::StyleUnderline) ); // we do not support underline yet!
	
	if (styleStr.IsEmpty())
		styleStr = "Normal ";
	
	desc = pango_font_description_from_string(SHVString8C::Format("%s,%s%d", name.GetSafeBuffer(), styleStr.GetSafeBuffer(), height).GetSafeBuffer());
	
	// We can use the context from the main window since it will always exist during the programs lifetime
	retVal = new SHVFontGtk(desc,gtk_widget_get_pango_context(mainWnd),true);
	
	return retVal;
}
