#include "stdafx.h"
#include "../../../../include/platformspc.h"
#include "../../../../include/utils/shvstringutf8.h"
#include "../../include/dataengineimpl/shvdatarowlist_sqlite.h"
#include "../../include/dataengineimpl/shvdatarowc_sqlite.h"
#include "../../include/dataengineimpl/shvdatarow_impl.h"


SHVDataRowList_SQLite::SHVDataRowList_SQLite(SHVDataTable_SQLite* dataTable, const SHVStringC& sql)
{
SHVString notparsed;
	Statement = dataTable->SQLite->Prepare(Ok, sql, notparsed);
	Table = dataTable;
}

SHVDataRowList_SQLite::~SHVDataRowList_SQLite()
{
	for (size_t i = ChangedRows.CalculateCount(); i; i--)
		ChangedRows[i]->ClearOwnership();
	for (size_t i = CommittedRows.CalculateCount(); i; i--)
		CommittedRows[i]->ClearOwnership();
	for (size_t i = NewRows.CalculateCount(); i; i--)
		NewRows[i]->ClearOwnership();
}

const SHVDataRowC* SHVDataRowList_SQLite::GetCurrentRow() const
{
	SHVASSERT(Ok == SHVBool::True);
	if (!Ok)
		return NULL;

size_t index;
SHVDataRowKeyRef Key = CurrentRow->GetKey(0);
SHVDataRowRef ChangedRow = (SHVDataRow *) Find(ChangedRows, Key, index);

	if (ChangedRow.IsNull())
		return CurrentRow.AsConst();
	else
		return (const SHVDataRowC *) ChangedRow;
}

SHVBool SHVDataRowList_SQLite::NextRow()
{
	SHVASSERT(Ok == SHVBool::True);
	if (!Ok)
		return SHVBool::False;

Eof = Statement->NextResult().GetError() != SHVSQLiteWrapper::SQLite_ROW;
size_t index;

	if (!Eof)
	{
		if (CurrentRow.IsNull())
			CurrentRow = new SHVDataRowC_SQLite(this);
		SHVDataRowKeyRef Key = CurrentRow->GetKey(0);
		SHVDataRowRef ChangedRow = Find(ChangedRows, Key, index);
		while (!Eof && !ChangedRow.IsNull() && ChangedRow->GetRowState() == SHVDataRow::SHVDataRowState_Deleted)
		{
			Eof = Statement->NextResult().GetError() != SHVSQLiteWrapper::SQLite_ROW;
			if (!Eof)
			{
				Key = CurrentRow->GetKey(0);
				ChangedRow = Find(ChangedRows, Key, index);
			}
			else
				ChangedRow = NULL;
		}
	}
	return Eof;
}

SHVBool SHVDataRowList_SQLite::Reset()
{
	CurrentRow = NULL;
	Eof = !Ok;
	return Statement->Reset();
}

SHVDataRow* SHVDataRowList_SQLite::EditCurrentRow()
{	
	SHVASSERT(Ok == SHVBool::True && !Eof);
	if (!Ok || Eof)
		return NULL;

size_t index;
SHVDataRowKeyRef key = CurrentRow->GetKey(0);
SHVDataRowRef ChangedRow = Find(ChangedRows, key, index);

	if (ChangedRow.IsNull())
	{
	SHVDataRowRef CommittedRow = Find(CommittedRows, key, index);
		if (CommittedRow.IsNull())
			ChangedRow = new SHVDataRow_impl(CurrentRow, this);
		else
		{
			CommittedRows.Replace(index, NULL);
			ChangedRow = CommittedRow;
		}		
		if (EmptySlot(ChangedRows, index))
			ChangedRows.Replace(index, ChangedRow);
		else
			ChangedRows.Add(ChangedRow);
	}
	return ChangedRow;
}

SHVDataRow* SHVDataRowList_SQLite::AddRow()
{
	SHVASSERT(Ok == SHVBool::True);
	if (!Ok)
		return NULL;

SHVDataRow* retVal = new SHVDataRow_impl(this);	
	NewRows.Add(retVal);
	return retVal;
}

