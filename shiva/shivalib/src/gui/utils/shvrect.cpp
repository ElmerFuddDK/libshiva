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
#include "../../../../include/platformspc.h"

#include "../../../../include/gui/utils/shvrect.h"

//=========================================================================================================
// SHVRect - Rectangle for the GUI
//=========================================================================================================

/*************************************
 * Constructors
 *************************************/
SHVRect::SHVRect()
{
	Left = Top = Right = Bottom = 0;
}
SHVRect::SHVRect(int left, int top, int right, int bottom)
{
	SetByLeftTop(left,top,right,bottom);
}

/*************************************
 * operators
 *************************************/
bool SHVRect::operator==(const SHVRect& rect) const
{
	return (rect.GetLeft() == GetLeft() && 
			rect.GetTop() == GetTop() &&
			rect.GetRight() == GetRight() &&
			rect.GetBottom() == GetBottom());
}
bool SHVRect::operator!=(const SHVRect& rect) const
{
	return (rect.GetLeft() != GetLeft() ||
			rect.GetTop() != GetTop() ||
			rect.GetRight() != GetRight() ||
			rect.GetBottom() != GetBottom());
}

/*************************************
 * SetByXY
 *************************************/
void SHVRect::SetByXY(int x, int y, int width, int height)
{
	SetLeft(x);
	SetTop(y);
	SetWidth(width);
	SetHeight(height);
}

/*************************************
 * SetByLeftTop
 *************************************/
void SHVRect::SetByLeftTop(int left, int top, int right, int bottom)
{
	SetLeft(left);
	SetTop(top);
	SetRight(right);
	SetBottom(bottom);
}

/*************************************
 * MoveTo
 *************************************/
void SHVRect::MoveTo(int x, int y)
{
	SetByXY(x,y,GetWidth(),GetHeight());
}

/*************************************
 * MoveBy
 *************************************/
void SHVRect::MoveBy(int x, int y)
{
	SetByXY(GetX()+x,GetY()+y,GetWidth(),GetHeight());
}

/*************************************
 * Shrink
 *************************************/
void SHVRect::Shrink(int left, int top, int right, int bottom)
{
	SetLeft(GetLeft()+left);
	SetTop(GetTop()+top);
	SetRight(GetRight()-right);
	SetBottom(GetBottom()-bottom);
}
