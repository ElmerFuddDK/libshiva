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
	Font = Container->GetFont();
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
 * SetRect
 *************************************/
SHVRegion* SHVRegionImpl::SetMargin(int hmargin, int vmargin)
{
	return SetMarginInPixels(Font->LFUToWidth(hmargin),Font->LFUToHeight(vmargin));
}

/*************************************
 * SetRect
 *************************************/
SHVRegion* SHVRegionImpl::SetMarginInPixels(int hmargin, int vmargin)
{
	HorizMargin = hmargin;
	VertMargin = vmargin;

	return this;
}

/*************************************
 * ClipTop
 *************************************/
SHVRegion* SHVRegionImpl::ClipTop(int lfuheight)
{
	return ClipTopInPixels(Font->LFUToHeight(lfuheight));
}

/*************************************
 * ClipBottom
 *************************************/
SHVRegion* SHVRegionImpl::ClipBottom(int lfuheight)
{
	return ClipBottomInPixels(Font->LFUToHeight(lfuheight));
}

/*************************************
 * ClipLeft
 *************************************/
SHVRegion* SHVRegionImpl::ClipLeft(int lfuwidth)
{
	return ClipLeftInPixels(Font->LFUToWidth(lfuwidth));
}

/*************************************
 * ClipRight
 *************************************/
SHVRegion* SHVRegionImpl::ClipRight(int lfuwidth)
{
	return ClipRightInPixels(Font->LFUToWidth(lfuwidth));
}

/*************************************
 * ClipTopInPixels
 *************************************/
SHVRegion* SHVRegionImpl::ClipTopInPixels(int pixels)
{
	Rect.SetTop(Rect.GetTop()+pixels);
	return this;
}

/*************************************
 * ClipBottomInPixels
 *************************************/
SHVRegion* SHVRegionImpl::ClipBottomInPixels(int pixels)
{
	Rect.SetBottom(Rect.GetBottom()-pixels);
	return this;
}

/*************************************
 * ClipLeftInPixels
 *************************************/
SHVRegion* SHVRegionImpl::ClipLeftInPixels(int pixels)
{
	Rect.SetLeft(Rect.GetLeft()+pixels);
	return this;
}

/*************************************
 * ClipRightInPixels
 *************************************/
SHVRegion* SHVRegionImpl::ClipRightInPixels(int pixels)
{
	Rect.SetRight(Rect.GetRight()-pixels);
	return this;
}

/*************************************
 * Move a window
 *************************************/
SHVRegionActionPtr SHVRegionImpl::Move(SHVControl* wnd)
{
	return new SHVRegionActionImpl(*this,wnd,true);
}

/*************************************
 * Move a window (in pixels)
 *************************************/
SHVRegionActionPtr SHVRegionImpl::MoveInPixels(SHVControl* wnd)
{
	return new SHVRegionActionImpl(*this,wnd,false);
}



//=========================================================================================================
// SHVRegionActionImpl - Performs actions against a region and its selected window
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
///\cond INTERNAL
SHVRegionActionImpl::SHVRegionActionImpl(SHVRegionImpl& region, SHVControl* wnd, bool lfumode) : Region(region)
{
	Initialized = false;
	LFUMode = lfumode;

	And(wnd);
}
///\endcond

/*************************************
 * Destructor
 *************************************/
SHVRegionActionImpl::~SHVRegionActionImpl()
{
SHVListWndIterator itr(Wnds);

	Commit();

	while (itr.MoveNext())
	{
		// adjust according to max
		if (!itr.Get()->MaxWidth.IsNull() && itr.Get()->Rect.GetWidth() > itr.Get()->MaxWidth)
		{
			itr.Get()->Rect.SetWidth(itr.Get()->MaxWidth);
		}
		if (!itr.Get()->MaxHeight.IsNull() && itr.Get()->Rect.GetHeight() > itr.Get()->MaxHeight)
		{
			itr.Get()->Rect.SetHeight(itr.Get()->MaxHeight);
		}

		itr.Get()->Wnd->SetRect(itr.Get()->Rect);
	}
}

/*************************************
 * And
 *************************************/
SHVRegionAction* SHVRegionActionImpl::And(SHVControl* extraControl)
{
	if (extraControl && extraControl->IsCreated())
	{
		Commit();
		Wnds.AddTail(new Control(extraControl));
	}

	return this;
}

/*************************************
 * CtrlMaxWidth
 *************************************/
SHVRegionAction* SHVRegionActionImpl::CtrlMaxWidth(int width)
{
	if (Wnds.GetCount())
	{
		CalculateHMargin(width);

		Wnds.GetLast()->FixedWidth = false;
		Wnds.GetLast()->MaxWidth = width;
	}
	
	return this;
}

/*************************************
 * CtrlWidth
 *************************************/