SHVBool SHVDataRowList_SQLite::CommitChanges()
{
	SHVASSERT(Ok == SHVBool::True);
	if (!Ok)
		return SHVBool::False;

SHVStringUTF8 Columns;
SHVStringUTF8 Values;
SHVStringUTF8 Condition;
SHVStringUTF8 Sql;
SHVStringUTF8 Query;
SHVDataStructCRef StructRef = (SHVDataStructC*) Table->GetDataStruct();
SHVBool retVal;
const SHVDataStructC& Struct = *StructRef;

	SHVASSERT(!ChangedRows.CalculateCount() && !NewRows.CalculateCount());

	if (ChangedRows.CalculateCount() || NewRows.CalculateCount())
		return SHVBool::False;
	
	for (size_t row = 0; row < CommittedRows.CalculateCount(); row++)
	{
	SHVDataRowRef dataRow = (SHVDataRow_impl*) CommittedRows[row];

		if (row)
			Query += ";";

		if (CommittedRows[row]->GetRowState() == SHVDataRow::SHVDataRowState_Changed ||
			CommittedRows[row]->GetRowState() == SHVDataRow::SHVDataRowState_Deleted
			)
		{
		SHVPtrContainer<SHVDataVariant> Val = dataRow->GetValue(row);
		SHVDataRowKeyRef key = dataRow->GetKey(0);
			for (size_t k = 0; 0 < key->Count(); k++)
			{
				if (k)
					Condition += " AND ";
				Condition += (*key)[k].Key.GetSafeBuffer();
				Condition += " = ";
				Condition += SHVString((*key)[k].Value->AsString()).ToStrUTF8();
			}
			if (CommittedRows[row]->GetRowState() == SHVDataRow::SHVDataRowState_Changed)
			{
				for (size_t col = 0; col < Struct.GetColumnCount(); col++)
				{
					if (col)
						Columns += ", ";
					if (Struct[col]->GetDataType() == SHVDataVariant::SHVDataType_String ||
						Struct[col]->GetDataType() == SHVDataVariant::SHVDataType_Time)
					{
						Values.Format("SET %s = '%s'", Struct[col]->GetColumnName().GetSafeBuffer(), Val->AsString().ToStrUTF8().GetSafeBuffer());
					}
					else
					{
						Values.Format("SET %s = %s", Struct[col]->GetColumnName().GetSafeBuffer(), Val->AsString().ToStrUTF8().GetSafeBuffer());
					}
					Columns += Values;
				}
				Sql.Format("UPDATE %s %s WHERE %s", Table->GetDataStruct()->GetTableName().GetSafeBuffer(), Columns.GetSafeBuffer(), Condition);
			}
			else
			{
				Sql.Format("DELETE FROM %s WHERE %s", Table->GetDataStruct()->GetTableName().GetSafeBuffer(), Condition);
			}
		}
		else
		{
			if (CommittedRows[row]->GetRowState() == SHVDataRow::SHVDataRowState_New)
			{
				for (size_t col = 0; col < Struct.GetColumnCount(); col++)
				{
				SHVPtrContainer<SHVDataVariant> Val = dataRow->GetValue(row);
					if (col)
						Columns += ", ";
					Columns += Struct[col]->GetColumnName().GetSafeBuffer();
					if (col)
						Values += ", ";
					if (Struct[col]->GetDataType() == SHVDataVariant::SHVDataType_String ||
						Struct[col]->GetDataType() == SHVDataVariant::SHVDataType_Time)
					{
						Values += "'";
						Values += Val->AsString().ToStrUTF8();
						Values += "'";
					}
					else
					{
						Values += Val->AsString().ToStrUTF8();
					}
				}
				Sql.Format("INSERT INTO %s (%s) VALUES(%s)", Struct.GetTableName().GetSafeBuffer(), Columns.GetSafeBuffer(), Values.GetSafeBuffer());
			}
		}
		Query += Sql;
	}
	SHVStringUTF8 notparsed;
	SHVSQLiteStatementRef SqlStatement = ((SHVDataTable_SQLite*) Table)->SQLite->PrepareUTF8(retVal, Query, notparsed);
	if (retVal)
	{
		if ((retVal = SqlStatement->NextResult()).GetError() == SHVSQLiteWrapper::SQLite_DONE)
		{
			for (size_t i = CommittedRows.CalculateCount(); i; i--)
				CommittedRows[i]->ClearOwnership();
			CommittedRows.Clear();
		}			
	}
	return retVal;
}

