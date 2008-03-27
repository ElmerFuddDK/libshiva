#include "stdafx.h"
#include "../../../../include/platformspc.h"
#include "../../../../include/utils/shvstringutf8.h"
#include "../../include/shvdatarowlistsqlite.h"
#include "../../include/shvdatarowcsqlite.h"
#include "../../include/shvdatarowimpl.h"


/*************************************
 * Constructor
 *************************************/
SHVDataRowList_SQLite::SHVDataRowList_SQLite(SHVDataSession* dataSession, SHVDataRowListC *rowList): NonAcceptedEnabled(false), EventsEnabled(false)
{
///\todo Missing thread safety
	RowList = rowList;
	DataSession = dataSession;
	PendingPosAdded = NULL;
	InsertedRows = 0;
}

SHVDataRowList_SQLite::~SHVDataRowList_SQLite()
{
	UnregisterDataList();
	DataSession = NULL;
}

/*************************************
 * GetCurrentRow
 *************************************/
const SHVDataRowC* SHVDataRowList_SQLite::GetCurrentRow() const
{
///\todo Missing thread safety
	if (PendingPosAdded)
		return PendingRows.GetAt(*((SHVListPos *)(&PendingPosAdded)));
	else
		return RowList->GetCurrentRow();
}

/*************************************
 * IsOk
 *************************************/
SHVBool SHVDataRowList_SQLite::IsOk() const
{
///\todo Missing thread safety
	return RowListValid() && RowList->IsOk();
}

/*************************************
 * GetStruct
 *************************************/
const SHVDataStructC* SHVDataRowList_SQLite::GetStruct() const
{
///\todo Missing thread safety
	return RowList->GetStruct();
}
/*************************************
 * GetAlias
 *************************************/
const SHVString8C SHVDataRowList_SQLite::GetAlias() const
{
	return RowList->GetAlias();
}

/*************************************
 * GetRowCount
 *************************************/
int SHVDataRowList_SQLite::GetRowCount() const
{
///\todo Missing thread safety
	return RowList->GetRowCount() + InsertedRows;
}

/*************************************
 * Find
 *************************************/
SHVDataRowC* SHVDataRowList_SQLite::Find(const SHVDataRowKey* key)
{
///\todo Missing thread safety
	return RowList->Find(key);
}

/*************************************
 * NextRow
 *************************************/
SHVBool SHVDataRowList_SQLite::NextRow()
{
SHVBool retVal = SHVBool::True;
///\todo Missing thread safety
	if (PendingPosAdded)
	{
		retVal = NextPendingAdded();
	}
	else
	{
		retVal = RowList->NextRow();
		if (!retVal)
		{
			retVal = NextPendingAdded();
		}
	}
	return retVal;
}

/*************************************
 * Reverse
 *************************************/
SHVDataRowListC* SHVDataRowList_SQLite::Reverse(const SHVStringC& condition)
{
	return RowList->Reverse(condition);
}

/*************************************
 * Reset
 *************************************/
SHVBool SHVDataRowList_SQLite::Reset()
{
///\todo Missing thread safety
	PendingPosAdded = NULL;
	return RowList->Reset();
}

/*************************************
 * GetRowProvider
 *************************************/
const void* SHVDataRowList_SQLite::GetRowProvider() const
{
	return RowList->GetRowProvider();
}

/*************************************
 * GetDataSession
 *************************************/
SHVDataSession* SHVDataRowList_SQLite::GetDataSession()
{
///\todo Missing thread safety
	return DataSession;
}

/*************************************
 * RowListValid
 *************************************/
SHVBool SHVDataRowList_SQLite::RowListValid() const
{
///\todo Missing thread safety
	return !DataSession.IsNull() && DataSession->SessionValid() && RowList->RowListValid();
}

/*************************************
 * EditCurrentRow
 *************************************/
SHVDataRow* SHVDataRowList_SQLite::EditCurrentRow()
{	
///\todo Missing thread safety
SHVDataRow* retVal = NULL;
	SHVASSERT(IsOk());
	SHVASSERT(GetCurrentRow());
	if (GetCurrentRow())
	{
		if (PendingPosAdded)
		{
			retVal = PendingRows.GetAt(PendingPosAdded);
		}
		else
		{
		SHVDataRowKeyRef key = GetCurrentRow()->GetKey(0);
			retVal = FindPending(key);
			if (!retVal)
			{
				retVal = new SHVDataRow_impl(GetCurrentRow(), this);
				if (NonAcceptedEnabled)
					PendingRows.AddHead(retVal);
			}
		}
	}
	return retVal;
}

/*************************************
 * AddRow
 *************************************/
SHVDataRow* SHVDataRowList_SQLite::AddRow()
{
///\todo Missing thread safety
SHVDataRow* retVal = NULL;
	SHVASSERT(IsOk());
	if (IsOk())
	{
		retVal = new SHVDataRow_impl(this);
		InsertedRows++;
		if (NonAcceptedEnabled)
			PendingRows.AddTail(retVal);
	}
	return retVal;
}

