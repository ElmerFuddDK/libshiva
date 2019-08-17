#ifndef __SHIVA_GUITESTER_FORMMAIN_H
#define __SHIVA_GUITESTER_FORMMAIN_H

#include "shiva/include/gui/shvform.h"
#include "shiva/include/gui/shvguimanager.h"


//-=========================================================================================================
/// SHVFormTest
/**
 */


class SHVFormTest : public SHVForm<>
{
protected:

	enum Menus {
		MenuQuit = 1
	};
	
	SHVFormTest(SHVGUIManager* manager, SHVControlContainer* controlContainer);
	
public:

	void OnEvent(SHVEvent* event);
	void OnMenuEvent(SHVEvent* event);
	
	// Constructor
	static SHVFormTest* Create(SHVGUIManager* manager);

	// From SHVForm
	void InitializeForm();
	SHVBool PreClose();

private:
	
	SHVControlContainerRef Container;
	SHVControlEditRef TextBox;
	SHVControlButtonRef OK;
	SHVControlButtonRef Cancel;
	SHVInt Seed;

	SHVEventSubscriberRef ButtonSubscriber;
};
typedef SHVRefObjectContainer<SHVFormTest> SHVFormTestRef;

#endif
