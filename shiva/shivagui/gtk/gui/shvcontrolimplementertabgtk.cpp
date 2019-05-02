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
#include "stdafx.h"
#include "../../../include/platformspc.h"

#include "shvcontrolimplementertabgtk.h"
#include "shvcontrolimplementercontainerwindowgtk.h"
#include "utils/shvdrawgtk.h"

///\cond INTERNAL
class SHVControlImplementerContainerWindowTabPageGtk : public SHVControlImplementerContainerWindowGtk
{
public:
	SHVControlImplementerContainerWindowTabPageGtk() : SHVControlImplementerContainerWindowGtk(SHVControlContainer::SubTypeTabPage) {}
	
	virtual SHVRect GetRect(SHVControl* owner)
	{
		SHVUNUSED_PARAM(owner);
		return GetSizedRect();
	}
	
	virtual void SetRect(SHVControl* owner, const SHVRect& rect)
	{
		SHVUNUSED_PARAM(owner);
		SHVUNUSED_PARAM(rect);
	}

	virtual SHVRect GetRegionRect()
	{
		return SHVRect(0,0,GetSizedRect().GetWidth(),GetSizedRect().GetHeight());
	}
};
///\endcond


//=========================================================================================================
// SHVControlImplementerTabGtk
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerTabGtk::SHVControlImplementerTabGtk() : SHVControlImplementerGtkWidget<SHVControlImplementerTab>()
{
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerTabGtk::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && parent && parent->IsCreated())
	{
		SetHandle(gtk_notebook_new());
		gtk_container_add(GTK_CONTAINER (parent->GetNative()), GetHandle());
		
		g_signal_connect (G_OBJECT (GetHandle()), "change-current-page",
					  G_CALLBACK (SHVControlImplementerTabGtk::on_current_page_changed), owner);
		g_signal_connect (G_OBJECT (GetHandle()), "unrealize",
					  G_CALLBACK (SHVControlImplementerTabGtk::on_unrealize), owner);
		owner->SetFont(NULL,true);
		owner->SetFlag(flags);
		
		return IsCreated();
	}
	
	return SHVBool::False;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerTabGtk::GetSubType(SHVControl* owner)
{
	SHVUNUSED_PARAM(owner);
	return SHVControl::SubTypeDefault;
}

/*************************************
 * AddPage
 *************************************/
SHVControlContainer* SHVControlImplementerTabGtk::AddPage(SHVControlTab* owner, const SHVStringC name, size_t atIndex)
{
PageItem* item = new PageItem(name);
SHVControlImplementerContainerWindowTabPageGtk* page;
size_t count = Pages.CalculateCount();

	item->Container = new SHVControlContainer(owner->GetManager(),page = new SHVControlImplementerContainerWindowTabPageGtk());
	Pages.Add(item);
	page->Create(item->Container, NULL, 0);
	
	gtk_notebook_insert_page(GTK_NOTEBOOK(GetHandle()), page->GetHandle(), gtk_label_new(name.AsStrUTF8C().GetSafeBuffer()), atIndex < count ? (int)atIndex : -1);
	SelectPage(owner,count);
	
	return item->Container;
}

/*************************************
 * GetPage
 *************************************/
SHVControlContainer* SHVControlImplementerTabGtk::GetPage(SHVControlTab* owner, size_t index)
{
	return (index < GetPageCount(owner) ? Pages[index]->Container : NULL);
}

/*************************************
 * RemovePage
 *************************************/
void SHVControlImplementerTabGtk::RemovePage(SHVControlTab* owner, size_t index)
{
size_t count = GetPageCount(owner);

	if 	(index < count)
	{
	PageItem* tab = Pages.Replace(index,NULL).ReleaseReference();
	size_t i = index;

		for (i++;i<count;i++)
		{
			Pages.Replace(i-1, Pages.Replace(i,NULL).ReleaseReference());
		}
		Pages.Truncate();

		gtk_notebook_remove_page(GTK_NOTEBOOK(GetHandle()),(int)index);
		delete tab;

		// owner->PerformSelectedPageChanged();
	}
}

/*************************************
 * RemovePageByContainer
 *************************************/
void SHVControlImplementerTabGtk::RemovePageByContainer(SHVControlTab* owner, SHVControlContainer* container)
{
size_t count = Pages.CalculateCount();

	for (size_t i=0; container && i<count; i++)
	{
		if (Pages[i]->Container == container)
		{
			RemovePage(owner,i);
			container = NULL;
		}
	}
}

/*************************************
 * Clear
 *************************************/
void SHVControlImplementerTabGtk::Clear(SHVControlTab* owner)
{
	while (GetPageCount(owner))
	{
		RemovePage(owner,GetPageCount(owner)-1);
	}
}

/*************************************
 * GetSelectedPage
 *************************************/
size_t SHVControlImplementerTabGtk::GetSelectedPage(SHVControlTab* owner)
{
gint retVal = gtk_notebook_get_current_page(GTK_NOTEBOOK(GetHandle()));
	SHVUNUSED_PARAM(owner);
	return (retVal >= 0 ? (size_t)retVal : SIZE_T_MAX);
}

/*************************************
 * SelectPage
 *************************************/
void SHVControlImplementerTabGtk::SelectPage(SHVControlTab* owner, size_t index)
{
	if (index < GetPageCount(owner))
	{
		gtk_notebook_set_current_page(GTK_NOTEBOOK(GetHandle()), (int)index);
	}
}

/*************************************
 * SetPageForm
 *************************************/
void SHVControlImplementerTabGtk::SetPageForm(SHVControlTab* owner, size_t index, SHVFormBase* form)
{
	if (index < GetPageCount(owner))
	{
		Pages[index]->Form = form;
	}
}

/*************************************
 * GetPageForm
 *************************************/
SHVFormBase* SHVControlImplementerTabGtk::GetPageForm(SHVControlTab* owner, size_t index)
{
	return (index < GetPageCount(owner) ? Pages[index]->Form : NULL);
}

/*************************************
 * GetPageName
 *************************************/
SHVStringBuffer SHVControlImplementerTabGtk::GetPageName(SHVControlTab* owner, size_t index)
{
	return SHVString(index < GetPageCount(owner) ? Pages[index]->Name.GetBufferConst() : NULL).ReleaseBuffer();
}

/*************************************
 * GetPageCount
 *************************************/
size_t SHVControlImplementerTabGtk::GetPageCount(SHVControlTab* owner)
{
	SHVUNUSED_PARAM(owner);
	return Pages.CalculateCount();
}

///\cond INTERNAL
/*************************************
 * on_current_page_changed
 *************************************/
gboolean SHVControlImplementerTabGtk::on_current_page_changed(GtkNotebook* notebook, gint arg1, gpointer user_data)
{
SHVControlTab* owner = (SHVControlTab*)user_data;
//SHVControlImplementerTabGtk* self = (SHVControlImplementerTabGtk*)owner->GetImplementor();

	SHVUNUSED_PARAM(notebook);
	SHVUNUSED_PARAM(arg1);

	owner->PerformSelectedPageChanged();
	return FALSE;
}

/*************************************
 * on_unrealize
 *************************************/
gboolean SHVControlImplementerTabGtk::on_unrealize(GtkWidget *widget, gpointer data)
{
SHVControlTab* owner = (SHVControlTab*)data;
SHVControlImplementerTabGtk* self = (SHVControlImplementerTabGtk*)owner->GetImplementor();
gboolean retVal = FALSE;

	SHVUNUSED_PARAM(widget);

	self->Pages.Clear();
	
	return retVal;
}
///\endcond
