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


