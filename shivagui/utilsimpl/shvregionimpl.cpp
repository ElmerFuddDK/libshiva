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
#include "../../include/gui/shvguimanager.h"

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
	if (extraControl && extraControl->IsCreated() && extraControl->GetFlag(SHVControl::FlagVisible))
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

/*************************************
 * UnifyWidth
 *************************************/
SHVRegionAction* SHVRegionActionImpl::UnifyWidth(int unify, int limit)
{
	if (limit >= 0)
	{
		CalculateHMargin(limit);
	}
	else if (unify == SHVRegion::UnifyFixed && limit < 0)
	{
		SHVASSERT(false);
		return this;
	}

	if (Wnds.GetCount())
	{
	SHVListWndIterator itr(Wnds);
	int interimValue;

		Commit();

		// Get unified width
		switch(unify)
		{
		case SHVRegion::UnifyMax:
		case SHVRegion::UnifyMin:
		case SHVRegion::UnifyAverage:
			itr.MoveNext();
			interimValue = itr.Get()->Rect.GetWidth();

			while (itr.MoveNext())
			{
				switch(unify)
				{
				case SHVRegion::UnifyMin:
					if (itr.Get()->Rect.GetWidth() < interimValue)
						interimValue = itr.Get()->Rect.GetWidth();
					break;
				case SHVRegion::UnifyMax:
					if (itr.Get()->Rect.GetWidth() > interimValue)
						interimValue = itr.Get()->Rect.GetWidth();
					break;
				case SHVRegion::UnifyAverage:
					interimValue += itr.Get()->Rect.GetWidth();
					break;
				}
			}

			// calculate the new width
			switch(unify)
			{
			case SHVRegion::UnifyMax:
			case SHVRegion::UnifyMin:
				if (limit >= 0 && interimValue > limit)
					interimValue = limit;
				break;
			case SHVRegion::UnifyAverage:
				interimValue = ((interimValue*10+5)/Wnds.GetCount())/10;
				break;
			}

			break;
		case SHVRegion::UnifyFixed:
			interimValue = limit;
			break;
		}

		// set the width
		while (itr.MoveNext())
		{
			itr.Get()->Rect.SetWidth(interimValue);
		}
	}

	return this;
}

// placement methods
/*************************************
 * Top
 *************************************/
SHVRegionAction* SHVRegionActionImpl::Top(int vmargin)
{
	if (Initialize())
	{
		CalculateVMargin(vmargin);

		WindowRect.SetY( Region.Rect.GetY()+vmargin );
	}

	return this;
}

/*************************************
 * Bottom
 *************************************/
SHVRegionAction* SHVRegionActionImpl::Bottom(int vmargin)
{
	if (Initialize())
	{
		CalculateVMargin(vmargin);

		WindowRect.SetY( Region.Rect.GetBottom()-vmargin-WindowRect.GetHeight() );
	}

	return this;
}

/*************************************
 * Left
 *************************************/
SHVRegionAction* SHVRegionActionImpl::Left(int hmargin)
{
	if (Initialize())
	{
		CalculateHMargin(hmargin);

		WindowRect.SetX( Region.Rect.GetX()+hmargin );
	}

	return this;
}

/*************************************
 * Right
 *************************************/
SHVRegionAction* SHVRegionActionImpl::Right(int hmargin)
{
	if (Initialize())
	{
		CalculateHMargin(hmargin);

		WindowRect.SetX( Region.Rect.GetRight()-hmargin-WindowRect.GetWidth() );
	}

	return this;
}

/*************************************
 * ClipTop
 *************************************/
SHVRegionAction* SHVRegionActionImpl::ClipTop(int vmargin)
{
	if (Initialize())
	{
		CalculateVMargin(vmargin);

		if (WindowRect.GetBottom()+vmargin > Region.Rect.GetTop())
			Region.Rect.SetTop( WindowRect.GetBottom()+vmargin );
	}

	return this;
}

/*************************************
 * ClipBottom
 *************************************/
SHVRegionAction* SHVRegionActionImpl::ClipBottom(int vmargin)
{
	if (Initialize())
	{
		CalculateVMargin(vmargin);

		if (WindowRect.GetTop()-vmargin < Region.Rect.GetBottom())
			Region.Rect.SetBottom( WindowRect.GetTop()-vmargin );
	}

	return this;
}

/*************************************
 * ClipLeft
 *************************************/
SHVRegionAction* SHVRegionActionImpl::ClipLeft(int hmargin)
{
	if (Initialize())
	{
		CalculateHMargin(hmargin);

		if (WindowRect.GetRight()+hmargin > Region.Rect.GetLeft())
			Region.Rect.SetLeft( WindowRect.GetRight()+hmargin );
	}

	return this;
}

/*************************************
 * ClipRight
 *************************************/