SHVBool SHVDataRowList_SQLite::CancelChanges()
{
	SHVASSERT(Ok == SHVBool::True);
	if (!Ok)
		return SHVBool::False;

	for (size_t i = CommittedRows.CalculateCount(); i; i--)
		CommittedRows[i]->ClearOwnership();
	CommittedRows.Clear();
	for (size_t i = ChangedRows.CalculateCount(); i; i--)
		ChangedRows[i]->ClearOwnership();
	ChangedRows.Clear();
	for (size_t i = NewRows.CalculateCount(); i; i--)
		NewRows[i]->ClearOwnership();
	NewRows.Clear();
	return SHVBool::True;;
}

const SHVDataRowVector* SHVDataRowList_SQLite::GetChangedRows() 
{
	return &CommittedRows;
}

SHVBool SHVDataRowList_SQLite::AcceptChanges(SHVDataRow* dataRow)
{
	SHVASSERT(Ok == SHVBool::True);
	if (!Ok)
		return SHVBool::False;

size_t pos;
size_t emptySlot;
bool found = false;

	for (pos = ChangedRows.CalculateCount(); pos && !found; pos--)
	{
		found = ChangedRows[pos] == dataRow;
	}
	if (found)
	{
		if (EmptySlot(CommittedRows, emptySlot))
			CommittedRows.Replace(emptySlot, dataRow);
		else
			CommittedRows.Add(dataRow);
		ChangedRows.Replace(pos, NULL);
		((SHVDataRow_impl*) dataRow)->RowState = SHVDataRow::SHVDataRowState_Unchanged;
		found = true;
	}
	else
	{
		found = false;
		for (pos = NewRows.CalculateCount(); pos && !found; pos--)
		{
			found = NewRows[pos] == dataRow;
		}
		if (found)
		{
			if (EmptySlot(CommittedRows, emptySlot))
				CommittedRows.Replace(emptySlot, dataRow);
			else
				CommittedRows.Add(dataRow);
			NewRows.Replace(pos, NULL);
		}
	}
	return found;
}

SHVBool SHVDataRowList_SQLite::RejectChanges(SHVDataRow* dataRow)
{
	SHVASSERT(Ok == SHVBool::True);
	if (!Ok)
		return SHVBool::False;

size_t pos;
SHVDataRow* row;
	if ((row = Find(ChangedRows, dataRow->GetKey(0), pos)))
	{
		row->ClearOwnership();
		ChangedRows.Replace(pos, NULL);
		return SHVBool::True;
	}
	else
		return SHVBool::False;
}

const SHVDataStructC* SHVDataRowList_SQLite::GetStruct() const
{
	return Table->GetDataStruct();
}

const void* SHVDataRowList_SQLite::GetProvider() const
{
	SHVASSERT(Ok == SHVBool::True);
	if (!Ok)
		return NULL;
	return Statement.AsConst();
}

SHVBool SHVDataRowList_SQLite::IsOk() const
{
	return Ok;
}

SHVDataRow* SHVDataRowList_SQLite::Find(const SHVDataRowVector& vector, const SHVDataRowKey* key, size_t& idx) const
{
	SHVASSERT(Ok == SHVBool::True);
	if (!Ok)
		return NULL;

SHVDataRow* found = NULL;
	for (idx = vector.CalculateCount(); idx && !found; idx--)
	{		
		if (vector[idx] && vector[idx]->MatchKey(key))
			found = (SHVDataRow*) vector[idx];
	}
	return found;
}

bool SHVDataRowList_SQLite::EmptySlot(const SHVDataRowVector& vector, size_t& idx)
{
bool found = false;
	for (idx = vector.CalculateCount(); idx && !found; idx--)
	{
		found = vector[idx] == NULL;	
	}
	return found;
}

