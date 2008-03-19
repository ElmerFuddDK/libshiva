#ifndef __SHIVA_GUIUTILS_COLOR_H
#define __SHIVA_GUIUTILS_COLOR_H

#include "../../../include/utils/shvrefobject.h"

class SHVGUIManager;


//-=========================================================================================================
/// SHVColor - interface for color
/**
 */

class SHVColor : public SHVRefObject
{
public:


	// Enums and typedefs
	enum MergeColorType {
		MergeTypeLighten,
		MergeTypeTransparency
	};

	typedef unsigned char ColorVal;


	// Properties
	virtual ColorVal GetRed() const = 0;
	virtual ColorVal GetGreen() const = 0;
	virtual ColorVal GetBlue() const = 0;


	// Merge 2 colors into 1 new
	inline static SHVColor* Merge(SHVGUIManager* manager, SHVColor* clr1, SHVColor* clr2, int opacity, MergeColorType mergeType = MergeTypeLighten);


protected:
	virtual SHVColor* MergeInternal(SHVGUIManager* manager, SHVColor* clr2, int opacity, MergeColorType mergeType);
	inline SHVColor() {}
};
typedef SHVRefObjectContainer<SHVColor> SHVColorRef;




// ============================================= implementation ============================================= //

/*************************************
 * Merge
 *************************************/
SHVColor* SHVColor::Merge(SHVGUIManager* manager, SHVColor* clr1, SHVColor* clr2, int opacity, MergeColorType mergeType)
{
	return clr1->MergeInternal(manager,clr2,opacity,mergeType);
}

#endif
