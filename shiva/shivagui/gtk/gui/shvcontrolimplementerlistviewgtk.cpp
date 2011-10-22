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
#include "../../../include/framework/shveventdata.h"
#include "../../../include/utils/shvstringutf8.h"

#include "shvcontrolimplementerlistviewgtk.h"
#include "utils/shvdrawgtk.h"


//=========================================================================================================
// SHVControlImplementerListViewGtk
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVControlImplementerListViewGtk::SHVControlImplementerListViewGtk(int subType) : SHVControlImplementerGtkWidget<SHVControlImplementerListViewCustomDraw>()
{
	SubType = subType;
}

/*************************************
 * Create
 *************************************/
SHVBool SHVControlImplementerListViewGtk::Create(SHVControl* owner, SHVControlImplementer* parent, int flags)
{
	if (!IsCreated() && parent && parent->IsCreated())
	{
		SetHandle(gtk_tree_view_new());
		gtk_container_add(GTK_CONTAINER (parent->GetNative()), GetHandle());

		// g_signal_connect (G_OBJECT (GetHandle()), "expose-event", G_CALLBACK (SHVControlImplementerListViewGtk::expose_event), owner);
		owner->SetFont(NULL,true);
		owner->SetFlag(flags);

		return IsCreated();
	}
	
	return SHVBool::False;
}

/*************************************
 * GetSubType
 *************************************/
int SHVControlImplementerListViewGtk::GetSubType(SHVControl* owner)
{
	SHVUNUSED_PARAM(owner);
	return SubType;
}

/*************************************
 * GetItemCount
 *************************************/
size_t SHVControlImplementerListViewGtk::GetItemCount(SHVControlListView* owner)
{
	SHVUNUSED_PARAM(owner);
	return ItemObjects.CalculateCount();
}

/*************************************
 * ClearItems
 *************************************/
void SHVControlImplementerListViewGtk::ClearItems(SHVControlListView* owner)
{
	SHVUNUSED_PARAM(owner);
	RecreateModel(true);
}

/*************************************
 * GetItemText
 *************************************/
SHVStringBuffer SHVControlImplementerListViewGtk::GetItemText(SHVControlListView* owner, size_t index, size_t col)
{
SHVString retVal;

SHVUNUSED_PARAM(owner);
	if (IsCreated())
	{
	GtkTreeModel* store = gtk_tree_view_get_model(GTK_TREE_VIEW (GetHandle()));
	GtkTreeIter iter;
	GValue* val = NULL;
		if (gtk_tree_model_get_iter_from_string(store, &iter, SHVStringC::LongToString((long)index).GetSafeBuffer()))
		{
			gtk_tree_model_get_value(store, &iter, col, val);
			if (val)
			{
				retVal = SHVStringBufferUTF8::Encapsulate(g_strdup_value_contents(val)).ToStrT();
				g_value_unset(val);
			}
		}
	}

	return retVal.ReleaseBuffer();
}

/*************************************
 * GetItemData
 *************************************/
SHVRefObject* SHVControlImplementerListViewGtk::GetItemData(SHVControlListView* owner, size_t index)
{
	return (index < GetItemCount(owner) ? ItemObjects[index] : NULL);
}

/*************************************
 * AddItem
 *************************************/
void SHVControlImplementerListViewGtk::AddItem(SHVControlListView* owner, const SHVStringC str, SHVRefObject* data)
{
	SHVUNUSED_PARAM(owner);
	if (IsCreated() && CachedCols.CalculateCount())
	{
	GtkTreeModel* store = gtk_tree_view_get_model(GTK_TREE_VIEW (GetHandle()));
	GtkTreeIter iter;
		gtk_list_store_append(GTK_LIST_STORE (store), &iter);
		gtk_list_store_set(GTK_LIST_STORE (store), &iter, 0, str.ToStrUTF8().GetSafeBuffer(), -1);
		ItemObjects.Add(data);
	}
	else if (data)
	{
		data->ValidateRefCount();
	}
}

/*************************************
 * SetItemText
 *************************************/
void SHVControlImplementerListViewGtk::SetItemText(SHVControlListView* owner, const SHVStringC text, size_t index, size_t col)
{
	if (IsCreated() && index < GetItemCount(owner) && col < CachedCols.CalculateCount())
	{
	GtkTreeModel* store = gtk_tree_view_get_model(GTK_TREE_VIEW (GetHandle()));
	GtkTreeIter iter;
		if (gtk_tree_model_get_iter_from_string(store, &iter, SHVStringC::LongToString((long)index).GetSafeBuffer()))
		{
			gtk_list_store_set(GTK_LIST_STORE (store), &iter, col, text.ToStrUTF8().GetSafeBuffer(), -1);
		}
	}
}

