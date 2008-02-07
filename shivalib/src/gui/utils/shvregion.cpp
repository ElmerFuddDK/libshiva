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

#include "../include/utils/shvregion.h"
#include "../include/shvguimanager.h"

//=========================================================================================================
// SHVRegion - Control placement class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVRegion::SHVRegion(SHVControlContainer* container) : Container(container)
{
	Wnd = NULL;
	Reset();
}

/*************************************
 * SHVRect operator
 *************************************/
SHVRegion::operator const SHVRect&() const
{
	return Rect;
}

/*************************************
 * GetHeight
 *************************************/
int SHVRegion::GetHeight()
{
	return Rect.GetHeight();
}

/*************************************
 * GetWidth
 *************************************/
int SHVRegion::GetWidth()
{
	return Rect.GetWidth();
}


/*************************************
 * Reset
 *************************************/
void SHVRegion::Reset()
{
	SetRect(Container->GetRegionRect());
	HorizMargin = Container->GetManager()->GetConfig().FindInt(SHVGUIManager::CfgRegionHorizMargin,0).ToInt();
	VertMargin = Container->GetManager()->GetConfig().FindInt(SHVGUIManager::CfgRegionVertMargin,0).ToInt();
}

/*************************************
 * SetRect
 *************************************/
void SHVRegion::SetRect(const SHVRect& rect)
{
	Rect = rect;
}

//-=========================================================================================================
// SHVRegionAction - Performs actions against a region and its selected window
//-=========================================================================================================

/*************************************
 * Constructor
 *************************************/
///\cond INTERNAL
SHVRegionAction::SHVRegionAction(SHVRegion& region, SHVControl* wnd) : Region(region), Wnd(wnd)
{
	Initialized = false;
}
///\endcond

/*************************************
 * Destructor
 *************************************/
SHVRegionAction::~SHVRegionAction()
{
	if (Initialized)
		Wnd->SetRect(WindowRect);
}

// placement methods
/*************************************
 * Top
 *************************************/
SHVRegionAction& SHVRegionAction::Top(int leftMargin, int topMargin)
{
	if (Initialize())
	{
		if (leftMargin < 0)
			leftMargin = Region.HorizMargin;
		if (topMargin < 0)
			topMargin = Region.VertMargin;

		WindowRect.SetByXY( Region.Rect.GetLeft()+leftMargin,
							Region.Rect.GetTop()+topMargin,
							WindowRect.GetWidth(),
							WindowRect.GetHeight() );

		if (WindowRect.GetBottom() > Region.Rect.GetTop())
			Region.Rect.SetTop( WindowRect.GetBottom() );
	}

	return *this;
}

/*************************************
 * Bottom
 *************************************/
SHVRegionAction& SHVRegionAction::Bottom(int leftMargin, int topMargin)
{
	if (Initialize())
	{
		if (leftMargin < 0)
			leftMargin = Region.HorizMargin;
		if (topMargin < 0)
			topMargin = Region.VertMargin;

		WindowRect.SetByXY( Region.Rect.GetLeft()+leftMargin,
							Region.Rect.GetBottom()-topMargin-WindowRect.GetHeight(),
							WindowRect.GetWidth(),
							WindowRect.GetHeight() );

		if (WindowRect.GetTop() < Region.Rect.GetBottom())
			Region.Rect.SetBottom( WindowRect.GetTop() );
	}

	return *this;
}

/*************************************
 * Left
 *************************************/
SHVRegionAction& SHVRegionAction::Left(int leftMargin, int topMargin)
{
	if (Initialize())
	{
		if (leftMargin < 0)
			leftMargin = Region.HorizMargin;
		if (topMargin < 0)
			topMargin = Region.VertMargin;

		WindowRect.SetByXY( Region.Rect.GetLeft()+leftMargin,
							Region.Rect.GetTop()+topMargin,
							WindowRect.GetWidth(),
							WindowRect.GetHeight() );

		if (WindowRect.GetRight() > Region.Rect.GetLeft())
			Region.Rect.SetLeft( WindowRect.GetRight() );
	}

	return *this;
}

/*************************************
 * Right
 *************************************/
SHVRegionAction& SHVRegionAction::Right(int leftMargin, int topMargin)
{
	if (Initialize())
	{
		if (leftMargin < 0)
			leftMargin = Region.HorizMargin;
		if (topMargin < 0)
			topMargin = Region.VertMargin;

		WindowRect.SetByXY( Region.Rect.GetLeft()-leftMargin-WindowRect.GetWidth(),
							Region.Rect.GetTop()+topMargin,
							WindowRect.GetWidth(),
							WindowRect.GetHeight() );

		if (WindowRect.GetLeft() < Region.Rect.GetRight())
			Region.Rect.SetRight( WindowRect.GetLeft() );
	}

	return *this;
}

