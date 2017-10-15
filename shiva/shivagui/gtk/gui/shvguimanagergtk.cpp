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

#include "../../../include/platformspc.h"

#include "../../../include/framework/shveventdata.h"

#include "shvguimanagergtk.h"
#include "shvgtk.h"
#include "utils/shvpengtk.h"
#include "utils/shvcolorgtk.h"
#include "utils/shvbitmapgtk.h"

#include "shvcontrolimplementercontainerwindowgtk.h"
#include "shvcontrolimplementerdialoggtk.h"
#include "shvcontrolimplementerlabelgtk.h"
#include "shvcontrolimplementereditgtk.h"
#include "shvcontrolimplementereditmultigtk.h"
#include "shvcontrolimplementerbuttongtk.h"
#include "shvcontrolimplementercomboboxgtk.h"
#include "shvcontrolimplementerlistviewgtk.h"
#include "shvcontrolimplementercheckboxgtk.h"
#include "shvcontrolimplementertabgtk.h"
#include "shvcontrolimplementerprogressbargtk.h"
#include "shvmenugtkpopup.h"
#include "../../../include/gui/shvcontrolcreator.h"
#include "../../../include/gui/shvcontrolcreatorsubtype.h"
#include "../../utilsimpl/shvregionimpl.h"

#include "../../gui/shvformimpl.h"


//=========================================================================================================
// SHVGUIManagerGtk - Module for registering control types, and instantiating them
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVGUIManagerGtk::SHVGUIManagerGtk(SHVModuleList& modules) : SHVGUIManagerImpl(modules)
{

	// Register controls
	RegisterFactory(SHVControl::TypeContainer,SHVControlContainer::SubTypeDefault,
		new SHVControlCreatorSubType<SHVControlContainer,SHVControlImplementerContainerWindowGtk,SHVControlContainer::SubTypeDefault>());
	RegisterFactory(SHVControl::TypeContainer,SHVControlContainer::SubTypeCustomDraw,
		new SHVControlCreatorSubType<SHVControlContainer,SHVControlImplementerContainerWindowGtk,SHVControlContainer::SubTypeCustomDraw>());

	RegisterFactory(SHVControl::TypeContainer,SHVControlContainer::SubTypeModalDialog,
		new SHVControlCreatorSubType<SHVControlContainer,SHVControlImplementerDialogGtk,SHVControlContainer::SubTypeModalDialog>());
	RegisterFactory(SHVControl::TypeContainer,SHVControlContainer::SubTypeDialog,
		new SHVControlCreatorSubType<SHVControlContainer,SHVControlImplementerDialogGtk,SHVControlContainer::SubTypeDialog>());

	RegisterFactory(SHVControl::TypeLabel,SHVControlLabel::SubTypeDefault,
		new SHVControlCreatorSubType<SHVControlLabel,SHVControlImplementerLabelGtk,SHVControlLabel::SubTypeDefault>());
	RegisterFactory(SHVControl::TypeLabel,SHVControlLabel::SubTypeCustomDraw,
		new SHVControlCreatorSubType<SHVControlLabel,SHVControlImplementerLabelGtk,SHVControlLabel::SubTypeCustomDraw>());

	RegisterFactory(SHVControl::TypeEdit,SHVControlEdit::SubTypeSingleLine,
		new SHVControlCreator<SHVControlEdit,SHVControlImplementerEditGtk>());
	RegisterFactory(SHVControl::TypeEdit,SHVControlEdit::SubTypeMultiLine,
		new SHVControlCreator<SHVControlEdit,SHVControlImplementerEditMultiGtk>());

	RegisterFactory(SHVControl::TypeButton,SHVControlButton::SubTypeDefault,
		new SHVControlCreator<SHVControlButton,SHVControlImplementerButtonGtk>());

	RegisterFactory(SHVControl::TypeComboBox,SHVControlComboBox::SubTypeEdit,
		new SHVControlCreatorSubType<SHVControlComboBox,SHVControlImplementerComboBoxGtk,SHVControlComboBox::SubTypeEdit>());
	RegisterFactory(SHVControl::TypeComboBox,SHVControlComboBox::SubTypeList,
		new SHVControlCreatorSubType<SHVControlComboBox,SHVControlImplementerComboBoxGtk,SHVControlComboBox::SubTypeList>());

	RegisterFactory(SHVControl::TypeListView,SHVControlListView::SubTypeNormal,
		new SHVControlCreatorSubType<SHVControlListView,SHVControlImplementerListViewGtk,SHVControlListView::SubTypeNormal>());
	RegisterFactory(SHVControl::TypeListView,SHVControlListView::SubTypeCustomDraw,
		new SHVControlCreatorSubType<SHVControlListView,SHVControlImplementerListViewGtk,SHVControlListView::SubTypeCustomDraw>());

	RegisterFactory(SHVControl::TypeCheckbox,SHVControlCheckbox::SubTypeNormal,
		new SHVControlCreatorSubType<SHVControlCheckbox,SHVControlImplementerCheckboxGtk,SHVControlCheckbox::SubTypeNormal>());
	RegisterFactory(SHVControl::TypeCheckbox,SHVControlCheckbox::SubTypeTristate,
		new SHVControlCreatorSubType<SHVControlCheckbox,SHVControlImplementerCheckboxGtk,SHVControlCheckbox::SubTypeTristate>());

	RegisterFactory(SHVControl::TypeTab,SHVControl::SubTypeDefault,
		new SHVControlCreator<SHVControlTab,SHVControlImplementerTabGtk>());

	RegisterFactory(SHVControl::TypeProgressBar,SHVControl::SubTypeDefault,
		new SHVControlCreator<SHVControlProgressBar,SHVControlImplementerProgressBarGtk>());



	// Set standard transparency color
	GetConfig().SetRef(CfgColorTransparent,CreateColor(0xFF,0x00,0xFF));
}

