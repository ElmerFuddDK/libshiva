#include "stdafx.h"
#include "../../../../../include/platformspc.h"

#include "shvtabtestform.h"
#include "shvtabtestpageform.h"

//=========================================================================================================
// SHVFormTabTest
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVFormTabTest::SHVFormTabTest(SHVGUIManager* manager, SHVControlContainer* controlContainer)
	: SHVForm<>(manager,controlContainer,"FormTabTest")
{
}

/*************************************
 * InitializeForm
 *************************************/
void SHVFormTabTest::InitializeForm()
{
SHVRegionRef rgn = GetManager()->CreateRegion(GetContainer());
SHVMenuRef toplevelmenu, menu;

	SetStandardLayoutEngine();

	GetContainer()->SetTitle(_T("Tab test dialog"));

	GetContainer()->SetMinimumSize(200,100);

	tbTabs = GetManager()->NewTab();
	tbTabs->SetParent(GetContainer());

	toplevelmenu = GetContainer()->CreateMenu(new SHVEventSubscriber(this,&GetManager()->GetModuleList()));

	menu = toplevelmenu->AddSubMenu(_T("Dialog"));
	menu->AddStringItem(1,_T("Close"));
	menu->AddStringItem(2,_T("Add tab"));
	menu->AddStringItem(3,_T("Remove tab"));
	menu->AddSeparator();
	menu->AddStringItem(4,_T("Change color"));
	toplevelmenu->Show();

	GetContainer()->ResizeControls();

	Show();
}

/*************************************
 * SetData
 *************************************/
SHVBool SHVFormTabTest::SetData(SHVControlData* data)
{
SHVBool retVal(SHVBool::False);

	return retVal;
}

/*************************************
 * GetData
 *************************************/
SHVBool SHVFormTabTest::GetData(SHVControlData* data)
{
SHVBool retVal(SHVBool::False);

	return retVal;
}

/*************************************
 * OnEvent
 *************************************/
void SHVFormTabTest::OnEvent(SHVEvent* event)
{
	if (event->GetCaller() == NULL && event->GetObject() == GetContainer())
	{
		if (SHVEvent::Equals(event,SHVControl::EventMenu))
		{
			switch (event->GetSubID())
			{
			case 1:
				GetContainer()->Close();
				break;
			case 2:
				{
				SHVString str;
				SHVControlContainer* cnt;
				SHVFormBase* form;

					str.Format(_T("Tab %d"), tbTabs->GetPageCount()+1);
					cnt = tbTabs->AddPage(str);
					tbTabs->SetPageForm(tbTabs->GetPageCount()-1, form = new SHVFormTabTestPage(GetManager(),cnt,str));

					form->InitializeForm();
				}
				break;
			case 3:
				tbTabs->RemovePage(0);
				break;
			case 4:
				GetContainer()->SetColor(GetManager()->CreateColor(::rand()&0xFF,::rand()&0xFF,::rand()&0xFF));
				break;
			}
		}
	}
}

/*************************************
 * OnResizeForm
 *************************************/
void SHVFormTabTest::OnResizeForm(SHVControlContainer* container, SHVControlLayout* layout)
{
SHVRegionRef rgn = GetManager()->CreateRegion(container);

	rgn->Move(tbTabs)->FillPercent(0,0,100,100,SHVRect(5,5,5,5));
}
