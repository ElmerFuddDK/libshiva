#ifndef __SHIVA_GUIIMPL_REGION_H
#define __SHIVA_GUIIMPL_REGION_H

#include "../include/utils/shvregion.h"
#include "../include/utils/shvfont.h"
#include "../../include/utils/shvlist.h"

// forward declare
class SHVRegionActionImpl;


//-=========================================================================================================
/// SHVRegionImpl - Control placement class
/**
 * This class is used for placing controls in a control container
 */

class SHVRegionImpl : public SHVRegion
{
public:


	// default constructor
	SHVRegionImpl(SHVControlContainer* container);


	// operators
	virtual operator const SHVRect&() const;


	// properties
	virtual int GetHeight();
	virtual int GetWidth ();


	// Rectangle handling
	virtual void Reset();
	virtual void SetRect(const SHVRect& rect);

	// Margin handling
	virtual SHVRegion* SetMargin(int hmargin, int vmargin);
	virtual SHVRegion* SetMarginInPixels(int hmargin, int vmargin);


	// Clipping functions
	// ------------------
	virtual SHVRegion* ClipTop(int lfuheight);
	virtual SHVRegion* ClipBottom(int lfuheight);
	virtual SHVRegion* ClipLeft(int lfuwidth);
	virtual SHVRegion* ClipRight(int lfuwidth);
	virtual SHVRegion* ClipTopInPixels(int pixels);
	virtual SHVRegion* ClipBottomInPixels(int pixels);
	virtual SHVRegion* ClipLeftInPixels(int pixels);
	virtual SHVRegion* ClipRightInPixels(int pixels);

	// Start placing a window
	virtual SHVRegionActionPtr Move(SHVControl* wnd);
	virtual SHVRegionActionPtr MoveInPixels(SHVControl* wnd);


private:
friend class SHVRegionActionImpl;

	///\cond INTERNAL
	SHVControlContainerRef Container;
	SHVControl* Wnd;
	SHVFontRef Font;
	int HorizMargin, VertMargin;
	SHVRect Rect;
	///\endcond
};


//-=========================================================================================================
/// SHVRegionAction - Performs actions against a region and its selected window
/**
 * This class is returned by a region when the user wants to perform actions
 * against the region that involves a window.
 */

class SHVRegionActionImpl : public SHVRegionAction
{
friend class SHVRegionImpl;
public:


	// desctructor
	virtual ~SHVRegionActionImpl();


	// add a control to a combined sub region
	virtual SHVRegionAction* And(SHVControl* extraControl);


	// Sets boundaries for the recently added control
	virtual SHVRegionAction* CtrlMaxWidth(int width);
	virtual SHVRegionAction* CtrlWidth(int width);
	virtual SHVRegionAction* CtrlFixedWidth();
	virtual SHVRegionAction* CtrlFixedHeight();


	// placement methods
	virtual SHVRegionAction* Top(int leftMargin = -1, int topMargin = -1);
	virtual SHVRegionAction* Bottom(int leftMargin = -1, int topMargin = -1);
	virtual SHVRegionAction* Left(int leftMargin = -1, int topMargin = -1);
	virtual SHVRegionAction* Right(int leftMargin = -1, int topMargin = -1);

	virtual SHVRegionAction* ClipTop(int extraMargin = 0);
	virtual SHVRegionAction* ClipBottom(int extraMargin = 0);
	virtual SHVRegionAction* ClipLeft(int extraMargin = 0);
	virtual SHVRegionAction* ClipRight(int extraMargin = 0);

	virtual SHVRegionAction* LeftOf(SHVControl* left = NULL, int leftMargin = -1);
	virtual SHVRegionAction* RightOf(SHVControl* right = NULL, int rightMargin = -1);

	virtual SHVRegionAction* FillPercent(int x = -1, int y = -1, int width = -1, int height = -1, SHVRect margins = SHVRect(-1,-1,-1,-1));

	virtual SHVRegionAction* AlignLeftRight(SHVControl* left = NULL, SHVControl* right = NULL, int alignment = SHVRegion::AlignLeft, int margin = -1);
	virtual SHVRegionAction* FillLeftRight(SHVControl* left = NULL, SHVControl* right = NULL, int margin = -1);


private:
	///\cond INTERNAL
	SHVRegionImpl& Region;
	bool Initialized, LFUMode;
	SHVRect WindowRect, OrigRect;

	// struct for caching values assosiated with a control whilst moving
	struct Control {
		SHVControl* Wnd;
		SHVRect Rect;
		SHVInt MaxWidth;
		SHVInt MaxHeight;
		bool FixedWidth, FixedHeight;
		Control(SHVControl* wnd);
	};
	typedef SHVPtrContainer<Control> ControlPtr;

	SHVList<ControlPtr,Control*> Wnds;
	typedef SHVListIterator<ControlPtr,Control*> SHVListWndIterator;

	SHVRegionActionImpl(SHVRegionImpl& region, SHVControl* wnd, bool lfumode);

	bool Initialize();
	void Commit();
	bool ContainsWnd(SHVControl* wnd);

	void CalculateHMargin(int& hmargin);
	void CalculateVMargin(int& vmargin);
	void CalculateRectMargin(SHVRect& margin);
	///\endcond
};

#endif
