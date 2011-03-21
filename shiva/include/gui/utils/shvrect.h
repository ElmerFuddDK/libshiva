#ifndef __SHIVA_GUIUTILS_RECT_H
#define __SHIVA_GUIUTILS_RECT_H


//-=========================================================================================================
/// SHVRect - Rectangle for the GUI
/**
 * Used for getting and manipulating dimensions of controls.
 */

class SHVAPI SHVRect
{
public:


	// default constructor
	SHVRect();
	SHVRect(int left, int top, int right, int bottom);


	// operators
	bool operator==(const SHVRect& rect) const;
	bool operator!=(const SHVRect& rect) const;


	// properties
	inline int  GetX() const;
	inline void SetX(int x);

	inline int  GetY() const;
	inline void SetY(int y);

	inline int  GetLeft() const;
	inline void SetLeft(int left);

	inline int  GetTop() const;
	inline void SetTop(int top);

	inline int  GetRight() const;
	inline void SetRight(int right);

	inline int  GetBottom() const;
	inline void SetBottom(int bottom);

	inline int  GetWidth() const;
	inline void SetWidth(int width);

	inline int  GetHeight() const;
	inline void SetHeight(int height);


	// composite set
	void SetByXY(int x, int y, int width, int height);
	void SetByLeftTop(int left, int top, int right, int bottom);

	void MoveTo(int x, int y);
	void MoveBy(int x, int y);

	void Shrink(int left, int top, int right, int bottom);
	inline void Expand(int left, int top, int right, int bottom);

	inline bool IsEmpty() const;
	inline void SetEmpty();

private:
	///\cond INTERNAL
	int Left,Top,Right,Bottom;
	///\endcond
};


// ============================================= implementation ============================================= //

/*************************************
 * X
 *************************************/
int  SHVRect::GetX() const
{
	return Left;
}
void SHVRect::SetX(int x)
{
int width = GetWidth();

	Left = x;
	Right = Left + width;
}

/*************************************
 * Y
 *************************************/
int  SHVRect::GetY() const
{
	return Top;
}
void SHVRect::SetY(int y)
{
int height = GetHeight();

	Top = y;
	Bottom = Top + height;
}

/*************************************
 * Left
 *************************************/
int  SHVRect::GetLeft() const
{
	return Left;
}
void SHVRect::SetLeft(int left)
{
	Left = left;
}

/*************************************
 * Top
 *************************************/
int  SHVRect::GetTop() const
{
	return Top;
}
void SHVRect::SetTop(int top)
{
	Top = top;
}

/*************************************
 * Right
 *************************************/
int  SHVRect::GetRight() const
{
	return Right;
}
void SHVRect::SetRight(int right)
{
	Right = right;
}

/*************************************
 * Bottom
 *************************************/
int  SHVRect::GetBottom() const
{
	return Bottom;
}
void SHVRect::SetBottom(int bottom)
{
	Bottom = bottom;
}

/*************************************
 * Width
 *************************************/
int  SHVRect::GetWidth() const
{
	return Right - Left;
}
void SHVRect::SetWidth(int width)
{
	Right = Left + width;
}

/*************************************
 * Height
 *************************************/
int  SHVRect::GetHeight() const
{
	return Bottom - Top;
}
void SHVRect::SetHeight(int height)
{
	Bottom = Top + height;
}

/*************************************
 * Expand
 *************************************/
void SHVRect::Expand(int left, int top, int right, int bottom)
{
	Shrink(left*-1,top*-1,right*-1,bottom*-1);
}

/*************************************
 * IsEmpty
 *************************************/
bool SHVRect::IsEmpty() const
{
	return (!Left && !Top && !Right && !Bottom);
}

/*************************************
 * SetEmpty
 *************************************/
void SHVRect::SetEmpty()
{
	Left   = 0;
	Top    = 0;
	Right  = 0;
	Bottom = 0;
}

#endif
