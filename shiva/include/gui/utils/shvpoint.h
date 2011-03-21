#ifndef __SHIVA_GUIUTILS_POINT_H
#define __SHIVA_GUIUTILS_POINT_H


//-=========================================================================================================
/// SHVPointVal - struct for holding a C style point 
/**
 * gcc doesn't like non C type data in ... operators so for portable
 * code use this in SHVDraw ... operators
 */

struct SHVPointVal { int x,y; };


//-=========================================================================================================
/// SHVPoint - simple class with x,y coords
/**
 */

class SHVPoint
{
public:

	inline SHVPoint(const SHVPointVal& val);
	inline SHVPoint(int _x = 0, int _y = 0);
	
	inline SHVPoint& Move(int _x,int _y);

	int x, y;

	inline SHVPointVal Val();
	inline static SHVPointVal Val(int x, int y);

};



// ============================================ implementation ============================================ //

/*************************************
 * Constructor
 *************************************/
SHVPoint::SHVPoint(const SHVPointVal& val) : x(val.x), y(val.y)
{}
SHVPoint::SHVPoint(int _x, int _y) : x(_x), y(_y)
{}

/*************************************
 * Move
 *************************************/
SHVPoint& SHVPoint::Move(int _x,int _y)
{
	x += _x;
	y += _y;
	return *this;
}

/*************************************
 * ToVal
 *************************************/
SHVPointVal SHVPoint::Val()
{
SHVPointVal r={x,y};
	return r;
}

/*************************************
 * Val
 *************************************/
SHVPointVal SHVPoint::Val(int x, int y)
{
SHVPointVal r={x,y};
	return r;
}

#endif
