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

	lblLongDate = GetManager()->NewLabel();
	lblLongDate->SetParent(GetContainer())->SetText(_T("Lang dato:"));

	dtLongDate = GetManager()->NewDateTime(SHVControlDateTime::SubTypeLongDate);
	dtLongDate->SetParent(GetContainer());

	lblShortDate = GetManager()->NewLabel();
	lblShortDate->SetParent(GetContainer())->SetText(_T("Kort dato:"));

	dtShortDate = GetManager()->NewDateTime(SHVControlDateTime::SubTypeShortDate);
	dtShortDate->SetParent(GetContainer());

	lblTime = GetManager()->NewLabel();
	lblTime->SetParent(GetContainer())->SetText(_T("Tid:"));

	dtTime = GetManager()->NewDateTime(SHVControlDateTime::SubTypeTime);
	dtTime->SetParent(GetContainer());

	GetContainer()->ResizeControls();

	Show();
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVFormTabTestPage::SetData(SHVDataBinder* data)
{
SHVBool retVal(SHVBool::False);

	return retVal;
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVFormTabTestPage::GetData(SHVDataBinder* data)
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
			// Set label to edit box contents
			Label->SetText(EditBox->GetText());

			// reset other labels width
			lblLongDate->SetText(lblLongDate->GetText());
			lblShortDate->SetText(lblShortDate->GetText());
			lblTime->SetText(lblTime->GetText());

			if (GetContainer()->GetTitle().Right(1) != _T("*"))
				GetContainer()->SetTitle(GetContainer()->GetTitle()+SHVStringC(_T("*")));
			GetContainer()->ResizeControls();

			dtLongDate->SetTime(dtShortDate->GetTime());
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

	rgn->Move(Label)->And(lblLongDate)->And(lblShortDate)->And(lblTime)->UnifyWidth();

	rgn->Move(Label)->Top()->Left();
	rgn->Move(EditBox)->Top()->CtrlMaxWidth(200)->FillHorizontal(Label)->ClipTop();
	rgn->Move(lblLongDate)->Top()->Left();
	rgn->Move(dtLongDate)->Top()->LeftOf(lblLongDate)->ClipTop();
	rgn->Move(lblShortDate)->Top()->Left();
	rgn->Move(dtShortDate)->Top()->LeftOf(lblShortDate)->ClipTop();
	rgn->Move(lblTime)->Top()->Left();
	rgn->Move(dtTime)->Top()->LeftOf(lblTime)->ClipTop();

	rgn->Move(Button)->Bottom()->Right();
}
