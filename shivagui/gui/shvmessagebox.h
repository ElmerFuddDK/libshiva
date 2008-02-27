#ifndef __SHIVA_GUIIMPL_MESSAGEBOX_H
#define __SHIVA_GUIIMPL_MESSAGEBOX_H

#include "../include/shvform.h"
#include "../include/shvguimanager.h"
#include "../include/shvcontroledit.h"
#include "../include/shvcontrolbutton.h"
#include "../../include/framework/shveventsubscriber.h"


//-=========================================================================================================
/// SHVMessageBox - Message box form
/**
 */

class SHVMessageBox : public SHVForm
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
	virtual void InitializeForm(SHVControlLayout* layout);

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
};
typedef SHVRefObjectContainer<SHVMessageBox> SHVMessageBoxRef;

#endif
