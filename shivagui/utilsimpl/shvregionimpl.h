#ifndef __SHIVA_GUIIMPL_REGION_H
#define __SHIVA_GUIIMPL_REGION_H

#include "../include/utils/shvregion.h"

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

	virtual SHVRegion* ClipTop(int pixels);
	virtual SHVRegion* ClipBottom(int pixels);
	virtual SHVRegion* ClipLeft(int pixels);
	virtual SHVRegion* ClipRight(int pixels);

	// Start placing a window
	virtual SHVRegionActionPtr Move(SHVControl* wnd);


private:
friend class SHVRegionActionImpl;

	///\cond INTERNAL
	SHVControlContainerRef Container;
	SHVControl* Wnd;
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

	virtual SHVRegionAction* SetPercent(int x = -1, int y = -1, int width = -1, int height = -1, SHVRect margins = SHVRect(-1,-1,-1,-1));

	virtual SHVRegionAction* AlignLeftRight(SHVControl* left = NULL, SHVControl* right = NULL, int alignment = SHVRegion::AlignLeft, int margin = -1);
	virtual SHVRegionAction* FillLeftRight(SHVControl* left = NULL, SHVControl* right = NULL, int margin = -1);


private:
	///\cond INTERNAL
	SHVRegionImpl& Region;
	SHVControl* Wnd;
	bool Initialized;
	SHVRect WindowRect;
	SHVRegionActionImpl(SHVRegionImpl& region, SHVControl* wnd);
	bool Initialize();
	///\endcond
};

#endif
