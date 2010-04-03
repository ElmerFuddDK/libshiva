#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvformmain.h"


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
	
	GetContainer()->SetTitle(_T("SHVTemplateSHV"));
	GetContainer()->SetMinimumSize(230,180);
	
	// Create a label into the form, setting the text as well
	Label = GetManager()->NewLabel()->SetParent(GetContainer())->SetText(_T("Hello World!"));
	
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
 * OnResizeForm
 *************************************/
void SHVFormMain::OnResizeForm(SHVControlContainer* container, SHVControlLayout* layout)
{
SHVRegionRef rgn = GetManager()->CreateRegion(container);

	SHVUNUSED_PARAM(layout);

	rgn->Move(Label)->AlignHorizontal(NULL,NULL,SHVRegion::AlignHCenter)->AlignVertical(NULL,NULL,SHVRegion::AlignVCenter);
}
