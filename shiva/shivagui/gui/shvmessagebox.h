#ifndef __SHIVA_GUIIMPL_MESSAGEBOX_H
#define __SHIVA_GUIIMPL_MESSAGEBOX_H

#include "../../include/gui/shvform.h"
#include "../../include/gui/shvguimanager.h"
#include "../../include/gui/shvcontroledit.h"
#include "../../include/gui/shvcontrolbutton.h"
#include "../../include/framework/shveventsubscriber.h"


//-=========================================================================================================
/// SHVMessageBox - Message box form
/**
 */

class SHVMessageBox : public SHVForm<>
{
public:

	enum Events {
		EventClicked ///< SubID is result, 1 = OK, 0 = cancel
	};

	// Constructor
	static SHVMessageBox* Create(SHVGUIManager* manager, const SHVStringC text, const SHVStringC title, int type = SHVGUIManager::MsgBoxDefault);


	// Set subscriber
	void ResultSubscribe(SHVEventSubscriberBase* resultSubscriber);


	// From SHVForm
	virtual void InitializeForm();

	virtual SHVBool PreClose();


protected:

	SHVMessageBox(SHVGUIManager* manager, SHVControlContainer* controlContainer);

	virtual void OnEvent(SHVEvent* event);

	SHVEventSubscriberBaseRef ResultSubscriber;
	SHVControlEditRef TextBox;
	SHVControlButtonRef OK;
	SHVControlButtonRef Cancel;

	SHVEventSubscriberRef ButtonSubscriber;
	SHVString Text, Title;

	int Type;
};
typedef SHVRefObjectContainer<SHVMessageBox> SHVMessageBoxRef;

#endif
