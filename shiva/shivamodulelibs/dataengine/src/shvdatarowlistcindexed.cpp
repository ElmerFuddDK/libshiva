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
#include "../../../include/utils/shvstringutf8.h"
#include "../include/shvdatarowlistcindexed.h"
#include "../include/shvdatarowcsqlite.h"
#include "../include/shvdatarowimpl.h"
#include "../include/shvdatastructcsqlite.h"
#include "../include/shvdatarowkeyimpl.h"
#include "../../../include/modules/dataengine/shvdataengine.h"


/*************************************
 * Constructor
 *************************************/
SHVDataRowListCIndexed::SHVDataRowListCIndexed(SHVDataSession* session, const SHVDataStructC* dataStruct, const SHVString8C& alias, const SHVStringC& condition, size_t index): SHVDataRowListCSQLite(session, dataStruct, alias)
{
SHVStringSQLite rest(NULL);
SHVSQLiteWrapperRef SQLite = (SHVSQLiteWrapper*) session->GetProvider();
SHVStringUTF8 query;
	SortIndex = index;
	SHVASSERT(GetStruct()->GetIndex(index));
	if (!GetStruct()->GetIndex(index))
	{
		Ok = SHVBool::False;
		return;
	}
	query = BuildQuery(condition, false);
	if (Ok)
		Statement = SQLite->PrepareUTF8(Ok, query, rest);
	Eof = !Ok;
}

/*************************************
 * Reset
 *************************************/
SHVBool SHVDataRowListCIndexed::Reset()
{
SHVBool retVal = IsOk();
	SHVASSERT(retVal);
	if (retVal)
	{
	const SHVDataRowKey& key = *GetStruct()->GetIndex(SortIndex);
		Eof = false;
		retVal = Statement->Reset();
		for (size_t i = 0; i < key.Count(); i++)
		{
		SHVStringUTF8 parm;
			parm.Format("@%s", key[i].Key.GetSafeBuffer());
			Statement->SetParameterNullUTF8(parm);
		}
		Statement->SetParameterNullUTF8("@idx");
		if (!retVal)
			Eof = SHVBool::True;
		Unlock();
	}
	return retVal;
}

/*************************************
 * GetRowCount
 *************************************/
int SHVDataRowListCIndexed::GetRowCount() const
{
	return RowCount;
}

/*************************************
 * GetPosition
 *************************************/
SHVDataRowKey* SHVDataRowListCIndexed::GetPosition()
{
SHVDataRowKey* retVal = NULL;
long p;
	if (GetCurrentRow() && Statement->GetLong(p, (int) GetStruct()->GetColumnCount()))
	{
		retVal = new SHVDataRowKeyImpl();	
		retVal->AddKeyWithValue("idx", new SHVDataVariantImpl(SHVInt(p)), false);
	}
	return retVal;
}

/*************************************
 * Destructor
 *************************************/
SHVDataRowListCIndexed::~SHVDataRowListCIndexed()
{
SHVSQLiteWrapperRef SQLite = (SHVSQLiteWrapper*) GetDataSession()->GetProvider();
SHVStringUTF8 sql;
SHVStringSQLite rest("");
SHVBool ok;
SHVSQLiteStatementRef statement;
	sql.Format("drop table memdb.%s", IndexTableName.GetSafeBuffer());
	Statement = NULL;
	statement = SQLite->ExecuteUTF8(ok, sql, rest);
}

/*************************************
 * BuildQuery
 *************************************/

