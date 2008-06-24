/*
 *   Copyright (C) 2008 by Mogens Bak Nielsen, Lars Eriksen
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
#include "../include/shvdatarowchangecacheimpl.h"


/*************************************
 * Constructors
 *************************************/
SHVDataRowChangeCacheImpl::SHVDataRowChangeCacheImpl()
{
}
SHVDataRowChangeCacheImpl::~SHVDataRowChangeCacheImpl()
{
	Clear();
}

/*************************************
 * AddItem
 *************************************/
void SHVDataRowChangeCacheImpl::AddItem(const SHVDataRowC* row)
{
bool found = false;
int rowState;
	switch (row->GetRowState())
	{
		case SHVDataRow::RowStateAdding:
			rowState = SHVDataRow::RowStateAdded;
			break;
		case SHVDataRow::RowStateChanging:
			rowState = SHVDataRow::RowStateChanged;
			break;
		case SHVDataRow::RowStateDeleting:
			rowState = SHVDataRow::RowStateDeleted;
			break;
		default:
			rowState = row->GetRowState();
	}
	for (size_t i = 0; i < ChangedItems.GetCount() && !found; i++)
	{
		found = row->MatchKey(ChangedItems[i]->Key);
	}
	if (!found)
		ChangedItems.Add(new CacheItem(row->GetKey(), rowState));
}

/*************************************
 * RemoveItem
 *************************************/
void SHVDataRowChangeCacheImpl::RemoveItem(const SHVDataRowC* row)
{
bool found = false;
size_t i;
	for (i = 0; i < ChangedItems.GetCount() && !found; i++)
	{
		found = row->MatchKey(ChangedItems[i]->Key);
	}
	if (found)
		ChangedItems.Remove(i);
}

/*************************************
 * Clear
 *************************************/
void SHVDataRowChangeCacheImpl::Clear()
{
	ChangedItems.ClearAndInit();
}

/*************************************
 * GetRowCount
 *************************************/
const size_t SHVDataRowChangeCacheImpl::GetRowCount() const
{
	return ChangedItems.GetCount();
}

/*************************************
 * SHVDataRowKey
 *************************************/
const SHVDataRowKey* SHVDataRowChangeCacheImpl::GetKey(size_t rowNo) const
{
	return ChangedItems[rowNo]->Key.AsConst();
}

/*************************************
 * GetRowState
 *************************************/
const int SHVDataRowChangeCacheImpl::GetRowState(size_t rowNo) const
{
	return ChangedItems[rowNo]->RowState;
}


