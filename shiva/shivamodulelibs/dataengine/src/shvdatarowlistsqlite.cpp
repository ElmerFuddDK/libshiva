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
#include "../include/shvdatarowlistsqlite.h"
#include "../include/shvdatarowcsqlite.h"
#include "../include/shvdatarowimpl.h"


/*************************************
 * Constructor
 *************************************/
SHVDataRowListSQLite::SHVDataRowListSQLite(SHVDataSession* dataSession, SHVDataRowListC *rowList): EventsEnabled(false), NonAcceptedEnabled(false), ReplaceIfDuplicate(false)
{
	RowList = rowList;
	DataSession = dataSession;
	PendingPosAdded = NULL;
	InsertedRows = 0;
}

/*************************************
 * StartEdit
 *************************************/
SHVBool SHVDataRowListSQLite::StartEdit()
{
SHVBool retVal;
	if (!ChangeCache.IsNull())
		ChangeCache->Clear();
	SavePosition = RowList->GetPosition();
	Reset();
	retVal = GetDataSession()->StartEdit();
	if (!SavePosition.IsNull())
		RowList->SetPosition(SavePosition);
	return retVal;
}

/*************************************
 * EndEdit
 *************************************/
SHVBool SHVDataRowListSQLite::EndEdit()
{
SHVBool retVal;
	SavePosition = RowList->GetPosition();
	Reset();
	retVal = GetDataSession()->Commit();
	if (!SavePosition.IsNull())
		RowList->SetPosition(SavePosition);
	return retVal;
}

/*************************************
 * CancelEdit
 *************************************/
SHVBool SHVDataRowListSQLite::CancelEdit()
{
	return GetDataSession()->Rollback();
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
 * DeleteRow
 *************************************/
SHVBool SHVDataRowListSQLite::DeleteRow(const SHVDataRowKey *key)
{
SHVDataFunctionRef func = DataSession->GetDataChangeFunction(this, SHVDataRowList::ChangeFunctionDelete);
SHVBool retVal = SHVBool::False;
	if (func)
	{
	const SHVDataStructC& st = *GetStruct();
	const SHVDataRowKey& Key = *key;
		for (size_t c = 0; c < Key.Count(); c++)
		{
		size_t colIdx;
			if (st.FindColumnIndex(colIdx, Key[c].Key))
			{
				if (!Key[c].Value)
				{
					func->SetParameterNullUTF8(st[colIdx]->GetColumnName().GetBufferConst());
				}
				else
				{
					switch (st[colIdx]->GetDataType())
					{
					case SHVDataVariant::TypeInt:
						func->SetParameterIntUTF8(st[colIdx]->GetColumnName().GetBufferConst(), Key[c].Value->AsInt());
						break;
					case SHVDataVariant::TypeBool:
						func->SetParameterIntUTF8(st[colIdx]->GetColumnName().GetBufferConst(), Key[c].Value->AsBool() ? 1 : 0);
						break;
					case SHVDataVariant::TypeInt64:
						func->SetParameterInt64UTF8(st[colIdx]->GetColumnName().GetBufferConst(), Key[c].Value->AsInt64());
						break;
					case SHVDataVariant::TypeDouble:
						func->SetParameterDoubleUTF8(st[colIdx]->GetColumnName().GetBufferConst(), Key[c].Value->AsDouble());
						 break;
					default:
						func->SetParameterStringUTF8(st[colIdx]->GetColumnName().GetBufferConst(), Key[c].Value->AsString().ToStrUTF8());
						break;
					}
				}
			}
		}
	}
	if (!func.IsNull())
	{
	SHVDataRowListCRef result = func->Exec();
		retVal = func->GetLastError();
		if (retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
		{
			retVal = SHVBool::True;
			AdjustRowCount(-1);
		}
		if (!result.IsNull())
			result->Reset();
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
 * EnableCacheChanges
 *************************************/
void SHVDataRowListSQLite::EnableCacheChanges(bool enable)
{
	if (enable)
	{
		if (ChangeCache.IsNull())
			ChangeCache = new SHVDataRowChangeCacheImpl();
	}
	else
		ChangeCache = NULL;
}

/*************************************
 * GetCacheChangesEnabled
 *************************************/
bool SHVDataRowListSQLite::GetCacheChangesEnabled() const
{
	return ChangeCache.IsNull();
}

/*************************************
 * GetCacheChangesEnabled
 *************************************/
void SHVDataRowListSQLite::EnableFullRowCache(bool enable)
{
	FullRowCache = enable;
}

/*************************************
 * GetCacheChangesEnabled
 *************************************/
bool SHVDataRowListSQLite::GetFullRowCache()
{
	return FullRowCache;
}

/*************************************
 * EnableReplaceIfDuplicate
 *************************************/
void SHVDataRowListSQLite::EnableReplaceIfDuplicate(bool enable)
{
	ReplaceIfDuplicate = enable;
}

/*************************************
 * GetReplaceIfDuplicate
 *************************************/
bool SHVDataRowListSQLite::GetReplaceIfDuplicate()
{
	return ReplaceIfDuplicate;
}

/*************************************
 * GetChangeCache
 *************************************/
const SHVDataRowChangeCache* SHVDataRowListSQLite::GetChangeCache() const
{
	return ChangeCache.AsConst();
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
 * GetPosition
 *************************************/
SHVDataRowKey* SHVDataRowListSQLite::GetPosition()
{
	return RowList->GetPosition();
}

/*************************************
 * SetPosition
 *************************************/
SHVBool SHVDataRowListSQLite::SetPosition(const SHVDataRowKey* key)
{
	return RowList->SetPosition(key);
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
			if (!ChangeCache.IsNull())
				ChangeCache->AddItem(row, FullRowCache);
			RowChanged(rrow);
			retVal = UpdateRow(row);
			if (retVal)
			{
				if (pos)
				{
					if (pos == PendingPosAdded)
					{
						PendingPosAdded = NULL;
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
				if (ChangeCache.IsNull() || !FullRowCache)
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
 * GetDataChangeFunction
 *************************************/
SHVDataFunction* SHVDataRowListSQLite::GetDataChangeFunction(int func)
{
SHVDataFunction* retVal = NULL;
	switch (func)
	{
	case ChangeFunctionAdd: retVal = InsertFunction; break;
	case ChangeFunctionUpdate: retVal = UpdateFunction; break;
	case ChangeFunctionDelete: retVal = DeleteFunction; break;
	}
	return retVal;
}

bool SHVDataRowListSQLite::SetDataChangeFunction(SHVDataFunction* datafunc, int func)
{
bool retVal = true;
	switch (func)
	{
	case ChangeFunctionAdd: InsertFunction = datafunc; break;
	case ChangeFunctionUpdate: UpdateFunction = datafunc; break;
	case ChangeFunctionDelete: DeleteFunction = datafunc; break;
	default: retVal = false;
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
 * InternalRowChanged
 *************************************/
SHVBool SHVDataRowListSQLite::InternalRowChanged(SHVDataRow* row)
{
	SHVUNUSED_PARAM(row);
	return SHVBool::True;
}

/*************************************
 * GetAliasID
 *************************************/
int SHVDataRowListSQLite::GetAliasID() const
{
	return RowList->GetAliasID();
}