SHVRegionAction* SHVRegionActionImpl::CtrlWidth(int width)
{
	if (Wnds.GetCount())
	{
		CalculateHMargin(width);

		Commit();
		Wnds.GetLast()->Rect.SetWidth(width);
	}
	
	return this;
}

/*************************************
 * CtrlFixedWidth
 *************************************/
SHVRegionAction* SHVRegionActionImpl::CtrlFixedWidth()
{
	if (Wnds.GetCount())
	{
		Wnds.GetLast()->FixedWidth = true;
		Wnds.GetLast()->MaxWidth.SetToNull();
	}
	
	return this;
}

/*************************************
 * CtrlFixedHeight
 *************************************/
SHVRegionAction* SHVRegionActionImpl::CtrlFixedHeight()
{
	if (Wnds.GetCount())
	{
		Wnds.GetLast()->FixedHeight = true;
		Wnds.GetLast()->MaxHeight.SetToNull();
	}
	
	return this;
}

// placement methods
/*************************************
 * Top
 *************************************/
SHVRegionAction* SHVRegionActionImpl::Top(int leftMargin, int topMargin)
{
	if (Initialize())
	{
		CalculateHMargin(leftMargin);
		CalculateVMargin(topMargin);

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
		CalculateHMargin(leftMargin);
		CalculateVMargin(topMargin);

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
		CalculateHMargin(leftMargin);
		CalculateVMargin(topMargin);

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
		CalculateHMargin(leftMargin);
		CalculateVMargin(topMargin);

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
		CalculateVMargin(extraMargin);

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
		CalculateVMargin(extraMargin);

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
		CalculateHMargin(extraMargin);

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
		CalculateHMargin(extraMargin);

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

		// Make sure you don't use any of the windows in the combined list
		SHVASSERT(left == NULL || !ContainsWnd(left));
		SHVASSERT(right == NULL || !ContainsWnd(right));
	
		width = WindowRect.GetWidth();
		height = WindowRect.GetHeight();

		CalculateHMargin(margin);

		if (left && left->IsCreated())
		{
			rctLeft = left->GetRect();
		}
		else
		{
			rctLeft = Region.Rect;
			rctLeft.SetRight(rctLeft.GetLeft());
		}

		if (right && right->IsCreated())
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

		// Make sure you don't use any of the windows in the combined list
		SHVASSERT(left == NULL || !ContainsWnd(left));
		
		CalculateHMargin(leftMargin);

		if (left && left->IsCreated())
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

		// Make sure you don't use any of the windows in the combined list
		SHVASSERT(right == NULL || !ContainsWnd(right));

		CalculateHMargin(rightMargin);

		if (right && right->IsCreated())
		{
		SHVRect rightRect(right->GetRect());

			rect.SetRight(rightRect.GetLeft()-rightMargin);
			rect.SetTop(rightRect.GetTop());
		}
		else
		{
			rect.SetTop(rect.GetTop()+Region.VertMargin);
			rect.SetRight(rect.GetLeft()-rightMargin);
		}

		rect.SetLeft(rect.GetRight()-WindowRect.GetWidth());
		rect.SetHeight(WindowRect.GetHeight());

		WindowRect = rect;
	}

	return this;
}

/*************************************
 * FillPercent
 *************************************/
