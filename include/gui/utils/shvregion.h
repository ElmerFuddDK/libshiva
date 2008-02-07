#ifndef __SHIVA_GUI_REGION_H
#define __SHIVA_GUI_REGION_H

#include "../shvcontrolcontainer.h"
#include "shvrect.h"


// forward declare
class SHVRegionAction;


//-=========================================================================================================
/// SHVRegion - Control placement class
/**
 * This class is used for placing controls in a control container
 */

class SHVRegion
{
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
	SHVRegion(SHVControlContainer* container);


	// operators
	operator const SHVRect&() const;


	// properties
	int GetHeight();
	int GetWidth ();


	// Rectangle handling
	void Reset();
	void SetRect(const SHVRect& rect);


	// Start placing a window
	inline SHVRegionAction Move(SHVControl* wnd);


private:
friend class SHVRegionAction;

	///\cond INTERNAL
	SHVControlContainerRef Container;
	SHVControl* Wnd;
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

	SHVRegionAction& AlignLeftRight(SHVControl* left = NULL, SHVControl* right = NULL, int alignment = SHVRegion::AlignLeft, int margin = -1);
	SHVRegionAction& FillLeftRight(SHVControl* left = NULL, SHVControl* right = NULL, int margin = -1);


private:
	///\cond INTERNAL
	SHVRegion& Region;
	SHVControl* Wnd;
	bool Initialized;
	SHVRect WindowRect;
	SHVRegionAction(SHVRegion& region, SHVControl* wnd);
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
SHVRegionAction SHVRegion::Move(SHVControl* wnd)
{
	return SHVRegionAction(*this,wnd);
}
#endif
