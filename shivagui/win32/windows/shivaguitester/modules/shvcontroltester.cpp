#include "stdafx.h"
#include "../../../../../include/platformspc.h"

#include "shvcontroltester.h"
#include "../../../../include/shvcontrollayout.h"
#include "../../../../include/utils/shvregion.h"


//=========================================================================================================
// SHVControlTester - Module for testing controls
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlTester::SHVControlTester(SHVModuleList& modules) : SHVModule(modules,"ControlTester")
{
}

/*************************************
 * Register
 *************************************/
SHVBool SHVControlTester::Register()
{
	if (!SHVModuleResolver<SHVGUIManager>(Modules,GUIManager,"GUIManager"))
		return false;

	return SHVModule::Register();
}

/*************************************
 * PostRegister
 *************************************/
void SHVControlTester::PostRegister()
{
	GUIManager->GetMainWindow()->SetTitle(_T("noget"));
	GUIManager->GetMainWindow()->SetLayoutEngine(new SHVControlLayoutCallback<SHVControlTester>(this,&SHVControlTester::OnResizeMainWnd));

	///\todo add your controls here
	Label = GUIManager->NewLabel()->SetParent(GUIManager->GetMainWindow())->SetText(_T("Label text"));
	EditBox = GUIManager->NewEdit(SHVControlEdit::SubTypeMultiLine)->SetParent(GUIManager->GetMainWindow())->SetText(_T("Edit text"))->SetLimit(10);
	Button = GUIManager->NewButton()->SetParent(GUIManager->GetMainWindow())->SetText(_T("Click Me!"));

	Label->SetRect(SHVRect(0,0,100,20));
	EditBox->SetRect(SHVRect(100,0,200,100));
	Button->SetRect(SHVRect(100,0,200,20));

	Button->SubscribeClicked(new SHVEventSubscriber(this,&Modules));

	GUIManager->GetMainWindow()->ResizeControls();

	SHVModule::PostRegister();
}

/*************************************
 * Unregister
 *************************************/
void SHVControlTester::Unregister()
{
	Label = NULL;
	EditBox = NULL;
	Button = NULL;

	SHVModule::Unregister();
}

void SHVControlTester::OnEvent(SHVEvent* event)
{
	::MessageBox(NULL,_T("Noget"),_T("Knap"),MB_OK);
}

void SHVControlTester::OnResizeMainWnd(SHVControlContainer* container, SHVControlLayout* layout)
{
SHVRegion rgn(container);

	rgn.Move(Label).Top();
	rgn.Move(EditBox).FillLeftRight(Label,NULL);

	rgn.Move(Button).Bottom().AlignLeftRight();
}
