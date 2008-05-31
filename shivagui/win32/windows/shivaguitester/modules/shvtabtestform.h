#ifndef __SHIVA_GUITESTER_TABTESTFORM_H
#define __SHIVA_GUITESTER_TABTESTFORM_H

#include "../../../../../include/gui/shvform.h"
#include "../../../../../include/gui/shvguimanager.h"


//-=========================================================================================================
/// SHVFormTabTest
/**
 */

class SHVFormTabTest : public SHVForm<>
{
public:


	// Constructor
	SHVFormTabTest(SHVGUIManager* manager, SHVControlContainer* controlContainer);


	virtual void InitializeForm();

	virtual SHVBool SetData(SHVControlData* data);
	virtual SHVBool GetData(SHVControlData* data);

private:
	
	virtual void OnEvent(SHVEvent* event);
	virtual void OnResizeForm(SHVControlContainer* container, SHVControlLayout* layout);

	SHVControlTabRef tbTabs;
};
typedef SHVRefObjectContainer<SHVFormTabTest> SHVFormTabTestRef;

#endif