SHVRegionAction* SHVRegionActionImpl::ClipRight(int hmargin)
{
	if (Initialize())
	{
		CalculateHMargin(hmargin);

		if (WindowRect.GetLeft()-hmargin < Region.Rect.GetRight())
			Region.Rect.SetRight( WindowRect.GetLeft()-hmargin );
	}

	return this;
}

/*************************************
 * LeftOf
 *************************************/
SHVRegionAction* SHVRegionActionImpl::LeftOf(SHVControl* ctrl, int hmargin)
{
	if (Initialize())
	{
	SHVRect rect(Region.Rect);

		// Make sure you don't use any of the windows in the combined list
		SHVASSERT(ctrl == NULL || !ContainsWnd(ctrl));

		CalculateHMargin(hmargin);

		if (ctrl && ctrl->IsCreated())
		{
		SHVRect rightRect(ctrl->GetRect());

			rect.SetRight(rightRect.GetLeft()-hmargin);
			rect.SetTop(rightRect.GetTop());
		}
		else
		{
			rect.SetTop(rect.GetTop()+Region.VertMargin);
			rect.SetRight(rect.GetLeft()-hmargin);
		}

		rect.SetLeft(rect.GetRight()-WindowRect.GetWidth());
		rect.SetHeight(WindowRect.GetHeight());

		WindowRect = rect;
	}

	return this;
}

/*************************************
 * RightOf
 *************************************/
SHVRegionAction* SHVRegionActionImpl::RightOf(SHVControl* ctrl, int hmargin)
{
	if (Initialize())
	{
	SHVRect rect(Region.Rect);

		// Make sure you don't use any of the windows in the combined list
		SHVASSERT(ctrl == NULL || !ContainsWnd(ctrl));
		
		CalculateHMargin(hmargin);

		if (ctrl && ctrl->IsCreated())
		{
		SHVRect leftRect(ctrl->GetRect());

			rect.SetLeft(leftRect.GetRight()+hmargin);
			rect.SetTop(leftRect.GetTop());
		}
		else
		{
			rect.SetTop(rect.GetTop()+Region.VertMargin);
			rect.SetLeft(rect.GetRight()+hmargin);
		}

		rect.SetWidth(WindowRect.GetWidth());
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
 * AlignHorizontal
 *************************************/
/// Aligns the control between 2 optional controls
SHVRegionAction* SHVRegionActionImpl::AlignHorizontal(SHVControl* left, SHVControl* right, int alignment, int hmargin)
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

		CalculateHMargin(hmargin);

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
			WindowRect.SetLeft( rctRight.GetLeft() - width - hmargin );
			WindowRect.SetWidth( width );
		}
		else if ( (alignment&SHVRegion::AlignLeft) )
		{
			WindowRect.SetLeft( rctLeft.GetRight() + hmargin );
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
				WindowRect.SetTop( bottom - height - hmargin );
				WindowRect.SetHeight( height );
			}
			else if ( (alignment&SHVRegion::AlignTop) )
			{
				WindowRect.SetTop( top + hmargin );
				WindowRect.SetHeight( height );
			}
		}
	}

	return this;
}

/*************************************
 * FillHorizontal
 *************************************/
/// Sizes the control between 2 optional controls, and places it on the X axis
SHVRegionAction* SHVRegionActionImpl::FillHorizontal(SHVControl* left, SHVControl* right, int hmargin)
{
	if (Initialize())
	{
	SHVRect rctLeft, rctRight;

		// Make sure you don't use any of the windows in the combined list
		SHVASSERT(left == NULL || !ContainsWnd(left));
		SHVASSERT(right == NULL || !ContainsWnd(right));

		CalculateHMargin(hmargin);

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

		WindowRect.SetLeft(rctLeft.GetRight() + hmargin);
		WindowRect.SetRight(rctRight.GetLeft() - hmargin);
	}

	return this;
}

/*************************************
 * AlignVertical
 *************************************/