/*************************************
 * EnableEvents
 *************************************/
void SHVDataRowList_SQLite::EnableEvents(bool enable)
{
	EventsEnabled = enable;
}

/*************************************
 * GetEventsEnabled
 *************************************/
bool SHVDataRowList_SQLite::GetEventsEnabled() const
{
	return EventsEnabled;
}

/*************************************
 * EnableNonAccepted
 *************************************/
void SHVDataRowList_SQLite::EnableNonAccepted(bool enable)
{
	NonAcceptedEnabled = enable;
}

/*************************************
 * GetNonAcceptedEnabled
 *************************************/
bool SHVDataRowList_SQLite::GetNonAcceptedEnabled() const
{
	return NonAcceptedEnabled;
}

/*************************************
 * AcceptChanges
 *************************************/
SHVBool SHVDataRowList_SQLite::AcceptChanges(SHVDataRow* row)
{
///\todo Missing thread safety
SHVBool retVal; 
SHVDataRowRef rrow = row;
SHVListPos pos =  PendingRows.Find(rrow);

	SHVASSERT(IsOk());
	retVal = (pos != NULL || !NonAcceptedEnabled) &&
		(row->GetRowState() == SHVDataRow::RowStateAdding ||
		 row->GetRowState() == SHVDataRow::RowStateChanging ||
		 row->GetRowState() == SHVDataRow::RowStateDeleting);
	if (retVal)
	{
		retVal = SHVDataRowListC::InternalRowChanged(RowList, row);
		if (retVal)
		{
			RowChanged(rrow);
			retVal = UpdateRow(row);
			if (retVal)
			{
				if (pos)
				{
					if (pos == PendingPosAdded)
					{
						PendingPosAdded = NULL;
						Reposition();
					}
					PendingRows.RemoveAt(pos);
					if (row->GetRowState() == SHVDataRow::RowStateAdding)
						InsertedRows--;
				}
				InternalAcceptChanges(rrow);
				if (rrow->GetRowState() == SHVDataRow::RowStateAdded)
					AdjustRowCount(1);
				if (rrow->GetRowState() == SHVDataRow::RowStateDeleted)
					AdjustRowCount(-1);
				RowChanged(rrow);
				((SHVDataRow_impl*) row)->Owner = NULL;
			}
		}
	}
	return retVal;
}

/*************************************
 * RejectChanges
 *************************************/
SHVBool SHVDataRowList_SQLite::RejectChanges(SHVDataRow* row)
{
SHVDataRowRef rrow = row;
SHVListPos pos =  PendingRows.Find(row);
SHVBool retVal = SHVBool::True;

	SHVASSERT(IsOk());
	retVal = (pos != NULL || !NonAcceptedEnabled) &&
		(row->GetRowState() == SHVDataRow::RowStateAdding ||
		 row->GetRowState() == SHVDataRow::RowStateChanging ||
		 row->GetRowState() == SHVDataRow::RowStateDeleting);
	if (pos && retVal)
	{
		PendingRows.RemoveAt(pos);
		if (row->GetRowState() == SHVDataRow::RowStateAdding)
			InsertedRows--;
	}
	if (retVal)
	{
		InternalRejectChanges(row);
		((SHVDataRow_impl*) row)->Owner = NULL;
	}
	return retVal;
}

/*************************************
 * AdjustRowCount
 *************************************/
void SHVDataRowList_SQLite::AdjustRowCount(int delta)
{
	InternalAdjustRowCount(RowList, delta);
}

/*************************************
 * TempReset
 *************************************/
SHVBool SHVDataRowList_SQLite::TempReset()
{
	return SHVDataRowListC::TempReset(RowList);
}

/*************************************
 * Reposition
 *************************************/
void SHVDataRowList_SQLite::Reposition()
{
	SHVDataRowListC::Reposition(RowList);
}
/*************************************
 * InternalRowChanged
 *************************************/
SHVBool SHVDataRowList_SQLite::InternalRowChanged(SHVDataRow* row)
{
	return SHVBool::True;
}

/*************************************
 * FindPending
 *************************************/
SHVDataRow* SHVDataRowList_SQLite::FindPending(const SHVDataRowKey* key)
{
SHVListIterator<SHVDataRowRef, SHVDataRow*> iter(PendingRows);
SHVDataRow* found = NULL;
	while (iter.MoveNext() && !found)
	{
		if (iter.Get()->MatchKey(key))
			found = iter.Get();
	}
	return found;
}

/*************************************
 * NextPendingAdded
 *************************************/
bool SHVDataRowList_SQLite::NextPendingAdded()
{
bool retVal = PendingRows.MoveNext(PendingPosAdded);
	while(retVal && PendingRows.GetAt(PendingPosAdded)->GetRowState() != SHVDataRow::RowStateAdding)
	{
		retVal = PendingRows.MoveNext(PendingPosAdded);
	}
	return retVal;
}
