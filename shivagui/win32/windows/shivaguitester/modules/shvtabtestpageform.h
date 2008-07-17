#ifndef __SHIVA_GUITESTER_TABTESTPAGEFORM_H
#define __SHIVA_GUITESTER_TABTESTPAGEFORM_H

#include "../../../../../include/gui/shvform.h"
#include "../../../../../include/gui/shvguimanager.h"


//-=========================================================================================================
/// SHVFormTabTestPage
/**
 */

class SHVFormTabTestPage : public SHVForm<>
{
public:


	// Constructor
	SHVFormTabTestPage(SHVGUIManager* manager, SHVControlContainer* controlContainer, const SHVStringC labelStr);


	virtual void InitializeForm();

	virtual SHVBool SetData(SHVDataBinder* data);
	virtual SHVBool GetData(SHVDataBinder* data);

private:
	
	virtual void OnEvent(SHVEvent* event);
	virtual void OnResizeForm(SHVControlContainer* container, SHVControlLayout* layout);

	SHVString LabelStr;

	SHVControlLabelRef Label;
	SHVControlEditRef EditBox;
	SHVControlButtonRef Button;
	SHVControlLabelRef lblLongDate, lblShortDate, lblTime;
	SHVControlDateTimeRef dtLongDate, dtShortDate, dtTime;
	SHVControlListViewRef lvData;

};
typedef SHVRefObjectContainer<SHVFormTabTestPage> SHVFormTabTestPageRef;

#endif