SHVStringBufferUTF8 SHVDataRowListCIndexed::BuildQuery(const SHVStringC& condition, bool reverse)
{
SHVStringUTF8 queryUTF8;
SHVStringUTF8 col;
SHVStringUTF8 colDefs;
SHVStringUTF8 cols;
SHVStringUTF8 joinCond;
SHVStringUTF8 type;
SHVStringSQLite rest("");
SHVString8 condition8;
SHVString8 orderby8;
SHVSQLiteWrapperRef SQLite = (SHVSQLiteWrapper*) GetDataSession()->GetProvider();
const SHVDataStructC& Struct = *GetStruct();
const SHVDataRowKey& Key = *Struct.GetIndex(0);
bool first = true;
SHVSQLiteStatementRef statement;
long rc;

// Let the magic begin
	IndexTableName.Format("%s_%d", Alias.GetSafeBuffer(), (int) GetDataSession()->GetFactory()->GetDataEngine().GetModuleList().CreateTag());
	for (size_t i = 0; i < Key.Count(); i++)
	{
	size_t colIdx;
		if (Struct.FindColumnIndex(colIdx, Key[i].Key))
		{
			if (!first)
			{
				colDefs += ", ";
				cols += ", ";
				joinCond += " and ";
			}
			else
				first = false;
			switch (Struct[colIdx]->GetDataType())
			{
				case SHVDataVariant::TypeInt:
					type = __SQLITE_TYPE_INT;
					break;
				case SHVDataVariant::TypeInt64:
					type = __SQLITE_TYPE_INT64;
					break;
				case SHVDataVariant::TypeBool:
					type = __SQLITE_TYPE_BOOL;
					break;
				case SHVDataVariant::TypeDouble:
					type = __SQLITE_TYPE_DOUBLE;
					break;
				case SHVDataVariant::TypeString:
					type.Format("%s(%d)", __SQLITE_TYPE_STRING, Struct[colIdx]->GetDataLength());
					break;
				case SHVDataVariant::TypeTime:
					type = __SQLITE_TYPE_DATETIME;
			}
			col.Format("%s %s", Struct[colIdx]->GetColumnName().GetSafeBuffer(), type.GetSafeBuffer());
			colDefs += col;
			col.Format("memdb.%s.%s = %s.%s", 
				IndexTableName.GetSafeBuffer(),
				Struct[colIdx]->GetColumnName().GetSafeBuffer(),
				Alias.GetSafeBuffer(),
				Struct[colIdx]->GetColumnName().GetSafeBuffer());
			joinCond += col;
			cols += Struct[colIdx]->GetColumnName().GetSafeBuffer();
		}
	}
	queryUTF8.Format("create table memdb.%s(idx integer primary key autoincrement, %s)", 
		IndexTableName.GetSafeBuffer(), 
		colDefs.GetSafeBuffer());
	statement = SQLite->ExecuteUTF8(Ok, queryUTF8, rest);
	if (Ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
		Ok = SHVBool::True;
	queryUTF8.Format("create index memdb.%s_index on %s(%s)",
		IndexTableName.GetSafeBuffer(),
		IndexTableName.GetSafeBuffer(),
		cols.GetSafeBuffer());
	statement = SQLite->ExecuteUTF8(Ok, queryUTF8, rest);
	if (Ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
		Ok = SHVBool::True;
	if (Ok)
	{
		GetDataSession()->GetFactory()->GetDataEngine().BuildKeySQL(Struct.GetIndex(SortIndex), condition8, orderby8, Alias.GetSafeBuffer(), reverse);
		queryUTF8.Format("insert into memdb.%s(%s) select %s from %s %s %s order by %s", 
			IndexTableName.GetSafeBuffer(), cols.GetSafeBuffer(), 
			cols.GetSafeBuffer(), 
			Alias.GetSafeBuffer(),
			(condition.IsEmpty() ? "" : "where"),
			condition.ToStrUTF8().GetSafeBuffer(),
			orderby8.GetSafeBuffer()
		);
		statement = SQLite->ExecuteUTF8(Ok, queryUTF8, rest);
		if (Ok.GetError() == SHVSQLiteWrapper::SQLite_DONE)
			Ok = SHVBool::True;
		queryUTF8.Format("select count(*) from memdb.%s", IndexTableName.GetSafeBuffer());
		statement = SQLite->ExecuteUTF8(Ok, queryUTF8, rest);
		if (Ok.GetError() == SHVSQLiteWrapper::SQLite_ROW)
			Ok = SHVBool::True;			
		statement->GetLong(rc, 0);
		RowCount = rc;
	}
	if (Ok)
	{
		queryUTF8.Format("select %s.*,idx from %s join memdb.%s on %s where %s and (@idx is null or idx >= @idx) order by idx",
			Alias.GetSafeBuffer(),
			Alias.GetSafeBuffer(),
			IndexTableName.GetSafeBuffer(),
			joinCond.GetSafeBuffer(),
			condition8.GetSafeBuffer());
	}		
	return queryUTF8.ReleaseBuffer();	
}

/*************************************
 * InitializeFind
 *************************************/
void SHVDataRowListCIndexed::InitializeFind()
{
}

/*************************************
 * InternalRowChanged
 *************************************/
SHVBool SHVDataRowListCIndexed::InternalRowChanged(SHVDataRow* row)
{
SHVBool retVal = SHVBool::True;
SHVSQLiteStatementRef statement;
	if (row->GetRowState() == SHVDataRow::RowStateAdding)
	{
	SHVStringUTF8 val;
	SHVStringUTF8 vals;
	SHVStringUTF8 cols;
	SHVStringUTF8 sql;
	SHVStringSQLite rest("");
	SHVSQLiteWrapperRef SQLite = (SHVSQLiteWrapper*) GetDataSession()->GetProvider();
	SHVDataRowKeyRef key = row->GetKey(0);
	const SHVDataRowKey& Key = *key.AsConst();
		for (size_t i = 0; i < Key.Count(); i++)
		{
			if (!vals.IsEmpty())
			{
				vals += ", ";
				cols += ", ";
			}
			if (Key[i].Value && !Key[i].Value->IsNull())
			{
				if (Key[i].Value->GetDataType() == SHVDataVariant::TypeString ||
					Key[i].Value->GetDataType() == SHVDataVariant::TypeTime)
					val.Format("'%s'", Key[i].Value->AsString().ToStrUTF8().GetSafeBuffer());
				else
					val = Key[i].Value->AsString().ToStrUTF8().GetSafeBuffer();
				vals += val;
				cols += Key[i].Key.GetSafeBuffer();
			}
		}
		sql.Format("insert into memdb.%s(%s) values(%s)", IndexTableName.GetSafeBuffer(), cols.GetSafeBuffer(), vals.GetSafeBuffer());
		statement = SQLite->ExecuteUTF8(retVal, sql, rest);	
		if (retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
			retVal = SHVBool::True;
		else
			SHVTRACE(_S("SHVDataRowListCIndexed::InternalRowChanged failed with %s\n"), GetDataSession()->GetErrorMessage().GetSafeBuffer());
		if (retVal)
		{
			sql.Format("select max(idx) from memdb.%s", IndexTableName.GetSafeBuffer());
			statement = SQLite->ExecuteUTF8(retVal, sql, rest);
			if (retVal.GetError() == SHVSQLiteWrapper::SQLite_ROW)
				retVal = SHVBool::True;
		}
	}
	else
	if (row->GetRowState() == SHVDataRow::RowStateDeleting)
	{
	SHVStringUTF8 sql;
	SHVSQLiteWrapperRef SQLite = (SHVSQLiteWrapper*) GetDataSession()->GetProvider();
	SHVStringUTF8 condition;
	SHVStringUTF8 subcondition;
	SHVStringSQLite rest("");
	bool first = true;
	const SHVDataStructC& Struct = *GetStruct();
	const SHVDataRowKey& Key = *GetStruct()->GetIndex(0);

		for (size_t i = 0; i < Key.Count(); i++)
		{
		size_t colIdx;
			if (Struct.FindColumnIndex(colIdx, Key[i].Key))
			{
			const SHVDataVariant* value = row->OriginalValue(colIdx);
				if (!value->IsNull())
				{
					if (!first)
					{
						condition += " and ";
					}
					else
						first = false;
					if (Struct[colIdx]->GetDataType() == SHVDataVariant::TypeString ||
						Struct[colIdx]->GetDataType() == SHVDataVariant::TypeTime)
						subcondition.Format("%s = '%s'", Key[i].Key.GetSafeBuffer(), value->AsString().ToStrUTF8().GetSafeBuffer());
					else
						subcondition.Format("%s = %s", Key[i].Key.GetSafeBuffer(), value->AsString().ToStrUTF8().GetSafeBuffer());
					condition += subcondition;
				}
			}
		}
		sql.Format("delete from memdb.%s where %s", IndexTableName.GetSafeBuffer(), condition.GetSafeBuffer());
		statement = SQLite->ExecuteUTF8(retVal, sql, rest);
		if (retVal.GetError() == SHVSQLiteWrapper::SQLite_DONE)
			retVal = SHVBool::True;
	}
	return retVal;
}
