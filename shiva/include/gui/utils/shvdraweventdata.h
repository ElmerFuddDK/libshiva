#ifndef __SHIVA_GUIUTILS_DRAWEVENTDATA_H
#define __SHIVA_GUIUTILS_DRAWEVENTDATA_H

#include "../../../include/utils/shvrefobject.h"
#include "shvdraw.h"


//-=========================================================================================================
/// SHVDrawEventData - placeholder for extended data
/**
 * Can contain extended data for a draw event. Each control that needs to extend
 * data transmitted on a draw event should create a class that inherits from this
 * and extend it with the data necessary. Then it should implement an static
 * function that retrieves the data.
 */

class SHVDrawEventData
{
public:

	inline SHVDrawEventData(SHVDraw* draw);

	SHVDrawRef Draw;
};


// ============================================ implementation ============================================ //

/*************************************
 * Constructor
 *************************************/
SHVDrawEventData::SHVDrawEventData(SHVDraw* draw) : Draw(draw)
{
}

#endif
