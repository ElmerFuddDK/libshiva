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


//=========================================================================================================
// SHVFontGtk
//=========================================================================================================

/*************************************
 * Constructors
 *************************************/
SHVFontGtk::SHVFontGtk(PangoFontDescription* font, bool owner) : Font(font)
{
	SHVASSERT(Font);
	if (Font && !owner)
		Font = pango_font_description_copy(Font);
	Height = CellHeight = -1;
	ApproximateWidth = -1;
	DrawFont = NULL;
}

/*************************************
 * Destructor
 *************************************/
SHVFontGtk::~SHVFontGtk()
{
	if (Font)
		pango_font_description_free(Font);
	if (DrawFont)
		gdk_font_unref(DrawFont);
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
			//pango_font_description_set_underlinethickness(font, ( styles & SHVFont::StyleUnderline ? 1 : 0 ));
		}

		retVal = new SHVFontGtk(font,true);
	}

	return retVal;
}

/*************************************
 * GetHeight
 *************************************/
int SHVFontGtk::GetHeight()
{
	if (Height == -1 && Font)
		Height = PANGO_PIXELS(pango_font_description_get_size(Font));
	return Height;
}

/*************************************
 * GetCellHeight
 *************************************/
int SHVFontGtk::GetCellHeight()
{
	if (CellHeight == -1 && Font)
	{
		if (InitDrawFont())
		{
		gint lbearing, rbearing, width, ascent, descent;
		
			descent = 0;
			gdk_string_extents(DrawFont, "q",&lbearing,&rbearing,&width,&ascent,&descent);
			
			CellHeight = GetHeight() + descent;
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
 * GetWidth
 *************************************/
int SHVFontGtk::CalculateTextWidth(const SHVStringC text)
{
int retVal = 0;
	if (InitDrawFont())
	{
		retVal = gdk_string_width(DrawFont,text.ToStrUTF8().GetSafeBuffer());
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
 * GetName
 *************************************/
SHVFontGtk* SHVFontGtk::CopyFrom(PangoFontDescription* font)
{
	if (font)
		font = pango_font_description_copy(font);

	return (font ? new SHVFontGtk(font,true) : NULL);
}

///\cond INTERNAL
/*************************************
 * InitDrawFont
 *************************************/
bool SHVFontGtk::InitDrawFont()
{
	if (Font && !DrawFont)
	{
		DrawFont = gdk_font_from_description(Font);
	}
	
	return DrawFont != NULL;
}
///\endcond