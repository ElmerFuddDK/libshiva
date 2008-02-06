#ifndef __SHIVA_GUI_REGION_H
#define __SHIVA_GUI_REGION_H

#include "shvcontrolcontainerbase.h"
#include "shvrect.h"


class SHVRegionAction;


//-=========================================================================================================
/// SHVRegion - Control placement class
/**
 * This class is used for placing controls in a control container
 */

class SHVRegion
{
friend class SHVRegionAction;
public:


	// constants
	enum Alignments
	{
		AlignNone = 0,
		AlignLeft = 1,
		AlignRight = 2,
		AlignHorizCenter = AlignLeft+AlignRight,
		AlignTop = 16,
		AlignBottom = 32,
		AlignVertCenter = AlignTop+AlignBottom
	};


	// default constructor
	SHVRegion(SHVControlContainerBase* container, SHVConfig& guiConfig);


	// operators
	operator const SHVRect&() const;


	// properties
	int GetHeight();
	int GetWidth ();


	// Rectangle handling
	void Reset();
	void SetRect(const SHVRect& rect);


	// Start placing a window
	inline SHVRegionAction Move(SHVControlBase* wnd);


private:
	///\cond INTERNAL
	SHVControlContainerBaseRef Container;
	SHVConfig& GuiConfig;
	SHVControlBase* Wnd;
	int HorizMargin, VertMargin;
	SHVRect Rect;
	#ifdef __SHIVA_WIN32
	#elif defined(__SHIVA_EPOC)
	#else
	#endif
	///\endcond
};


//-=========================================================================================================
/// SHVRegionAction - Performs actions against a region and its selected window
/**
 * This class is returned by a region when the user wants to perform actions
 * against the region that involves a window.
 */

class SHVRegionAction
{
friend class SHVRegion;
public:


	// desctructor
	~SHVRegionAction();


	// placement methods
	SHVRegionAction& Top(int leftMargin = -1, int topMargin = -1);
	SHVRegionAction& Bottom(int leftMargin = -1, int topMargin = -1);
	SHVRegionAction& Left(int leftMargin = -1, int topMargin = -1);
	SHVRegionAction& Right(int leftMargin = -1, int topMargin = -1);

	SHVRegionAction& AlignLeftRight(SHVControlBase* left = NULL, SHVControlBase* right = NULL, int alignment = SHVRegion::AlignLeft, int margin = -1);
	SHVRegionAction& FillLeftRight(SHVControlBase* left = NULL, SHVControlBase* right = NULL, int margin = -1);


private:
	///\cond INTERNAL
	SHVRegion& Region;
	SHVControlBase* Wnd;
	bool Initialized;
	SHVRect WindowRect;
	SHVRegionAction(SHVRegion& region, SHVControlBase* wnd);
	bool Initialize();
	///\endcond
};



// ============================================= implementation ============================================= //

/*************************************
 * Move
 *************************************/
/// Initializes placement of a window in the region
/**
 * This function will return an action class that can be
 * used to directly manipulate the region with the provided
 * window.\n
 * Here is an example as to how this works:
\code
	{
	SHVRegion rgn(container,guiConfig);

		rgn.Move(button).Top().FillLeftRight();
	}
\endcode
 */
SHVRegionAction SHVRegion::Move(SHVControlBase* wnd)
{
	return SHVRegionAction(*this,wnd);
}
#endif