/*************************************
 * AlignLeftRight
 *************************************/
/// Aligns the control between 2 optional controls
SHVRegionAction& SHVRegionAction::AlignLeftRight(SHVControl* left, SHVControl* right, int alignment, int margin)
{
	if (Initialize())
	{
	SHVRect rctLeft, rctRight;
	int width, height;
	
		width = WindowRect.GetWidth();
		height = WindowRect.GetHeight();

		if (margin < 0)
			margin = Region.HorizMargin;

		if (left)
		{
			rctLeft = left->GetRect();
		}
		else
		{
			rctLeft = Region.Rect;
			rctLeft.SetRight(rctLeft.GetLeft());
		}

		if (right)
		{
			rctRight = right->GetRect();
		}
		else
		{
			rctRight = Region.Rect;
			rctRight.SetLeft(rctRight.GetRight());
		}

		// set the x axis position
		if ( (alignment&SHVRegion::AlignHorizCenter) )
		{
			WindowRect.SetLeft( (rctRight.GetLeft() - rctLeft.GetRight() - width)/2 );
			WindowRect.SetWidth( width );
		}
		else if ( (alignment&SHVRegion::AlignRight) )
		{
			WindowRect.SetLeft( rctRight.GetLeft() - width - margin );
			WindowRect.SetWidth( width );
		}
		else if ( (alignment&SHVRegion::AlignLeft) )
		{
			WindowRect.SetLeft( rctLeft.GetRight() + margin );
			WindowRect.SetWidth( width );
		}
	
		// check if we need to align the y axis as well
		if ( (alignment&SHVRegion::AlignTop) || (alignment&SHVRegion::AlignBottom) )
		{
		SHVInt top, bottom;
			
			if (!left && !right)
			{
				top = Region.Rect.GetTop();
				bottom = Region.Rect.GetBottom();
			}
			else
			{
				if (left)
				{
					if (top.IsNull() || rctLeft.GetTop() < top)
						top = rctLeft.GetTop();
					if (bottom.IsNull() || rctLeft.GetBottom() > bottom)
						bottom = rctLeft.GetBottom();
				}
				if (right)
				{
					if (top.IsNull() || rctRight.GetTop() < top)
						top = rctRight.GetTop();
					if (bottom.IsNull() || rctRight.GetBottom() > bottom)
						bottom = rctRight.GetBottom();
				}
			}
			
			// set the y axis position
			if ( (alignment&SHVRegion::AlignVertCenter) )
			{
				WindowRect.SetTop( (bottom - top - height)/2 );
				WindowRect.SetHeight( height );
			}
			else if ( (alignment&SHVRegion::AlignBottom) )
			{
				WindowRect.SetTop( bottom - height - margin );
				WindowRect.SetHeight( height );
			}
			else if ( (alignment&SHVRegion::AlignTop) )
			{
				WindowRect.SetTop( top + margin );
				WindowRect.SetHeight( height );
			}
		}
	}

	return *this;
}

/*************************************
 * FillLeftRight
 *************************************/
/// Sizes the control between 2 optional controls, and places it on the X axis
SHVRegionAction& SHVRegionAction::FillLeftRight(SHVControl* left, SHVControl* right, int margin)
{
	if (Initialize())
	{
	SHVRect rctLeft, rctRight;

		if (margin < 0)
			margin = Region.HorizMargin;

		if (left)
		{
			rctLeft = left->GetRect();
		}
		else
		{
			rctLeft = Region.Rect;
			rctLeft.SetRight(rctLeft.GetLeft());
		}

		if (right)
		{
			rctRight = right->GetRect();
		}
		else
		{
			rctRight = Region.Rect;
			rctRight.SetLeft(rctRight.GetRight());
		}

		WindowRect.SetLeft(rctLeft.GetRight() + margin);
		WindowRect.SetRight(rctRight.GetLeft() - margin);
	}

	return *this;
}

/*************************************
 * Initialize
 *************************************/
///\cond INTERNAL
bool SHVRegionAction::Initialize()
{
	if (!Initialized && Wnd && Wnd->IsCreated())
	{
		Initialized = true;
		WindowRect = Wnd->GetRect();
	}

	return Initialized;
}
///\endcond
