#ifndef __SHIVA_GUIUTILS_PEN_H
#define __SHIVA_GUIUTILS_PEN_H

#include "../../../include/utils/shvrefobject.h"
#include "shvcolor.h"

//-=========================================================================================================
/// SHVPen - interface for pen for drawing lines
/**
 */

class SHVPen : public SHVRefObject
{
public:


	enum Styles {
		StyleSolid,
		StyleDefault = StyleSolid
	};


	virtual SHVColor* GetColor() = 0;


protected:
	inline SHVPen() {}
};
typedef SHVRefObjectContainer<SHVPen> SHVPenRef;


#endif