SHVRegionAction* SHVRegionActionImpl::FillPercent(int x, int y, int width, int height, SHVRect margins)
{
	if (Initialize())
	{
	int wwidth, wheight, i, j;

		CalculateRectMargin(margins);

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

		// Make sure you don't use any of the windows in the combined list
		SHVASSERT(left == NULL || !ContainsWnd(left));
		SHVASSERT(right == NULL || !ContainsWnd(right));

		CalculateHMargin(margin);

		if (left && left->IsCreated())
		{
			rctLeft = left->GetRect();
		}
		else
		{
			rctLeft = Region.Rect;
			rctLeft.SetRight(rctLeft.GetLeft());
		}

		if (right && right->IsCreated())
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

///\cond INTERNAL
/*************************************
 * Control constructor
 *************************************/
SHVRegionActionImpl::Control::Control(SHVControl* wnd) : Wnd(wnd), Rect(wnd->GetRect())
{
	FixedHeight = FixedWidth = false;
}
/*************************************
 * Initialize
 *************************************/
bool SHVRegionActionImpl::Initialize()
{
	if (!Initialized && Wnds.GetCount())
	{
	SHVListWndIterator itr(Wnds);
	SHVRect tmpRect;

		Initialized = true;

		WindowRect = Wnds.GetFirst()->Rect;

		itr.MoveNext();
		while (itr.MoveNext())
		{
			tmpRect = itr.Get()->Rect;

			if (tmpRect.GetLeft() < WindowRect.GetLeft())
				WindowRect.SetLeft(tmpRect.GetLeft());

			if (tmpRect.GetTop() < WindowRect.GetTop())
				WindowRect.SetTop(tmpRect.GetTop());

			if (tmpRect.GetRight() > WindowRect.GetRight())
				WindowRect.SetRight(tmpRect.GetRight());

			if (tmpRect.GetBottom() > WindowRect.GetBottom())
				WindowRect.SetBottom(tmpRect.GetBottom());
		}

		OrigRect = WindowRect;
	}

	return Initialized;
}

/*************************************
 * Commit
 *************************************/
void SHVRegionActionImpl::Commit()
{
	if (Initialized)
	{
		if (Wnds.GetCount() == 1)
		{
			if (Wnds.GetFirst()->FixedWidth)
				WindowRect.SetWidth(Wnds.GetFirst()->Rect.GetWidth());
			if (Wnds.GetFirst()->FixedHeight)
				WindowRect.SetHeight(Wnds.GetFirst()->Rect.GetHeight());
			Wnds.GetFirst()->Rect = WindowRect;
		}
		else
		{
		SHVListWndIterator itr(Wnds);
		SHVRect tmpRect;
		SHVDouble pixelWidthOffset;
		SHVDouble pixelHeightOffset;

			// Check if we need to resize the windows first and move them a bit
			if (OrigRect.GetWidth() != WindowRect.GetWidth())
			{
				pixelWidthOffset = double(WindowRect.GetWidth()) / double(OrigRect.GetWidth());
			}
			if (OrigRect.GetHeight() != WindowRect.GetHeight())
			{
				pixelHeightOffset = double(WindowRect.GetHeight()) / double(OrigRect.GetHeight());
			}

			while (itr.MoveNext())
			{
				tmpRect = itr.Get()->Rect;

				if (pixelWidthOffset.IsNull())
				{
					tmpRect.SetX( WindowRect.GetX() + (tmpRect.GetX() - OrigRect.GetX()) );
				}
				else
				{
					tmpRect.SetX( WindowRect.GetX() + int(double(tmpRect.GetX() - OrigRect.GetX()) * pixelWidthOffset + 0.5) );
					tmpRect.SetWidth( int(double(tmpRect.GetWidth()) * pixelWidthOffset + 0.5) );
				}

				if (pixelHeightOffset.IsNull())
				{
					tmpRect.SetY( WindowRect.GetY() + (tmpRect.GetY() - OrigRect.GetY()) );
				}
				else
				{
					tmpRect.SetY( WindowRect.GetY() + int(double(tmpRect.GetY() - OrigRect.GetY()) * pixelHeightOffset + 0.5) );
					tmpRect.SetHeight( int(double(tmpRect.GetHeight()) * pixelHeightOffset + 0.5) );
				}

				if (itr.Get()->FixedWidth)
					tmpRect.SetWidth(itr.Get()->Rect.GetWidth());
				if (itr.Get()->FixedHeight)
					tmpRect.SetHeight(itr.Get()->Rect.GetHeight());

				itr.Get()->Rect = tmpRect;
			}
		}

		Initialized = false;
		WindowRect.SetEmpty();
		OrigRect.SetEmpty();
	}
}

/*************************************
 * ContainsWnd
 *************************************/
bool SHVRegionActionImpl::ContainsWnd(SHVControl* wnd)
{
SHVListWndIterator itr(Wnds);
bool retVal = (wnd == NULL);

	while (!retVal && itr.MoveNext())
	{
		retVal = (itr.Get()->Wnd == wnd);
	}

	return retVal;
}

/*************************************
 * CalculateHMargin
 *************************************/
void SHVRegionActionImpl::CalculateHMargin(int& hmargin)
{
	if (hmargin < 0)
		hmargin = Region.HorizMargin;
	else if (LFUMode)
		hmargin = Region.Font->LFUToWidth(hmargin);
}

/*************************************
 * CalculateVMargin
 *************************************/
void SHVRegionActionImpl::CalculateVMargin(int& vmargin)
{
	if (vmargin < 0)
		vmargin = Region.VertMargin;
	else if (LFUMode)
		vmargin = Region.Font->LFUToHeight(vmargin);
}

/*************************************
 * CalculateRectMargin
 *************************************/
void SHVRegionActionImpl::CalculateRectMargin(SHVRect& margin)
{
	if (margin.GetLeft() < 0)
		margin.SetLeft(Region.HorizMargin);
	else if (LFUMode)
		margin.SetLeft(Region.Font->LFUToWidth(margin.GetLeft()));

	if (margin.GetRight() < 0)
		margin.SetRight(Region.HorizMargin);
	else if (LFUMode)
		margin.SetRight(Region.Font->LFUToWidth(margin.GetRight()));

	if (margin.GetTop() < 0)
		margin.SetTop(Region.VertMargin);
	else if (LFUMode)
		margin.SetTop(Region.Font->LFUToHeight(margin.GetTop()));

	if (margin.GetBottom() < 0)
		margin.SetBottom(Region.VertMargin);
	else if (LFUMode)
		margin.SetBottom(Region.Font->LFUToHeight(margin.GetBottom()));

}
///\endcond
