#ifndef __SHIVA_GUIUTILS_POINT_H
#define __SHIVA_GUIUTILS_POINT_H


//-=========================================================================================================
/// SHVPoint - simple class with x,y coords
/**
 */

class SHVPoint
{
public:


	inline SHVPoint(int _x = 0, int _y = 0);
	
	inline SHVPoint& Move(int _x,int _y);

	int x, y;

};



// ============================================ implementation ============================================ //

/*************************************
 * Constructor
 *************************************/
SHVPoint::SHVPoint(int _x, int _y) : x(_x), y(_y)
{
}

/*************************************
 * Constructor
 *************************************/
SHVPoint& SHVPoint::Move(int _x,int _y)
{
	x += _x;
	y += _y;
	return *this;
}

#endif
