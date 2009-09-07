#ifndef __SHIVA_GUI_MANAGER_H
#define __SHIVA_GUI_MANAGER_H

#include "../../include/framework/shvmodule.h"
#include "shvcontrolcreatorbase.h"
#include "shvcontrolcontainer.h"
#include "shvcontroledit.h"
#include "shvcontrollabel.h"
#include "shvcontrolbutton.h"
#include "shvcontroldatetime.h"
#include "shvcontrolcombobox.h"
#include "shvcontroltab.h"
#include "shvcontrollistview.h"
#include "shvformimplementer.h"
#include "shvformbase.h"
#include "utils/shvfont.h"
#include "utils/shvbitmap.h"
#include "utils/shvcolor.h"
#include "utils/shvpen.h"
#include "utils/shvbrush.h"
#include "utils/shvregion.h"
#include "utils/shvdraw.h"



//-=========================================================================================================
/// SHVGUIManager - Module for registering control types, and instantiating them
/**
 * This class is a SHIVA module that can be used to keeping default control factories, as well as
 * allowing custom control factories to be registered.
 */

class SHVGUIManager : public SHVModule
{
public:


	// enums
	enum {
		// Standard config options
		CfgInstanceHandle,
		CfgRegionHorizMargin,
		CfgRegionVertMargin,
		CfgColorTransparent,

		// Font options
		CfgFontNormal = 900,
		CfgFontNormalBold,
		CfgFontLarge,
		CfgFontLargeBold,

		// Native option seed
		CfgNativeOptions = 1000
	};

	enum Events {
		EventPreTranslateMessage,
		EventControl
	};

	enum MsgBoxTypes {
		MsgBoxOK,
		MsgBoxOKCancel,
		MsgBoxDefault = MsgBoxOK
	};

	enum AdditionalControlEvents {
		CtrlEventContainerDestroy,
		CtrlEventContainerRectChanged
	};


	// Show a message box
	virtual SHVBool ShowMessageBox( const SHVStringC text,
									const SHVStringC title = NULL,
									int type = MsgBoxDefault,
									SHVEventSubscriberBase* resultSubscriber = NULL) = 0;

	// Control creation functions
	inline SHVControlContainer* NewContainer(int subType = SHVControlContainer::SubTypeDefault);
	inline SHVControlContainer* NewContainerCustomDraw(SHVEventSubscriberBase* subscriber);
	inline SHVControlContainer* NewDialog();
	inline SHVControlContainer* NewModalDialog();
	inline SHVControlEdit* NewEdit(int subType = SHVControlEdit::SubTypeDefault);
	inline SHVControlLabel* NewLabel(int subType = SHVControlLabel::SubTypeDefault);
	inline SHVControlLabel* NewLabelCustomDraw(SHVEventSubscriberBase* subscriber);
	inline SHVControlButton* NewButton(int subType = SHVControlButton::SubTypeDefault);
	inline SHVControlTab* NewTab(int subType = SHVControlTab::SubTypeDefault);
	inline SHVControlDateTime* NewDateTime(int subType = SHVControlDateTime::SubTypeDefault);
	inline SHVControlComboBox* NewComboBox(int subType = SHVControlDateTime::SubTypeDefault);
	inline SHVControlListView* NewListView(int subType = SHVControlListView::SubTypeDefault);
	inline SHVControlListView* NewListViewCustomDraw(SHVEventSubscriberBase* subscriber);

	// Font functions
	inline SHVFont* GetFont(int cfgID);
	virtual SHVFont* CreateFont(const SHVStringC name, int height, int styles = SHVFont::StyleNormal) = 0;

	// Obtain default transparent color
	inline SHVColor* GetTransparentColor();

	// Factories
	virtual SHVColor* CreateColor(SHVColor::ColorVal r, SHVColor::ColorVal g, SHVColor::ColorVal b) = 0;
	virtual SHVPen* CreatePen(SHVColor* color, int style = SHVPen::StyleDefault, int width = 1) = 0;
	virtual SHVBrush* CreateBrush(SHVColor* color, int style = SHVBrush::StyleDefault) = 0;
	virtual SHVRegion* CreateRegion(SHVControlContainer* container) = 0;
	virtual SHVBitmap* CreateBitmap(SHVInt bitmapID) = 0;
	virtual SHVMenu* CreatePopupMenu(SHVEventSubscriberBase* subscriber, SHVControl* parent) = 0;

	// Factories for template inherited classes
	virtual SHVFormImplementer* ConstructFormImplementer(SHVFormBase* owner, SHVGUIManager* manager, SHVControlContainer* controlContainer, SHVString8C entityName) = 0;


	// Register controls
	virtual SHVBool RegisterFactory(int controlType, int controlSubType, SHVControlCreatorBase* factory) = 0;
	virtual SHVBool RegisterFactory(const SHVString8C& controlName, SHVControlCreatorBase* factory) = 0;


	// Control factory
	virtual SHVControl* NewControl(int controlType, int controlSubType) = 0;
	virtual SHVControl* NewControl(const SHVString8C& controlName) = 0;

	// Additional control events
	virtual SHVBool ControlEventSubscribe(SHVInt controlEvent, SHVControl* control, SHVEventSubscriberBase* subscriber) = 0;
	virtual void ClearControlEvents(SHVControl* control) = 0;

	// Wait cursor functions
	inline void WaitCursorShow();
	inline void WaitCursorHide();


