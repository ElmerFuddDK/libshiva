#ifndef __SHIVA_WIN32GUI_MANAGER_H
#define __SHIVA_WIN32GUI_MANAGER_H

#include "../../gui/shvguimanagerimpl.h"


//-=========================================================================================================
/// SHVGUIManagerWin32 - Module for registering control types, and instantiating them
/**
 * This class is a SHIVA module that can be used to keeping default control factories, as well as
 * allowing custom control factories to be registered.
 */

class SHVGUIManagerWin32 : public SHVGUIManagerImpl
{
public:


	// Constructor
	SHVGUIManagerWin32(SHVModuleList& modules);


	// From SHVModule
	virtual SHVBool Register();
	virtual void Unregister();


	// PreTranslateMessage
	virtual bool PreTranslateMessage(MSG* message);


protected:
	///\cond INTERNAL
	///\endcond
};

#endif
