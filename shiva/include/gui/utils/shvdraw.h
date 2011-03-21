#ifndef __SHIVA_GUIUTILS_DRAW_H
#define __SHIVA_GUIUTILS_DRAW_H

#include "../../../include/utils/shvrefobject.h"

#include "../shvcontrol.h"
#include "shvrect.h"
#include "shvbitmap.h"
#include "shvcolor.h"
#include "shvfont.h"
#include "shvpen.h"
#include "shvpoint.h"

// forward declare
class SHVGUIManager;
class SHVDrawEventData;


//-=========================================================================================================
/// SHVDraw - interface for drawing routines
/**
 * Contains drawing routines for drawing into a window.
 */

class SHVDraw : public SHVRefObject
{
public:


	enum TextEnums {
		TextSingleLine = 0, // default
		TextMultiLine = 1,
		TextLeft = 2, // default
		TextRight = 4,
		TextHCenter = TextLeft+TextRight,
		TextTop = 8, // default
		TextBottom = 16,
		TextVCenter = TextTop+TextBottom,
		TextEndEllipsis = 32, // does not work in multi line mode
		TextDefault = TextSingleLine|TextVCenter|TextEndEllipsis
	};

	enum EdgeTypes {
		EdgeTypeSunken,
		EdgeTypeRaised,
		EdgeTypeFlat,
		EdgeTypeFrame,
		EdgeTypeDefault = EdgeTypeSunken
	};

	enum EdgeFlags {
		EdgeFlagLeft = 1,
		EdgeFlagRight = 2,
		EdgeFlagTop = 4,
		EdgeFlagBottom = 8,
		EdgeFlagRect = EdgeFlagLeft|EdgeFlagRight|EdgeFlagTop|EdgeFlagBottom,
		EdgeFlagThick = 16,
		EdgeFlagDefault = EdgeFlagRect
	};


	// Access to the GUI manager
	inline SHVGUIManager* GetManager();


	// Obtain the internal rectangle of a window
	virtual SHVRect GetClientRect(SHVControl* control) = 0;


	// Drawing routines:
	// -----------------

	// DrawLine
	virtual void DrawLine(SHVPoint from, SHVPoint to,  SHVColor* color) = 0;
	virtual void DrawLine(SHVPoint from, SHVPoint to, SHVPen* pen) = 0;

	virtual void DrawXORLine(SHVPoint from, SHVPoint to) = 0;

	// DrawPixel
	virtual void DrawPixel(SHVPoint where, SHVColor* color) = 0;

	// DrawRect
	virtual void DrawRect(SHVRect rect, SHVColor* color) = 0;
	virtual void DrawRectFilled(SHVRect rect, SHVColor* color) = 0;
	inline  void DrawEdge(SHVRect rect, EdgeTypes type = EdgeTypeDefault, int flags = EdgeFlagDefault);
	virtual void DrawEdgeAndShrink(SHVRect& rect, EdgeTypes type = EdgeTypeDefault, int flags = EdgeFlagDefault) = 0;

	virtual void DrawXORRect(SHVRect rect) = 0;

	// DrawPoly
	virtual void DrawPolyline(SHVColor* color, int points, ...) = 0;
	virtual void DrawPolygon(SHVColor* color, int points, ...) = 0;

	// Draw bitmaps
	virtual void DrawBitmap(SHVBitmap* bitmap, SHVPoint position, int width, int height, SHVColor* transparentColor = NULL) = 0;
	virtual void DrawBitmap(SHVBitmap* bitmap, SHVPoint position, SHVColor* transparentColor = NULL) = 0;
	virtual void DrawBitmapCentered(SHVBitmap* bitmap, SHVRect rDest, SHVColor* transparentColor = NULL) = 0;

	// Draw text
	virtual void DrawText(const SHVStringC txt, SHVRect rect, SHVColor* color = NULL, int options = TextDefault) = 0;
	virtual void DrawText(SHVFont* font, const SHVStringC txt, SHVRect rect, SHVColor* color = NULL, int options = TextDefault) = 0;

	// Obtain a SHVDraw* from an EventDraw
	inline static SHVDraw* FromDrawEvent(SHVEvent* event);


protected:

	inline SHVDraw(SHVGUIManager* manager);

	SHVGUIManager* GUIManager;
};
typedef SHVRefObjectContainer<SHVDraw> SHVDrawRef;

#endif


// ============================================ implementation ============================================ //

#ifndef __SHIVA_GUIUTILS_DRAW_INL
#define __SHIVA_GUIUTILS_DRAW_INL

#include "shvdraweventdata.h"

/*************************************
 * Constructor
 *************************************/
SHVDraw::SHVDraw(SHVGUIManager* manager) : GUIManager(manager)
{
}

/*************************************
 * GetManager
 *************************************/
SHVGUIManager* SHVDraw::GetManager()
{
	return GUIManager;
}

/*************************************
 * DrawEdge
 *************************************/
void SHVDraw::DrawEdge(SHVRect rect, SHVDraw::EdgeTypes type, int flags)
{
	DrawEdgeAndShrink(rect,type,flags);
}

/*************************************
 * FromDrawEvent
 *************************************/
SHVDraw* SHVDraw::FromDrawEvent(SHVEvent* event)
{
	return ((SHVDrawEventData*)event->GetData())->Draw;
}

#endif