	virtual SHVModuleList& GetModuleList() = 0;
	virtual SHVConfig& GetConfig() = 0;

	virtual SHVControlContainer* GetMainWindow() = 0;


protected:
friend class SHVControl;

	virtual void EmitControlEvent(SHVControl* control, SHVInt controlEvent) = 0;

	///\cond INTERNAL
	inline SHVGUIManager(SHVModuleList& modules) : SHVModule(modules,"GUIManager") {}
	///\endcond
};



// ============================================= implementation ============================================= //

/*************************************
 * NewContainer
 *************************************/
SHVControlContainer* SHVGUIManager::NewContainer(int subType)
{
	return (SHVControlContainer*)NewControl(SHVControl::TypeContainer,subType);
}

/*************************************
 * NewContainerCustomDraw
 *************************************/
SHVControlContainer* SHVGUIManager::NewContainerCustomDraw(SHVEventSubscriberBase* subscriber)
{
SHVControlContainer* retVal = (SHVControlContainer*)NewControl(SHVControl::TypeContainer,SHVControlContainer::SubTypeCustomDraw);

	if (retVal)
		((SHVControlImplementerContainerCustomDraw*)retVal->GetImplementor())->SubscribeDraw(subscriber);
	else
		subscriber->ValidateRefCount();

	return retVal;
}

/*************************************
 * NewDialog
 *************************************/
SHVControlContainer* SHVGUIManager::NewDialog()
{
	return (SHVControlContainer*)NewControl(SHVControl::TypeContainer,SHVControlContainer::SubTypeDialog);
}

/*************************************
 * NewDialog
 *************************************/
SHVControlContainer* SHVGUIManager::NewModalDialog()
{
	return (SHVControlContainer*)NewControl(SHVControl::TypeContainer,SHVControlContainer::SubTypeModalDialog);
}

/*************************************
 * NewEdit
 *************************************/
SHVControlEdit* SHVGUIManager::NewEdit(int subType)
{
	return (SHVControlEdit*)NewControl(SHVControl::TypeEdit,subType);
}

/*************************************
 * NewLabel
 *************************************/
SHVControlLabel* SHVGUIManager::NewLabel(int subType)
{
	return (SHVControlLabel*)NewControl(SHVControl::TypeLabel,subType);
}

/*************************************
 * NewLabelCustomDraw
 *************************************/
SHVControlLabel* SHVGUIManager::NewLabelCustomDraw(SHVEventSubscriberBase* subscriber)
{
SHVControlLabel* retVal = (SHVControlLabel*)NewControl(SHVControl::TypeLabel,SHVControlLabel::SubTypeCustomDraw);

	if (retVal)
		((SHVControlImplementerLabelCustomDraw*)retVal->GetImplementor())->SubscribeDraw(subscriber);
	else
		subscriber->ValidateRefCount();

	return retVal;
}

/*************************************
 * NewButton
 *************************************/
SHVControlButton* SHVGUIManager::NewButton(int subType)
{
	return (SHVControlButton*)NewControl(SHVControl::TypeButton,subType);
}

/*************************************
 * NewTab
 *************************************/
SHVControlTab* SHVGUIManager::NewTab(int subType)
{
	return (SHVControlTab*)NewControl(SHVControl::TypeTab,subType);
}

/*************************************
 * NewDateTime
 *************************************/
SHVControlDateTime* SHVGUIManager::NewDateTime(int subType)
{
	return (SHVControlDateTime*)NewControl(SHVControl::TypeDateTime,subType);
}

/*************************************
 * NewComboBox
 *************************************/
SHVControlComboBox* SHVGUIManager::NewComboBox(int subType)
{
	return (SHVControlComboBox*)NewControl(SHVControl::TypeComboBox,subType);
}

/*************************************
 * NewListView
 *************************************/
SHVControlListView* SHVGUIManager::NewListView(int subType)
{
	return (SHVControlListView*)NewControl(SHVControl::TypeListView,subType);
}

/*************************************
 * NewListViewCustomDraw
 *************************************/
SHVControlListView* SHVGUIManager::NewListViewCustomDraw(SHVEventSubscriberBase* subscriber)
{
SHVControlListView* retVal = (SHVControlListView*)NewControl(SHVControl::TypeListView,SHVControlListView::SubTypeCustomDraw);

	if (retVal)
		((SHVControlImplementerListViewCustomDraw*)retVal->GetImplementor())->SubscribeDraw(subscriber);
	else
		subscriber->ValidateRefCount();

	return retVal;
}

/*************************************
 * GetFont
 *************************************/
SHVFont* SHVGUIManager::GetFont(int cfgID)
{
	return (SHVFont*)GetConfig().FindRef(cfgID)->ToRef();
}

/*************************************
 * GetTransparentColor
 *************************************/
SHVColor* SHVGUIManager::GetTransparentColor()
{
	return (SHVColor*)GetConfig().FindRef(SHVGUIManager::CfgColorTransparent)->ToRef();
}

/*************************************
 * WaitCursorShow
 *************************************/
void SHVGUIManager::WaitCursorShow()
{
	Modules.EmitEvent(new SHVEventString(NULL,__EVENT_GLOBAL_WAITCURSORSHOW));
}

/*************************************
 * WaitCursorHide
 *************************************/
void SHVGUIManager::WaitCursorHide()
{
	Modules.EmitEvent(new SHVEventString(NULL,__EVENT_GLOBAL_WAITCURSORHIDE));
}

#endif
