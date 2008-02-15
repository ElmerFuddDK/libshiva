#ifndef __SHIVA_GUIUTILS_COLOR_H
#define __SHIVA_GUIUTILS_COLOR_H

#include "../../../include/utils/shvrefobject.h"


//-=========================================================================================================
/// SHVColor - interface for color
/**
 */

class SHVColor : public SHVRefObject
{
public:


	typedef unsigned char ColorVal;


	virtual ColorVal GetRed() const = 0;
	virtual ColorVal GetGreen() const = 0;
	virtual ColorVal GetBlue() const = 0;


protected:
	inline SHVColor() {}
};
typedef SHVRefObjectContainer<SHVColor> SHVColorRef;


#endif
