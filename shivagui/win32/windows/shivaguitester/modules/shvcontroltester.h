#ifndef __SHIVA_GUITESTER_CONTROLTESTER_H
#define __SHIVA_GUITESTER_CONTROLTESTER_H

#include "../../../../../include/framework/shvmodule.h"
#include "../../../../include/shvguimanager.h"

//-=========================================================================================================
/// SHVControlTester - Module for testing controls
/**
 * Implements a module for testing the generic controls
 */

class SHVControlTester : public SHVModule
{
public:


	// default constructor
	SHVControlTester(SHVModuleList& modules);


	virtual SHVBool Register();
	virtual void PostRegister();
	virtual void Unregister();

	virtual void OnEvent(SHVEvent* event);

private:

	void OnResizeMainWnd(SHVControlContainer* container, SHVControlLayout* layout);

	SHVGUIManager* GUIManager;
	SHVControlLabelRef Label;
	SHVControlEditRef EditBox;
	SHVControlButtonRef Button;
};

#endif
