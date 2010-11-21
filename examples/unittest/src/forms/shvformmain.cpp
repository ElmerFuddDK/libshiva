#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvformmain.h"
#include "../logger/shvtestloggergui.h"


//=========================================================================================================
// SHVFormMain class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVFormMain::SHVFormMain(SHVGUIManager* manager, SHVControlContainer* controlContainer)
	: SHVForm<>(manager,controlContainer,"FormMain")
{
}

/*************************************
 * InitializeForm
 *************************************/
void SHVFormMain::InitializeForm()
{
	// Will make the form resizable, through the OnResizeForm function
	SetStandardLayoutEngine();
	
	GetContainer()->SetTitle(_S("UnitTest"));
	GetContainer()->SetMinimumSize(230,180);
	
	// Create a label into the form, setting the text as well
	Label = GetManager()->NewLabel()
		->SetParent(GetContainer())
		->SetText(_S("Log:"));
	LogWindow = GetManager()->NewEdit(SHVControlEdit::SubTypeMultiLine)
		->SetParent(GetContainer(),SHVControl::FlagVisible|SHVControlEdit::FlagReadonly);
	
	// Force a resize and show the form
	GetContainer()->ResizeControls();
	Show();
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVFormMain::SetData(SHVDataBinder* data)
{
	SHVUNUSED_PARAM(data);
	return SHVBool::False;
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVFormMain::GetData(SHVDataBinder* data)
{
	SHVUNUSED_PARAM(data);
	return SHVBool::False;
}

/*************************************
 * CreateLogger
 *************************************/
SHVTestLogger* SHVFormMain::CreateLogger()
{
	return new SHVTestLoggerGUI(LogWindow);
}

/*************************************
 * ClearLog
 *************************************/
void SHVFormMain::ClearLog()
{
	LogWindow->SetText(_S(""));
}

/*************************************
 * OnResizeForm
 *************************************/
void SHVFormMain::OnResizeForm(SHVControlContainer* container, SHVControlLayout* layout)
{
SHVRegionRef rgn = GetManager()->CreateRegion(container);

	SHVUNUSED_PARAM(layout);

	rgn->Move(Label)
		->Top()
		->Left()
		->ClipTop();
	rgn->Move(LogWindow)
		->FillPercent(0,0,100,100);
}
