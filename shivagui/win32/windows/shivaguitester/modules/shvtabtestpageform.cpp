#include "stdafx.h"
#include "../../../../../include/platformspc.h"

#include "shvtabtestpageform.h"


//=========================================================================================================
// SHVFormTabTestPage
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVFormTabTestPage::SHVFormTabTestPage(SHVGUIManager* manager, SHVControlContainer* controlContainer, const SHVStringC labelStr)
	: SHVForm<>(manager,controlContainer,"FormTabTestPage")
{
	LabelStr = labelStr;
}

/*************************************
 * InitializeForm
 *************************************/
void SHVFormTabTestPage::InitializeForm()
{
SHVMenuRef toplevelmenu, menu;

	SetStandardLayoutEngine();

	Label = GetManager()->NewLabel();
	Label->SetParent(GetContainer())->SetText(LabelStr);

	EditBox = GetManager()->NewEdit();
	EditBox->SetParent(GetContainer());

	Button = GetManager()->NewButton();
	Button->SetParent(GetContainer())->SetText(_T("Click me"))->SubscribeClicked(new SHVEventSubscriber(this));

	GetContainer()->ResizeControls();

	Show();
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVFormTabTestPage::SetData(SHVControlData* data)
{
SHVBool retVal(SHVBool::False);

	return retVal;
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVFormTabTestPage::GetData(SHVControlData* data)
{
SHVBool retVal(SHVBool::False);

	return retVal;
}

/*************************************
 * OnEvent
 *************************************/
void SHVFormTabTestPage::OnEvent(SHVEvent* event)
{
	if (event->GetCaller() == NULL && event->GetObject() == Button)
	{
		if (SHVEvent::Equals(event,SHVControlButton::EventClicked))
		{
			Label->SetText(EditBox->GetText());
			GetContainer()->ResizeControls();
		}
	}
}

/*************************************
 * OnResizeForm
 *************************************/
void SHVFormTabTestPage::OnResizeForm(SHVControlContainer* container, SHVControlLayout* layout)
{
SHVRegionRef rgn = GetManager()->CreateRegion(container);

	rgn->SetMargin(5,5);

	rgn->Move(Label)->Top()->Left();
	rgn->Move(EditBox)->Top()->CtrlMaxWidth(200)->FillHorizontal(Label);

	rgn->Move(Button)->Bottom()->Right();
}
