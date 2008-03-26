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
#include "../../include/platformspc.h"

#include "../../include/gui/utils/shvcolor.h"
#include "../../include/gui/shvguimanager.h"


//=========================================================================================================
// SHVColor - interface for color
//=========================================================================================================

/*************************************
 * MergeInternal
 *************************************/
SHVColor* SHVColor::MergeInternal(SHVGUIManager* manager, SHVColor* clr2, int opacity, MergeColorType mergeType)
{
int r1,g1,b1;
int r2,g2,b2;
double dblOpacity;

	dblOpacity = double(opacity)/100.0;

	r1 = GetRed();
	g1 = GetGreen();
	b1 = GetBlue();

	r2 = clr2->GetRed();
	g2 = clr2->GetGreen();
	b2 = clr2->GetBlue();

	switch(mergeType)
	{
	case MergeTypeLighten:
		r1 = (r1 >= r2 ? r1 : r1 - int(double(r1-r2)*dblOpacity));
		g1 = (g1 >= g2 ? g1 : g1 - int(double(g1-g2)*dblOpacity));
		b1 = (b1 >= b2 ? b1 : b1 - int(double(b1-b2)*dblOpacity));
		break;
	case MergeTypeTransparency:
		r1 = r1 - int(double(r1-r2)*dblOpacity);
		g1 = g1 - int(double(g1-g2)*dblOpacity);
		b1 = b1 - int(double(b1-b2)*dblOpacity);
		break;
	}

	// this might not be nessecary, but make sure the colors are within the boundaries
	if (r1 < 0)
		r1 = 0;
	else if (r1 > 0xFF)
		r1 = 0xFF;
	if (g1 < 0)
		g1 = 0;
	else if (g1 > 0xFF)
		g1 = 0xFF;
	if (b1 < 0)
		b1 = 0;
	else if (b1 > 0xFF)
		b1 = 0xFF;

	return manager->CreateColor((ColorVal)r1,(ColorVal)g1,(ColorVal)b1);
}
