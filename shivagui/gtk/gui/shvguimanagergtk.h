#ifndef __SHIVA_GTKGUI_MANAGER_H
#define __SHIVA_GTKGUI_MANAGER_H

#include "../../gui/shvguimanagerimpl.h"

#include <gtk/gtk.h>

// forward declares
//class SHVDrawGtk;


//-=========================================================================================================
/// SHVGUIManagerGtk - Module for registering control types, and instantiating them
/**
 * This class is a SHIVA module that can be used to keeping default control factories, as well as
 * allowing custom control factories to be registered.
 */

class SHVGUIManagerGtk : public SHVGUIManagerImpl
{
public:


	// Constructor
	SHVGUIManagerGtk(SHVModuleList& modules);


	// From SHVModule
	virtual SHVBool Register();
	virtual void Unregister();

	// CreateFont
	virtual SHVFont* CreateFont(const SHVStringC name, int height, int styles = SHVFont::StyleNormal);

	// Factories
	virtual SHVColor* CreateColor(SHVColor::ColorVal r, SHVColor::ColorVal g, SHVColor::ColorVal b);
	virtual SHVPen* CreatePen(SHVColor* color, int style = SHVPen::StyleDefault, int width = 1);
	virtual SHVBrush* CreateBrush(SHVColor* color, int style = SHVBrush::StyleDefault);
	virtual SHVRegion* CreateRegion(SHVControlContainer* container);
	virtual SHVBitmap* CreateBitmap(SHVInt bitmapID);
	virtual SHVMenu* CreatePopupMenu(SHVEventSubscriberBase* subscriber, SHVControl* parent);

	// Factories for template inherited classes
	virtual SHVFormImplementer* ConstructFormImplementer(SHVFormBase* owner, SHVGUIManager* manager, SHVControlContainer* controlContainer, SHVString8C entityName);

	// Additional control events
	virtual SHVBool ControlEventSubscribe(SHVInt controlEvent, SHVControl* control, SHVEventSubscriberBase* subscriber);
	virtual void ClearControlEvents(SHVControl* control);


protected:
	virtual void EmitControlEvent(SHVControl* control, SHVInt controlEvent);
	///\cond INTERNAL
	friend class SHVMainThreadEventDispatcherGtk;
	void SetStandardFonts(SHVFont* font);
	///\endcond
};

#endif
