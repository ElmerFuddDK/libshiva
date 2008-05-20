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
