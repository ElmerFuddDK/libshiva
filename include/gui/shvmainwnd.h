#ifndef __SHIVA_GUI_MAINWND_H
#define __SHIVA_GUI_MAINWND_H

#include "shvcontrolcontainerbase.h"
#include "../../include/framework/shvmodulelist.h"

//-=========================================================================================================
/// SHVMainWnd - base main window control class
/**
 * Inherit from this class if you want to implement a main window.\n
 * Remember there can only be one main window instance per application. For other top level windows
 * use the Dialog based control containers instead.
 */

class SHVMainWnd : public SHVControlContainerBase
{
public:


	// defines and constants
	enum GuiConfigEnums {
		GuiCfgMainWnd,
		GuiCfgHorizMargin,
		GuiCfgVertMargin,
		GuiCfgInstanceHandle // only applicable in win32
	};


	// constructor
	SHVMainWnd(SHVModuleList& modules);

	virtual int GetSubType();


	// GuiConfig property
	virtual SHVConfig& GetConfig();


	// pure virtuals from SHVControlContainer
	virtual const SHVChar* GetControlName() = 0;

protected:
	
	virtual SHVBool CreateInternal(SHVControlContainerBase* parent) = 0;
	
	///\cond INTERNAL
	SHVModuleList& Modules;
	///\endcond
};
typedef SHVRefObjectContainer<SHVMainWnd> SHVMainWndRef;

#endif
