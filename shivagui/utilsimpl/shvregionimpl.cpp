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

#include "shvregionimpl.h"
#include "../include/shvguimanager.h"

//=========================================================================================================
// SHVRegionImpl - Control placement class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVRegionImpl::SHVRegionImpl(SHVControlContainer* container) : Container(container)
{
	Wnd = NULL;
	Reset();
}

/*************************************
 * SHVRect operator
 *************************************/
SHVRegionImpl::operator const SHVRect&() const
{
	return Rect;
}

/*************************************
 * GetHeight
 *************************************/
int SHVRegionImpl::GetHeight()
{
	return Rect.GetHeight();
}

/*************************************
 * GetWidth
 *************************************/
int SHVRegionImpl::GetWidth()
{
	return Rect.GetWidth();
}


/*************************************
 * Reset
 *************************************/
void SHVRegionImpl::Reset()
{
	SetRect(Container->GetRegionRect());
	HorizMargin = Container->GetManager()->GetConfig().FindInt(SHVGUIManager::CfgRegionHorizMargin,0).ToInt();
	VertMargin = Container->GetManager()->GetConfig().FindInt(SHVGUIManager::CfgRegionVertMargin,0).ToInt();
}

/*************************************
 * SetRect
 *************************************/
void SHVRegionImpl::SetRect(const SHVRect& rect)
{
	Rect = rect;
}

/*************************************
 * ClipTop
 *************************************/
SHVRegion* SHVRegionImpl::ClipTop(int pixels)
{
	Rect.SetTop(Rect.GetTop()+pixels);
	return this;
}

/*************************************
 * ClipBottom
 *************************************/
SHVRegion* SHVRegionImpl::ClipBottom(int pixels)
{
	Rect.SetBottom(Rect.GetBottom()-pixels);
	return this;
}

/*************************************
 * ClipLeft
 *************************************/
SHVRegion* SHVRegionImpl::ClipLeft(int pixels)
{
	Rect.SetLeft(Rect.GetLeft()+pixels);
	return this;
}

/*************************************
 * ClipRight
 *************************************/
SHVRegion* SHVRegionImpl::ClipRight(int pixels)
{
	Rect.SetRight(Rect.GetRight()-pixels);
	return this;
}

/*************************************
 * Move a window
 *************************************/
SHVRegionActionPtr SHVRegionImpl::Move(SHVControl* wnd)
{
	return new SHVRegionActionImpl(*this,wnd);
}



//=========================================================================================================
// SHVRegionActionImpl - Performs actions against a region and its selected window
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
///\cond INTERNAL
SHVRegionActionImpl::SHVRegionActionImpl(SHVRegionImpl& region, SHVControl* wnd) : Region(region), Wnd(wnd)
{
	Initialized = false;
}
///\endcond

/*************************************
 * Destructor
 *************************************/
SHVRegionActionImpl::~SHVRegionActionImpl()
{
	if (Initialized)
		Wnd->SetRect(WindowRect);
}

// placement methods
/*************************************
 * Top
 *************************************/
SHVRegionAction* SHVRegionActionImpl::Top(int leftMargin, int topMargin)
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
	}

	return this;
}

/*************************************
 * Bottom
 *************************************/
SHVRegionAction* SHVRegionActionImpl::Bottom(int leftMargin, int topMargin)
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
	}

	return this;
}

/*************************************
 * Left
 *************************************/
SHVRegionAction* SHVRegionActionImpl::Left(int leftMargin, int topMargin)
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
	}

	return this;
}

/*************************************
 * Right
 *************************************/
SHVRegionAction* SHVRegionActionImpl::Right(int leftMargin, int topMargin)
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
	}

	return this;
}

/*************************************
 * ClipTop
 *************************************/
SHVRegionAction* SHVRegionActionImpl::ClipTop(int extraMargin)
{
	if (Initialize())
	{
		if (WindowRect.GetBottom()+extraMargin > Region.Rect.GetTop())
			Region.Rect.SetTop( WindowRect.GetBottom()+extraMargin );
	}

	return this;
}

/*************************************
 * ClipBottom
 *************************************/
SHVRegionAction* SHVRegionActionImpl::ClipBottom(int extraMargin)
{
	if (Initialize())
	{
		if (WindowRect.GetTop()-extraMargin < Region.Rect.GetBottom())
			Region.Rect.SetBottom( WindowRect.GetTop()-extraMargin );
	}

	return this;
}

/*************************************
 * ClipLeft
 *************************************/
SHVRegionAction* SHVRegionActionImpl::ClipLeft(int extraMargin)
{
	if (Initialize())
	{
		if (WindowRect.GetRight()+extraMargin > Region.Rect.GetLeft())
			Region.Rect.SetLeft( WindowRect.GetRight()+extraMargin );
	}

	return this;
}

/*************************************
 * ClipRight
 *************************************/
SHVRegionAction* SHVRegionActionImpl::ClipRight(int extraMargin)
{
	if (Initialize())
	{
		if (WindowRect.GetLeft()-extraMargin < Region.Rect.GetRight())
			Region.Rect.SetRight( WindowRect.GetLeft()-extraMargin );
	}

	return this;
}

