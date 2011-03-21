#ifndef __SHIVA_GUIUTILS_BRUSH_H
#define __SHIVA_GUIUTILS_BRUSH_H

#include "../../../include/utils/shvrefobject.h"
#include "shvcolor.h"

//-=========================================================================================================
/// SHVBrush - interface for brush for filling areas
/**
 */

class SHVBrush : public SHVRefObject
{
public:


	enum Styles {
		StyleSolid,
		StyleDefault = StyleSolid
	};


	virtual SHVColor* GetColor() = 0;


protected:
	inline SHVBrush() {}
};
typedef SHVRefObjectContainer<SHVBrush> SHVBrushRef;


#endif
