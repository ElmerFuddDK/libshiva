#ifndef __SHIVA_GUI_REGION_H
#define __SHIVA_GUI_REGION_H

#include "../shvcontrolcontainer.h"
#include "shvrect.h"
#include "../../../include/utils/shvptrcontainer.h"
#include "../../../include/utils/shvrefobject.h"


// forward declare
class SHVRegionAction;
typedef SHVPtrContainer<SHVRegionAction> SHVRegionActionPtr;


//-=========================================================================================================
/// SHVRegion - Control placement class
/**
 * This class is used for placing controls in a control container
 */

class SHVRegion : public SHVRefObject
{
public:


	// constants
	enum Alignments
	{
		AlignNone = 0,
		AlignLeft = 1,
		AlignRight = 2,
		AlignHCenter = AlignLeft+AlignRight,
		AlignTop = 16,
		AlignBottom = 32,
		AlignVCenter = AlignTop+AlignBottom
	};


	// operators
	operator const SHVRect&() const;


	// properties
	virtual int GetHeight() = 0;
	virtual int GetWidth () = 0;


	// Rectangle handling
	virtual void Reset() = 0;
	virtual void SetRect(const SHVRect& rect) = 0;

	// Margin handling
	virtual SHVRegion* SetMargin(int hmargin, int vmargin) = 0;
	virtual SHVRegion* SetMarginInPixels(int hmargin, int vmargin) = 0;


	// Clipping functions
	// ------------------
	virtual SHVRegion* ClipTop(int lfuheight) = 0;
	virtual SHVRegion* ClipBottom(int lfuheight) = 0;
	virtual SHVRegion* ClipLeft(int lfuwidth) = 0;
	virtual SHVRegion* ClipRight(int lfuwidth) = 0;
	virtual SHVRegion* ClipTopInPixels(int pixels) = 0;
	virtual SHVRegion* ClipBottomInPixels(int pixels) = 0;
	virtual SHVRegion* ClipLeftInPixels(int pixels) = 0;
	virtual SHVRegion* ClipRightInPixels(int pixels) = 0;


	// Start placing a window
	virtual SHVRegionActionPtr Move(SHVControl* wnd) = 0;
	virtual SHVRegionActionPtr MoveInPixels(SHVControl* wnd) = 0;


protected:
	inline SHVRegion() {}
};
typedef SHVRefObjectContainer<SHVRegion> SHVRegionRef;


//-=========================================================================================================
/// SHVRegionAction - Performs actions against a region and its selected window
/**
 * This class is returned by a region when the user wants to perform actions
 * against the region that involves a window.
 */

class SHVRegionAction
{
public:


	// add a control to a combined sub region
	virtual SHVRegionAction* And(SHVControl* extraControl) = 0;


	// Sets boundaries for the recently added control
	virtual SHVRegionAction* CtrlMaxWidth(int width) = 0;
	virtual SHVRegionAction* CtrlWidth(int width) = 0;
	virtual SHVRegionAction* CtrlFixedWidth() = 0;
	virtual SHVRegionAction* CtrlFixedHeight() = 0;


	// placement methods
	virtual SHVRegionAction* Top(int leftMargin = -1, int topMargin = -1) = 0;
	virtual SHVRegionAction* Bottom(int leftMargin = -1, int topMargin = -1) = 0;
	virtual SHVRegionAction* Left(int leftMargin = -1, int topMargin = -1) = 0;
	virtual SHVRegionAction* Right(int leftMargin = -1, int topMargin = -1) = 0;

	virtual SHVRegionAction* ClipTop(int extraMargin = 0) = 0;
	virtual SHVRegionAction* ClipBottom(int extraMargin = 0) = 0;
	virtual SHVRegionAction* ClipLeft(int extraMargin = 0) = 0;
	virtual SHVRegionAction* ClipRight(int extraMargin = 0) = 0;

	virtual SHVRegionAction* LeftOf(SHVControl* left = NULL, int leftMargin = -1) = 0;
	virtual SHVRegionAction* RightOf(SHVControl* right = NULL, int rightMargin = -1) = 0;

	virtual SHVRegionAction* FillPercent(int x = -1, int y = -1, int width = -1, int height = -1, SHVRect margins = SHVRect(-1,-1,-1,-1)) = 0;

	virtual SHVRegionAction* AlignLeftRight(SHVControl* left = NULL, SHVControl* right = NULL, int alignment = SHVRegion::AlignLeft, int margin = -1) = 0;
	virtual SHVRegionAction* FillLeftRight(SHVControl* left = NULL, SHVControl* right = NULL, int margin = -1) = 0;


	// desctructor
	virtual ~SHVRegionAction() {}

};

#endif