/*************************************
 * AlignLeftRight
 *************************************/
/// Aligns the control between 2 optional controls
SHVRegionAction* SHVRegionActionImpl::AlignLeftRight(SHVControl* left, SHVControl* right, int alignment, int margin)
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
		if ( (alignment&SHVRegion::AlignHCenter) == SHVRegion::AlignHCenter )
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
			if ( (alignment&SHVRegion::AlignVCenter) == SHVRegion::AlignVCenter )
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

	return this;
}

/*************************************
 * LeftOf
 *************************************/
SHVRegionAction* SHVRegionActionImpl::LeftOf(SHVControl* left, int leftMargin)
{
	if (Initialize())
	{
	SHVRect rect(Region.Rect);

		if (leftMargin < 0)
			leftMargin = Region.HorizMargin;

		if (left)
		{
		SHVRect leftRect(left->GetRect());

			rect.SetLeft(leftRect.GetRight()+leftMargin);
			rect.SetTop(leftRect.GetTop());
		}
		else
		{
			rect.SetTop(rect.GetTop()+Region.VertMargin);
			rect.SetLeft(rect.GetRight()+leftMargin);
		}

		rect.SetWidth(WindowRect.GetWidth());
		rect.SetHeight(WindowRect.GetHeight());

		WindowRect = rect;
	}

	return this;
}

/*************************************
 * RightOf
 *************************************/
SHVRegionAction* SHVRegionActionImpl::RightOf(SHVControl* right, int rightMargin)
{
	if (Initialize())
	{
	SHVRect rect(Region.Rect);

		if (rightMargin < 0)
			rightMargin = Region.HorizMargin;

		if (right)
		{
		SHVRect rightRect(right->GetRect());

			rect.SetRight(rightRect.GetLeft()+rightMargin);
			rect.SetTop(rightRect.GetTop());
		}
		else
		{
			rect.SetTop(rect.GetTop()+Region.VertMargin);
			rect.SetRight(rect.GetLeft()+rightMargin);
		}

		rect.SetLeft(rect.GetRight()-WindowRect.GetWidth());
		rect.SetHeight(WindowRect.GetHeight());

		WindowRect = rect;
	}

	return this;
}

/*************************************
 * SetPercent
 *************************************/
SHVRegionAction* SHVRegionActionImpl::SetPercent(int x, int y, int width, int height, SHVRect margins)
{
	if (Initialize())
	{
	int wwidth, wheight, i, j;

		if (margins.GetLeft() < 0)
			margins.SetLeft(Region.HorizMargin);
		if (margins.GetRight() < 0)
			margins.SetRight(Region.HorizMargin);
		if (margins.GetTop() < 0)
			margins.SetTop(Region.VertMargin);
		if (margins.GetBottom() < 0)
			margins.SetBottom(Region.VertMargin);

		wwidth = WindowRect.GetWidth();
		wheight = WindowRect.GetHeight();

		// calculate and set left/right
		i = (Region.Rect.GetWidth()*width)/100;
		j = Region.Rect.GetLeft() + (Region.Rect.GetWidth()*x)/100;
		if (x>=0)
		{
			WindowRect.SetLeft(j + margins.GetLeft());
			i-=margins.GetLeft();
		}
		if (width>=0)
		{
			WindowRect.SetWidth(i - margins.GetRight());
			if (WindowRect.GetRight() > Region.Rect.GetRight() - margins.GetRight())
				WindowRect.SetRight(Region.Rect.GetRight() - margins.GetRight());
		}
		else
		{
			WindowRect.SetWidth(wwidth);
		}
		
		// calculate and set top/bottom
		i = (Region.Rect.GetHeight()*height)/100;
		j = Region.Rect.GetTop() + (Region.Rect.GetHeight()*y)/100;
		if (y>=0)
		{
			WindowRect.SetTop(j + margins.GetTop());
			i-=margins.GetTop();
		}
		if (height>=0)
		{
			WindowRect.SetHeight(i - margins.GetBottom());
			if (WindowRect.GetBottom() > Region.Rect.GetBottom() - margins.GetBottom())
				WindowRect.SetBottom(Region.Rect.GetBottom() - margins.GetBottom());
		}
		else
		{
			WindowRect.SetHeight(wheight);
		}
	}

	return this;
}

/*************************************
 * FillLeftRight
 *************************************/
/// Sizes the control between 2 optional controls, and places it on the X axis
SHVRegionAction* SHVRegionActionImpl::FillLeftRight(SHVControl* left, SHVControl* right, int margin)
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

	return this;
}

/*************************************
 * Initialize
 *************************************/
///\cond INTERNAL
bool SHVRegionActionImpl::Initialize()
{
	if (!Initialized && Wnd && Wnd->IsCreated())
	{
		Initialized = true;
		WindowRect = Wnd->GetRect();
	}

	return Initialized;
}
///\endcond
