#include "stdafx.h"
#include "../../../../../include/platformspc.h"

#include "shvcontroltester.h"
#include "../../../../../include/gui/shvcontrollayout.h"
#include "../../../../../include/gui/utils/shvregion.h"
#include "../../../../../include/gui/utils/shvdraw.h"


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
SHVFontRef font;

	if (!SHVModuleResolver<SHVGUIManager>(Modules,GUIManager,"GUIManager"))
		return false;

	font = GUIManager->GetFont(SHVGUIManager::CfgFontNormal)->CreateCopy(100);

	GUIManager->GetConfig().SetRef(SHVGUIManager::CfgFontNormal,font);
	GUIManager->GetMainWindow()->SetFont(font,false);

	return SHVModule::Register();
}

/*************************************
 * PostRegister
 *************************************/
void SHVControlTester::PostRegister()
{
SHVEventSubscriberBaseRef subsOnDrawLabel = new SHVEventSubscriberFunc<SHVControlTester>(this,&SHVControlTester::OnDrawLabel);
SHVEventSubscriberBaseRef subsOnDrawContainer = new SHVEventSubscriberFunc<SHVControlTester>(this,&SHVControlTester::OnDrawContainer);
SHVFontRef ownerDrawFont = GUIManager->GetFont(SHVGUIManager::CfgFontNormal)->CreateCopy(250);

	
	GUIManager->GetMainWindow()->SetTitle(_T("SHIVA GUI test application"));
	GUIManager->GetMainWindow()->SetMinimumSize(200,115);
	GUIManager->GetMainWindow()->SetLayoutEngine(new SHVControlLayoutCallback<SHVControlTester>(this,&SHVControlTester::OnResizeContainer));


	Container = GUIManager->NewContainerCustomDraw(subsOnDrawContainer)
		->SetColor(GUIManager->CreateColor(0xFF,0xFF,0xFF))
		->SetParent(GUIManager->GetMainWindow());

	Container->SetLayoutEngine(new SHVControlLayoutCallback<SHVControlTester>(this,&SHVControlTester::OnResizeControls));

	Label = GUIManager->NewLabel()->SetParent(Container)->SetText(_T("Label text"));
	LabelCustomDraw = GUIManager->NewLabelCustomDraw(subsOnDrawLabel)->SetParent(Container)->SetText(_T(""));
	EditBox = GUIManager->NewEdit(SHVControlEdit::SubTypeMultiLine)->SetParent(Container)->SetText(_T("Edit\ntext"))->SetLimit(50);
	Button = GUIManager->NewButton()->SetParent(Container)->SetText(_T("Click Me!"));


	LabelCustomDraw->SetFont(ownerDrawFont,true);
	EditBox->SetHeight(4);


	Button->SubscribeClicked(new SHVEventSubscriber(this,&Modules));

	GUIManager->GetMainWindow()->SetFlag(SHVControl::FlagVisible);
	GUIManager->GetMainWindow()->ResizeControls();

	SHVModule::PostRegister();
}

/*************************************
 * Unregister
 *************************************/
void SHVControlTester::Unregister()
{
	Label = NULL;
	LabelCustomDraw = NULL;
	EditBox = NULL;
	Button = NULL;

	SHVModule::Unregister();
}

void SHVControlTester::OnEvent(SHVEvent* event)
{
	//::MessageBox(NULL,_T("Noget"),_T("Knap"),MB_OK);
	GUIManager->ShowMessageBox(_T("Noget\nEller\nNoget\nAndet\nEller\nNoget\nTredje\nNoget"),_T("Knap"));
	GUIManager->ShowMessageBox(_T("Noget\nEller\nNoget\nAndet\nEller\nNoget\nTredje\nNoget"));
	GUIManager->ShowMessageBox(_T("Noget\nEller\nNoget\nAndet\nEller\nNoget\nTredje\nNoget"),_T("Knap"));
	GUIManager->ShowMessageBox(_T("Noget\nEller\nNoget\nAndet\nEller\nNoget\nTredje\nNoget"));
	GUIManager->GetMainWindow()->SetColor(GUIManager->CreateColor(0x99,0x66,0x66));
}

void SHVControlTester::OnResizeContainer(SHVControlContainer* container, SHVControlLayout* layout)
{
SHVRegionRef rgn = GUIManager->CreateRegion(container);

	rgn->ClipTop(15)->Move(Container)->FillPercent(15,0,100,100,SHVRect());
}

void SHVControlTester::OnResizeControls(SHVControlContainer* container, SHVControlLayout* layout)
{
SHVRegionRef rgn = GUIManager->CreateRegion(container);

	rgn->Move(Label)->Top()->ClipTop();
	rgn->Move(EditBox)->FillHorizontal(Label,NULL)->LeftOf(Label)->ClipTop();

	rgn->Move(Button)->Bottom()->AlignHorizontal(NULL,NULL,SHVRegion::AlignHCenter)->ClipBottom(4);

	rgn->Move(LabelCustomDraw)->FillHorizontal()->Top()->ClipTop();
}

void SHVControlTester::OnDrawLabel(SHVEvent* event)
{
SHVDrawRef draw = SHVDraw::FromDrawEvent(event);
SHVControlRef control = (SHVControl*)event->GetObject();
SHVRect rct = draw->GetClientRect(control);

	draw->DrawText(_T("These are not the droids you are looking for"),
		rct,
		NULL,
		SHVDraw::TextSingleLine|SHVDraw::TextHCenter|SHVDraw::TextVCenter|SHVDraw::TextEndEllipsis);
}

void SHVControlTester::OnDrawContainer(SHVEvent* event)
{
SHVDrawRef draw = SHVDraw::FromDrawEvent(event);
SHVRect rct = draw->GetClientRect((SHVControl*)event->GetObject());

	//draw->DrawRectFilled(rct,GUIManager->CreateColor(0xFF,0xFF,0xFF));
}
