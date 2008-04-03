#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../../../include/utils/shvstringutf8.h"
#include "../include/shvdatarowlistsqlite.h"
#include "../include/shvdatarowcsqlite.h"
#include "../include/shvdatarowimpl.h"


/*************************************
 * Constructor
 *************************************/
SHVDataRowListSQLite::SHVDataRowListSQLite(SHVDataSession* dataSession, SHVDataRowListC *rowList): NonAcceptedEnabled(false), EventsEnabled(false)
{
	RowList = rowList;
	DataSession = dataSession;
	PendingPosAdded = NULL;
	InsertedRows = 0;
}

/*************************************
 * EditCurrentRow
 *************************************/
SHVDataRow* SHVDataRowListSQLite::EditCurrentRow()
{	
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
				retVal = new SHVDataRowImpl(GetCurrentRow(), this);
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
SHVDataRow* SHVDataRowListSQLite::AddRow()
{
SHVDataRow* retVal = NULL;
	SHVASSERT(IsOk());
	if (IsOk())
	{
		retVal = new SHVDataRowImpl(this);
		if (NonAcceptedEnabled)
		{
			InsertedRows++;
			PendingRows.AddTail(retVal);
		}
	}
	return retVal;
}

/*************************************
 * EnableEvents
 *************************************/
void SHVDataRowListSQLite::EnableEvents(bool enable)
{
	EventsEnabled = enable;
}

/*************************************
 * GetEventsEnabled
 *************************************/
bool SHVDataRowListSQLite::GetEventsEnabled() const
{
	return EventsEnabled;
}

/*************************************
 * EnableNonAccepted
 *************************************/
void SHVDataRowListSQLite::EnableNonAccepted(bool enable)
{
	NonAcceptedEnabled = enable;
}

/*************************************
 * GetNonAcceptedEnabled
 *************************************/
bool SHVDataRowListSQLite::GetNonAcceptedEnabled() const
{
	return NonAcceptedEnabled;
}

/*************************************
 * GetCurrentRow
 *************************************/
const SHVDataRowC* SHVDataRowListSQLite::GetCurrentRow() const
{
	if (PendingPosAdded)
		return PendingRows.GetAt(*((SHVListPos *)(&PendingPosAdded)));
	else
		return RowList->GetCurrentRow();
}

/*************************************
 * IsOk
 *************************************/
SHVBool SHVDataRowListSQLite::IsOk() const
{
	return RowListValid() && RowList->IsOk();
}

/*************************************
 * GetStruct
 *************************************/
const SHVDataStructC* SHVDataRowListSQLite::GetStruct() const
{
	return RowList->GetStruct();
}

/*************************************
 * GetAlias
 *************************************/
const SHVString8C SHVDataRowListSQLite::GetAlias() const
{
	return RowList->GetAlias();
}

/*************************************
 * GetRowCount
 *************************************/
int SHVDataRowListSQLite::GetRowCount() const
{
	return RowList->GetRowCount() + InsertedRows;
}

/*************************************
 * Find
 *************************************/
SHVDataRowC* SHVDataRowListSQLite::Find(const SHVDataRowKey* key)
{
	return RowList->Find(key);
}

/*************************************
 * NextRow
 *************************************/
SHVBool SHVDataRowListSQLite::NextRow()
{
SHVBool retVal = SHVBool::True;
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
SHVDataRowListC* SHVDataRowListSQLite::Reverse(const SHVStringC& condition)
{
	return RowList->Reverse(condition);
}

/*************************************
 * Reset
 *************************************/
SHVBool SHVDataRowListSQLite::Reset()
{
	PendingPosAdded = NULL;
	return RowList->Reset();
}

/*************************************
 * GetRowProvider
 *************************************/
const void* SHVDataRowListSQLite::GetRowProvider() const
{
	return RowList->GetRowProvider();
}

/*************************************
 * GetDataSession
 *************************************/
SHVDataSession* SHVDataRowListSQLite::GetDataSession()
{
	return DataSession;
}

/*************************************
 * RowListValid
 *************************************/
SHVBool SHVDataRowListSQLite::RowListValid() const
{
	return !DataSession.IsNull() && DataSession->SessionValid() && RowList->RowListValid();
}

/*************************************
 * Destructor
 *************************************/
SHVDataRowListSQLite::~SHVDataRowListSQLite()
{
	UnregisterDataList();
	DataSession = NULL;
}

/*************************************
 * FindPending
 *************************************/
SHVDataRow* SHVDataRowListSQLite::FindPending(const SHVDataRowKey* key)
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
bool SHVDataRowListSQLite::NextPendingAdded()
{
bool retVal = PendingRows.MoveNext(PendingPosAdded);
	while(retVal && PendingRows.GetAt(PendingPosAdded)->GetRowState() != SHVDataRow::RowStateAdding)
	{
		retVal = PendingRows.MoveNext(PendingPosAdded);
	}
	return retVal;
}

/*************************************
 * AcceptChanges
 *************************************/
SHVBool SHVDataRowListSQLite::AcceptChanges(SHVDataRow* row)
{
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
				((SHVDataRowImpl*) row)->Owner = NULL;
			}
		}
	}
	return retVal;
}

/*************************************
 * RejectChanges
 *************************************/
SHVBool SHVDataRowListSQLite::RejectChanges(SHVDataRow* row)
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
		((SHVDataRowImpl*) row)->Owner = NULL;
	}
	return retVal;
}

/*************************************
 * AdjustRowCount
 *************************************/
void SHVDataRowListSQLite::AdjustRowCount(int delta)
{
	InternalAdjustRowCount(RowList, delta);
}

/*************************************
 * TempReset
 *************************************/
SHVBool SHVDataRowListSQLite::TempReset()
{
	return SHVDataRowListC::TempReset(RowList);
}

/*************************************
 * Reposition
 *************************************/
void SHVDataRowListSQLite::Reposition()
{
	SHVDataRowListC::Reposition(RowList);
}
/*************************************
 * InternalRowChanged
 *************************************/
SHVBool SHVDataRowListSQLite::InternalRowChanged(SHVDataRow* row)
{
	return SHVBool::True;
}
