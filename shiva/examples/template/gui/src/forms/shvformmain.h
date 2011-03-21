#ifndef __SHIVA_FORMS_FORMMAIN_H
#define __SHIVA_FORMS_FORMMAIN_H

#include "shiva/include/gui/shvform.h"
#include "shiva/include/gui/shvguimanager.h"

//-=========================================================================================================
/// SHVFormMain class
/**
 */

class SHVFormMain : public SHVForm<>
{
public:

	SHVFormMain(SHVGUIManager* manager, SHVControlContainer* controlContainer);

	virtual void InitializeForm();

	// For future use ...
	virtual SHVBool SetData(SHVDataBinder* data);
	virtual SHVBool GetData(SHVDataBinder* data);


private:

	virtual void OnResizeForm(SHVControlContainer* container, SHVControlLayout* layout);
	
	SHVControlLabelRef Label;
};
typedef SHVRefObjectContainer<SHVFormMain> SHVFormMainRef;

#endif