/*************************************
 * ClearColumns
 *************************************/
void SHVControlImplementerListViewGtk::ClearColumns(SHVControlListView* owner)
{
	SHVUNUSED_PARAM(owner);
	if (IsCreated())
		RecreateModel(true);
}

/*************************************
 * AddColumn
 *************************************/
void SHVControlImplementerListViewGtk::AddColumn(SHVControlListView* owner, const SHVStringC colName, SHVInt width)
{
	SHVUNUSED_PARAM(owner);
	if (IsCreated())
	{
	GtkCellRenderer* renderer;
	GtkTreeViewColumn* col;

		renderer = gtk_cell_renderer_text_new();
		col = gtk_tree_view_column_new_with_attributes(colName.ToStrUTF8().GetSafeBuffer(),renderer,"text", CachedCols.CalculateCount(), NULL);
		gtk_tree_view_column_set_resizable(col,TRUE);
		if (!width.IsNull())
		{
			gtk_tree_view_column_set_sizing(col,GTK_TREE_VIEW_COLUMN_FIXED);
			gtk_tree_view_column_set_fixed_width(col,width);
		}
		gtk_tree_view_insert_column(GTK_TREE_VIEW (GetHandle()), col, -1);
		CachedCols.Add(new CachedCol(colName,width));
		RecreateModel();
	}
}

/*************************************
 * GetSelected
 *************************************/
SHVInt SHVControlImplementerListViewGtk::GetSelected(SHVControlListView* owner)
{
	SHVUNUSED_PARAM(owner);
	///\todo Implement selection stuff
	return SHVInt();
}

/*************************************
 * SetSelected
 *************************************/
void SHVControlImplementerListViewGtk::SetSelected(SHVControlListView* owner, SHVInt index)
{
	if (IsCreated() && index < (int)GetItemCount(owner))
	{
		///\todo Implement selection stuff
	}
}

/*************************************
 * SubscribeDraw
 *************************************/
void SHVControlImplementerListViewGtk::SubscribeDraw(SHVEventSubscriberBase* subscriber)
{
	Subscriber = subscriber;
}

///\cond INTERNAL
/*************************************
 * expose_event
 *************************************/
/*gboolean SHVControlImplementerListViewGtk::expose_event(GtkWidget *widget, GdkEvent* event, gpointer user_data)
{
SHVControl* owner = (SHVControl*)user_data;
SHVControlImplementerListViewGtk* self = (SHVControlImplementerListViewGtk*)owner->GetImplementor();
GdkEventExpose* exposeEvent = (GdkEventExpose*)event;
SHVDrawGtkRef draw = new SHVDrawGtk(owner->GetManager(),gdk_gc_new(GDK_DRAWABLE (exposeEvent->window)),exposeEvent->window, owner);

	SHVUNUSED_PARAM(widget);
	
	if (!self->Subscriber.IsNull())
	{
	SHVControlRef refToSelf = owner; // ensure the validity of the object through this function
		self->Subscriber->EmitNow(owner->GetModuleList(),new SHVEventData<SHVDrawEventData>(SHVDrawEventData(draw),NULL,SHVControl::EventDraw,NULL,owner));
	}
	else
	{
		draw->DrawText(self->GetText(),draw->GetClientRect(owner),NULL,SHVDraw::TextVCenter|SHVDraw::TextLeft|SHVDraw::TextEndEllipsis);
	}
	
	return TRUE;
}*/

/*************************************
 * RecreateStore
 *************************************/
void SHVControlImplementerListViewGtk::RecreateModel(bool clearRows)
{
GtkListStore* store;
size_t i, colCount = CachedCols.CalculateCount();
GType* types;

	if (!colCount)
		colCount++;
	types = (GType*)::malloc(sizeof(GType)*colCount);
	for (i=0; i<colCount; i++) types[i] = G_TYPE_STRING;

	store = gtk_list_store_newv(colCount, types);

	if (!clearRows)
	{
		///\todo If clearRows is false, transfer data from old store
		ItemObjects.Clear();
	}
	else
	{
		ItemObjects.Clear();
	}

	::free(types);

	gtk_tree_view_set_model(GTK_TREE_VIEW (GetHandle()), GTK_TREE_MODEL (store));
	g_object_unref(store);
}
///\endcond
