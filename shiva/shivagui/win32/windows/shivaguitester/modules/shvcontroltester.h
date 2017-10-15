#ifndef __SHIVA_GUITESTER_CONTROLTESTER_H
#define __SHIVA_GUITESTER_CONTROLTESTER_H

#include "../../../../../include/framework/shvmodule.h"
#include "../../../../../include/gui/shvguimanager.h"

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

	void OnResizeContainer(SHVControlContainer* container, SHVControlLayout* layout);
	void OnResizeControls(SHVControlContainer* container, SHVControlLayout* layout);
	void OnDrawLabel(SHVEvent* event);
	void OnDrawContainer(SHVEvent* event);

	SHVGUIManager* GUIManager;
	SHVControlContainerRef Container;
	SHVControlLabelRef Label;
	SHVControlLabelRef LabelCustomDraw;
	SHVControlEditRef EditBox;
	SHVControlButtonRef Button;
	SHVControlComboBoxRef ComboBox;
	SHVControlCheckboxRef Checkbox;
	SHVBitmapRef Bitmap;

	SHVFormBaseRef TestDialog;
};

#endif
