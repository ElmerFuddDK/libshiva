#ifndef __SHIVA_GUITESTER_CONTROLTESTER_H
#define __SHIVA_GUITESTER_CONTROLTESTER_H

#include "shiva/include/framework/shvmodule.h"
#include "shiva/include/gui/shvguimanager.h"

#include "shvformtest.h"
#include "shvtabtestform.h"

//-=========================================================================================================
/// SHVControlTester - Module for testing controls
/**
 * Implements a module for testing the generic controls
 */

class SHVControlTester : public SHVModule
{
public:

	enum Menus {
		MenuTestTab = 1,
		MenuQuit
	};


	// default constructor
	SHVControlTester(SHVModuleList& modules);


	virtual SHVBool Register();
	virtual void PostRegister();
	virtual void Unregister();

	virtual void OnEvent(SHVEvent* event);

private:

	
	void OnResize(SHVControlContainer* container, SHVControlLayout* /*layout*/);
	void OnResizeDummy(SHVControlContainer* /*container*/, SHVControlLayout* /*layout*/);
	void OnPopupMenu(SHVEvent* event);
	void OnClick(SHVEvent* event);
	void OnCustomDraw(SHVEvent* event);
	void OnMenuEvent(SHVEvent* event);
	void OnComboBoxEvent(SHVEvent* event);

	SHVGUIManager* GUIManager;

	SHVControlCheckboxRef Checkbox;
	SHVControlLabelRef Label;
	SHVControlButtonRef Button;
	SHVControlComboBoxRef ComboBox;
	SHVControlEditRef Edit;
	SHVControlListViewRef ListView;
	SHVFormTestRef NewWindow;
	SHVFormTabTestRef TabTest;
	int Counter;
};

#endif