/// Aligns the control between 2 optional controls
SHVRegionAction* SHVRegionActionImpl::AlignVertical(SHVControl* top, SHVControl* bottom, int alignment, int vmargin)
{
	if (Initialize())
	{
	SHVRect rctTop, rctBottom;
	int width, height;

		// Make sure you don't use any of the windows in the combined list
		SHVASSERT(top == NULL || !ContainsWnd(top));
		SHVASSERT(bottom == NULL || !ContainsWnd(bottom));
	
		width = WindowRect.GetWidth();
		height = WindowRect.GetHeight();

		CalculateVMargin(vmargin);

		if (top && top->IsCreated())
		{
			rctTop = top->GetRect();
		}
		else
		{
			rctTop = Region.Rect;
			rctTop.SetBottom(rctTop.GetTop());
		}

		if (bottom && bottom->IsCreated())
		{
			rctBottom = bottom->GetRect();
		}
		else
		{
			rctBottom = Region.Rect;
			rctBottom.SetTop(rctBottom.GetBottom());
		}

		// set the x axis position
		if ( (alignment&SHVRegion::AlignVCenter) == SHVRegion::AlignVCenter )
		{
			WindowRect.SetTop( (rctBottom.GetTop() - rctTop.GetBottom() - height)/2 );
			WindowRect.SetHeight( height );
		}
		else if ( (alignment&SHVRegion::AlignBottom) )
		{
			WindowRect.SetTop( rctBottom.GetTop() - height - vmargin );
			WindowRect.SetHeight( height );
		}
		else if ( (alignment&SHVRegion::AlignTop) )
		{
			WindowRect.SetTop( rctTop.GetBottom() + vmargin );
			WindowRect.SetHeight( height );
		}
	
		// check if we need to align the x axis as well
		if ( (alignment&SHVRegion::AlignLeft) || (alignment&SHVRegion::AlignRight) )
		{
		SHVInt left, right;
			
			if (!top && !bottom)
			{
				left = Region.Rect.GetLeft();
				right = Region.Rect.GetRight();
			}
			else
			{
				if (top)
				{
					if (left.IsNull() || rctTop.GetLeft() < left)
						left = rctTop.GetTop();
					if (right.IsNull() || rctTop.GetRight() > right)
						right = rctTop.GetRight();
				}
				if (bottom)
				{
					if (left.IsNull() || rctBottom.GetLeft() < left)
						left = rctBottom.GetLeft();
					if (right.IsNull() || rctBottom.GetRight() > right)
						right = rctBottom.GetRight();
				}
			}
			
			// set the y axis position
			if ( (alignment&SHVRegion::AlignHCenter) == SHVRegion::AlignHCenter )
			{
				WindowRect.SetLeft( (right - left - width)/2 );
				WindowRect.SetWidth( width );
			}
			else if ( (alignment&SHVRegion::AlignRight) )
			{
				WindowRect.SetLeft( right - width - vmargin );
				WindowRect.SetWidth( width );
			}
			else if ( (alignment&SHVRegion::AlignLeft) )
			{
				WindowRect.SetLeft( left + vmargin );
				WindowRect.SetWidth( width );
			}
		}
	}

	return this;
}

/*************************************
 * FillVertical
 *************************************/
/// Sizes the control between 2 optional controls, and places it on the Y axis
SHVRegionAction* SHVRegionActionImpl::FillVertical(SHVControl* top, SHVControl* bottom, int vmargin)
{
	if (Initialize())
	{
	SHVRect rctTop, rctBottom;

		// Make sure you don't use any of the windows in the combined list
		SHVASSERT(top == NULL || !ContainsWnd(top));
		SHVASSERT(bottom == NULL || !ContainsWnd(bottom));

		CalculateHMargin(vmargin);

		if (top && top->IsCreated())
		{
			rctTop = top->GetRect();
		}
		else
		{
			rctTop = Region.Rect;
			rctTop.SetBottom(rctTop.GetTop());
		}

		if (bottom && bottom->IsCreated())
		{
			rctBottom = bottom->GetRect();
		}
		else
		{
			rctBottom = Region.Rect;
			rctBottom.SetTop(rctBottom.GetBottom());
		}

		WindowRect.SetTop(rctTop.GetBottom() + vmargin);
		WindowRect.SetBottom(rctBottom.GetTop() - vmargin);
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
		bool pixelWidthInf;
		SHVDouble pixelHeightOffset;
		bool pixelHeightInf;

			pixelWidthInf = pixelHeightInf = false;

			// Check if we need to resize the windows first and move them a bit
			if (OrigRect.GetWidth() != WindowRect.GetWidth())
			{
				pixelWidthInf = ( OrigRect.GetWidth() ? false : true );
				if (!pixelWidthInf)
					pixelWidthOffset = double(WindowRect.GetWidth()) / double(OrigRect.GetWidth());
			}
			if (OrigRect.GetHeight() != WindowRect.GetHeight())
			{
				pixelHeightInf = ( OrigRect.GetHeight() ? false : true );
				if (!pixelHeightInf)
					pixelHeightOffset = double(WindowRect.GetHeight()) / double(OrigRect.GetHeight());
			}

			while (itr.MoveNext())
			{
				tmpRect = itr.Get()->Rect;

				if (pixelWidthInf)
				{
					tmpRect.SetLeft(WindowRect.GetLeft());
					tmpRect.SetRight(WindowRect.GetRight());
				}
				else if (pixelWidthOffset.IsNull())
				{
					tmpRect.SetX( WindowRect.GetX() + (tmpRect.GetX() - OrigRect.GetX()) );
				}
				else
				{
					tmpRect.SetX( WindowRect.GetX() + int(double(tmpRect.GetX() - OrigRect.GetX()) * pixelWidthOffset + 0.5) );
					tmpRect.SetWidth( int(double(tmpRect.GetWidth()) * pixelWidthOffset + 0.5) );
				}

				if (pixelHeightInf)
				{
					tmpRect.SetTop(WindowRect.GetTop());
					tmpRect.SetBottom(WindowRect.GetBottom());
				}
				else if (pixelHeightOffset.IsNull())
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