/*************************************
 * Register
 *************************************/
SHVBool SHVGUIManagerGtk::Register()
{
	return SHVGUIManagerImpl::Register();
}

/*************************************
 * Unregister
 *************************************/
void SHVGUIManagerGtk::Unregister()
{
	SHVGUIManagerImpl::Unregister();
	
	ActivePopupMenus.Clear();
}

/*************************************
 * CreateFont
 *************************************/
SHVFont* SHVGUIManagerGtk::CreateFont(const SHVStringC name, int height, int styles)
{
	return SHVFontGtk::CreateFromName(Gtk::GetHandle(GetMainWindow()),name,height,styles);
}

/*************************************
 * CreateColor
 *************************************/
SHVColor* SHVGUIManagerGtk::CreateColor(SHVColor::ColorVal r, SHVColor::ColorVal g, SHVColor::ColorVal b)
{
	return new SHVColorGtk(r,g,b);
}

/*************************************
 * CreatePen
 *************************************/
SHVPen* SHVGUIManagerGtk::CreatePen(SHVColor* color, int style, int width)
{
	return new SHVPenGtk((SHVColorGtk*)color,style,width);
}

/*************************************
 * CreateBrush
 *************************************/
SHVBrush* SHVGUIManagerGtk::CreateBrush(SHVColor* color, int style)
{
	return NULL; //new SHVBrushGtk((SHVColorGtk*)color,style);
}

/*************************************
 * CreateRegion
 *************************************/
SHVRegion* SHVGUIManagerGtk::CreateRegion(SHVControlContainer* container)
{
	return new SHVRegionImpl(container);
}

/*************************************
 * CreateBitmap
 *************************************/
SHVBitmap* SHVGUIManagerGtk::CreateBitmap(SHVInt bitmapID)
{
SHVBitmap* retVal = NULL;
	if (Modules.GetConfig(SHVModuleList::CfgBitmapResourceMap).Contains(bitmapID))
	{
	char ** xpmResource((char **)Modules.GetConfig(SHVModuleList::CfgBitmapResourceMap).FindPtr(bitmapID)->ToPtr());
		retVal = new SHVBitmapGtk(this,xpmResource);
	}

	return retVal;
}

/*************************************
 * CreatePopupMenu
 *************************************/
SHVMenu* SHVGUIManagerGtk::CreatePopupMenu(SHVEventSubscriberBase* subscriber, SHVControl* parent)
{
	return new SHVMenuGtkPopup(this,subscriber,parent);
}

/*************************************
 * ContructFormImplementer
 *************************************/
SHVFormImplementer* SHVGUIManagerGtk::ConstructFormImplementer(SHVFormBase* owner, SHVGUIManager* manager, SHVControlContainer* controlContainer, SHVString8C entityName)
{
	return new SHVFormImpl(owner,manager,controlContainer,entityName);
}

/*************************************
 * ControlEventSubscribe
 *************************************/
SHVBool SHVGUIManagerGtk::ControlEventSubscribe(SHVInt controlEvent, SHVControl* control, SHVEventSubscriberBase* subscriber)
{
	return SHVBool::False;
}

/*************************************
 * ClearControlEvents
 *************************************/
void SHVGUIManagerGtk::ClearControlEvents(SHVControl* control)
{
}

/*************************************
 * EmitControlEvent
 *************************************/
void SHVGUIManagerGtk::EmitControlEvent(SHVControl* control, SHVInt controlEvent)
{
}

///\cond INTERNAL
/*************************************
 * SetStandardFonts
 *************************************/
void SHVGUIManagerGtk::SetStandardFonts(SHVFont* font)
{
	GetConfig().SetRef(CfgFontNormal,font->CreateCopy(100));
	GetConfig().SetRef(CfgFontNormalBold,font->CreateCopy(100,SHVFont::StyleBold));
	GetConfig().SetRef(CfgFontLarge,font->CreateCopy(120));
	GetConfig().SetRef(CfgFontLargeBold,font->CreateCopy(120,SHVFont::StyleBold));
}
///\endcond
