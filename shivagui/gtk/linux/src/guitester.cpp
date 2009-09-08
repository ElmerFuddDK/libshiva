/*
 *   Copyright (C) 2008 by Lars Eriksen
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of the
 *   License, or (at your option) any later version with the following
 *   exeptions:
 *
 *   1) Static linking to the library does not constitute derivative work
 *      and does not require the author to provide source code for the
 *      application.
 *      Compiling applications with the source code directly linked in is
 *      Considered static linking as well.
 *
 *   2) You do not have to provide a copy of the license with programs
 *      that are linked against this code.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "../../../../include/platformspc.h"
#include "../../../../include/shvversion.h"

#include "../../../../include/frameworkimpl/shvmainthreadeventqueue.h"
#include "../../../../include/utils/shvdll.h"

#include "../../../../include/gui/shvguimanager.h"
#include "../../../../include/gui/shvregisterbitmap.h"

#include "heyyou.xpm"

class SHVTest : public SHVModule
{
public:
	
	SHVGUIManager* GUIManager;
	SHVControlLabelRef Label;
	SHVControlButtonRef Button;
	SHVControlEditRef Edit;
	SHVControlContainerRef NewWindow;
	int Counter;

	SHVTest(SHVModuleList& modules) : SHVModule(modules,"Test")
	{
		Counter = 0;
	}
	
	void OnResize(SHVControlContainer* container, SHVControlLayout* /*layout*/)
	{
		if (Label)
		{
		SHVRegionRef rgn = GUIManager->CreateRegion(container);
			if (++Counter > 10)
			{
				Label->SetFlag(SHVControl::FlagVisible);
				Label->SetText(SHVStringC::Format(_T("Label text : %d and counting"), Counter));
			}
			rgn->Move(Button)->Bottom(5)->AlignHorizontal(NULL,NULL,SHVRegion::AlignHCenter,5)->ClipBottom();
			rgn->Move(Label)
				->FillHorizontal(NULL,NULL,SHVRegion::AlignHCenter)
				->AlignVertical(NULL,NULL,SHVRegion::AlignBottom,10)
				->ClipBottom();
			rgn->Move(Edit)
				->FillPercent(0,0,100,100, SHVRect(2,2,2,2));
		}
	}

	void OnResizeDummy(SHVControlContainer* /*container*/, SHVControlLayout* /*layout*/)
	{
	}

	void OnClick(SHVEvent* event)
	{
		GUIManager->ShowMessageBox(_T("Noget\nEller\nNoget\nAndet\nEller\nNoget\nTredje\nNoget\nNoget\nNoget\nNoget\nNoget\nNoget\nNoget"),_T("Knap"));
		GUIManager->ShowMessageBox(_T("En heel masse text med maaaaange linjer hvis word wrap virker eller noget eller noget andet..."),_T("Knap"));
		NewWindow = GUIManager->NewDialog();
		NewWindow->Create();
		NewWindow->SetSize(220,100);
		NewWindow->SetLayoutEngine(new SHVControlLayoutCallback<SHVTest>(this,&SHVTest::OnResizeDummy));
		NewWindow->SetMinimumSize(120,100);
		NewWindow->SetTitle(_T("YaY!"));
		NewWindow->SetFlag(SHVControl::FlagVisible);
	}

	void OnCustomDraw(SHVEvent* event)
	{
	SHVDraw* draw = SHVDraw::FromDrawEvent(event);
	SHVRect client(draw->GetClientRect(Label));
		
		draw->DrawRect(client, GUIManager->CreateColor(0xFF,0x00,0xFF));
		draw->DrawXORRect(client);
		draw->DrawXORLine(SHVPoint(client.GetRight()-1, client.GetTop()+1),SHVPoint(client.GetLeft()+1, client.GetBottom()-1));
		//draw->DrawLine(SHVPoint(client.GetX()+1, client.GetY()+1),SHVPoint(client.GetRight()-1, client.GetBottom()-1),GUIManager->CreateColor(0x00,0xFF,0x00));
		draw->DrawLine(SHVPoint(client.GetX()+1, client.GetY()+1),SHVPoint(client.GetRight()-1, client.GetBottom()-1),GUIManager->CreatePen(GUIManager->CreateColor(0x00,0xFF,0x00),SHVPen::StyleSolid,3));
		//draw->DrawPolyline(GUIManager->CreateColor(0xFF,0,0), 2, SHVPoint::Val(client.GetX()+1, client.GetY()+1), SHVPoint::Val(client.GetRight()-1, client.GetTop()+1));
		draw->DrawPolygon(GUIManager->CreateColor(0xFF,0,0), 3, SHVPoint::Val(client.GetX()+1, client.GetY()+1), SHVPoint::Val(client.GetRight()-1, client.GetTop()+1), SHVPoint::Val(client.GetRight()-1, client.GetBottom()+1));
		client.Shrink(5,2,5,2);
		draw->DrawBitmapCentered(GUIManager->CreateBitmap(1),client,GUIManager->CreateColor(0xFF,0xFF,0xFF));
		draw->DrawBitmap(GUIManager->CreateBitmap(1),SHVPoint(client.GetX(),client.GetY()),13,10);
		draw->DrawText(GUIManager->GetFont(SHVGUIManager::CfgFontNormalBold),"TEEEEEST teeest\nDette er en test",client, GUIManager->CreateColor(0,0,0xFF), SHVDraw::TextHCenter|SHVDraw::TextVCenter|SHVDraw::TextMultiLine);
	}

	SHVBool Register()
	{
		printf("In register\n");
		
		if (!SHVModuleResolver<SHVGUIManager>(Modules,GUIManager,"GUIManager"))
			return false;
	
		SHVASSERT(Modules.ResolveModule("Test"));
		SHVASSERT(Modules.ResolveModule("Timer"));
	
		Modules.EventSubscribe(SHVModuleList::EventClosing, new SHVEventSubscriber(this));
		
		GUIManager->GetMainWindow()->SetFont(GUIManager->GetFont(SHVGUIManager::CfgFontLargeBold)->CreateCopy(250),false);
	
		return SHVModule::Register();
	}
	
	void PostRegister()
	{
		GUIManager->GetMainWindow()->SetTitle(_T("SHIVA GUI test application"));
		
		GUIManager->GetMainWindow()->SetLayoutEngine(new SHVControlLayoutCallback<SHVTest>(this,&SHVTest::OnResize));
		
		Edit = GUIManager->NewEdit(SHVControlEdit::SubTypeMultiLine)->SetParent(GUIManager->GetMainWindow(),SHVControl::FlagVisible);
		Edit->SetFont(GUIManager->GetMainWindow()->GetFont(),true);
		
		Button = GUIManager->NewButton()
			->SetParent(GUIManager->GetMainWindow())
			->SetText(_T("Click me"));
		Button->SubscribeClicked(new SHVEventSubscriberFunc<SHVTest>(this,&SHVTest::OnClick));
		
		//Label = GUIManager->NewLabel()->SetParent(GUIManager->GetMainWindow(),0);
		Label = GUIManager->NewLabelCustomDraw(new SHVEventSubscriberFunc<SHVTest>(this,&SHVTest::OnCustomDraw))->SetParent(GUIManager->GetMainWindow(),0);
		Label->SetFont(GUIManager->GetMainWindow()->GetFont(),true);
		
		SHVFontRef tstFont = GUIManager->CreateFont("Times",27,SHVFont::StyleItalic|SHVFont::StyleBold);
		if (!tstFont.IsNull())
		{
			printf("Cell height : %d\n", tstFont->GetHeight());
			Label->SetFont(tstFont,true);
		}
		Label->SetText(_T("Label text"));

		GUIManager->GetMainWindow()->SetMinimumSize(120,100);
		GUIManager->GetMainWindow()->SetSize(300,100);
		GUIManager->GetMainWindow()->SetColor(GUIManager->CreateColor(0xFF,0xFF,0xFF));
		GUIManager->GetMainWindow()->ResizeControls();
		GUIManager->GetMainWindow()->SetFlag(SHVControl::FlagVisible);
	}

	void OnEvent(SHVEvent* event)
	{
		SHVUNUSED_PARAM(event);
		printf("Delaying shutdown by 2500 ms\n");
		Modules.EmitEvent(new SHVEventString(this,__EVENT_GLOBAL_DELAYSHUTDOWN,2500));
		Label->SetParent(NULL);
	}

	void Unregister()
	{
		printf("In unregister\n");
		SHVModule::Unregister();
		Label = NULL;
		Button = NULL;
		NewWindow = NULL;
		Edit = NULL;
	}
};



int main(int argc, char *argv[])
{
SHVDll guilib;
int retVal = -1;

	SHVUNUSED_PARAM(argc);
	SHVUNUSED_PARAM(argv);

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR,__SHIVA_VERSION_MINOR,__SHIVA_VERSION_RELEASE))
	{
		::fprintf(stderr,_T("Invalid version of libshiva.dll\n"));
	}
	else if (!guilib.Load(guilib.CreateLibFileName(_T("shivaguigtk"))))
	{
		::fprintf(stderr,_T("Could not load GUI module library shivaguigtk\n"));
	}
	else
	{
	SHVMainThreadEventQueue mainqueue((SHVMainThreadEventDispatcher*)guilib.CreateObjectInt(NULL,SHVDll::ClassTypeMainThreadDispatcher));
	SHVRegisterBitmap::SHVRegisterBitmap(mainqueue.GetModuleList(),1,(char**)heyyou_xpm);

// 		mainqueue.GetModuleList().AddModule(new SHVControlTester(mainqueue.GetModuleList()));
 		mainqueue.GetModuleList().AddModule(new SHVTest(mainqueue.GetModuleList()));

		retVal = mainqueue.Run();
	}

	guilib.Unload();

	return retVal;
}
