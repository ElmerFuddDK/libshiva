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
#include "../include/shvdatarowlistcsqlite.h"
#include "../include/shvdatarowcsqlite.h"
#include "../include/shvdatarowimpl.h"
#include "../include/shvdatarowkeyimpl.h"
#include "../include/shvdatastructimpl.h"
#include "../include/shvdatastructcsqlite.h"
#include "../../../include/modules/dataengine/shvdataengine.h"
#include "../../../include/sqlite/sqlitestatement.h"

/*************************************
 * Constructor
 *************************************/
SHVDataRowListCSQLite::SHVDataRowListCSQLite(SHVDataSession* session, const SHVDataStructC* dataStruct, const SHVString8C& alias): DataSession(session), StructCache((SHVDataStructC*)dataStruct), RowCount(-1), Alias(alias), Ok(true), HasShareLock(false)
{
	AliasID = SHVDataRowListC::GetAliasID(alias);
}

SHVDataRowListCSQLite::SHVDataRowListCSQLite(SHVDataSession* session, const SHVStringC& sql, const SHVDataRowKey* sortKey): DataSession(session), RowCount(-1), Alias(""), HasShareLock(false)
{
SHVString rest;
SHVDataStructImpl* st = new SHVDataStructImpl();
SHVSQLiteWrapperRef SQLite = (SHVSQLiteWrapper*) session->GetProvider();
	AliasID = -1;
	StructCache = st;
	Statement = SQLite->Prepare(Ok, sql, rest);
	if (sortKey)
		st->AddIndex((SHVDataRowKey*) sortKey);
	SortIndex = 0;	
	Eof = !Ok;
}

SHVDataRowListCSQLite::SHVDataRowListCSQLite(SHVDataSession* session, const SHVDataStructC* dataStruct, const SHVString8C& alias, const SHVStringC& condition, size_t index): DataSession(session), StructCache((SHVDataStructC*)dataStruct), RowCount(-1), Alias(alias), HasShareLock(false)
{
SHVStringSQLite rest(NULL);
SHVSQLiteWrapperRef SQLite = (SHVSQLiteWrapper*) session->GetProvider();
SHVStringUTF8 sql;

	Ok = SHVBool::True;
	AliasID = SHVDataRowListC::GetAliasID(alias);
	SHVASSERT(dataStruct->GetIndex(index));
	if (!dataStruct->GetIndex(index))
	{
		Ok = SHVBool::False;
		return;
	}
	SortIndex = index;
	sql = BuildQuery(condition, false);
	if (Ok)
		Statement = SQLite->PrepareUTF8(Ok, sql, rest);
	Eof = !Ok;
}

SHVDataRowListCSQLite::SHVDataRowListCSQLite(SHVDataSession* session, SHVSQLiteStatement* statement, const SHVDataStructC* dataStruct, const SHVString8C& alias, size_t index): DataSession(session), StructCache((SHVDataStructC*)dataStruct), RowCount(-1), Alias(alias)
{
	SHVASSERT(dataStruct->GetIndex(index));
	AliasID = SHVDataRowListC::GetAliasID(alias);
	Ok = SHVBool::True;
	if (!dataStruct->GetIndex(index))
	{
		Ok = SHVBool::False;
		return;
	}
	SortIndex = index;
	Statement = statement;
	Eof = !Ok;
}

/*************************************
 * GetCurrentRow
 *************************************/
const SHVDataRowC* SHVDataRowListCSQLite::GetCurrentRow() const
{
	SHVASSERT(IsOk());
	if (!Eof)
	{
		return CurrentRow.AsConst();
	}
	else
		return NULL;
}

/*************************************
 * IsOk
 *************************************/
SHVBool SHVDataRowListCSQLite::IsOk() const
{
	return RowListValid() && Ok;
}


/*************************************
 * GetStruct
 *************************************/
const SHVDataStructC* SHVDataRowListCSQLite::GetStruct() const
{
	return StructCache.AsConst();
}

/*************************************
 * GetAlias
 *************************************/
const SHVString8C SHVDataRowListCSQLite::GetAlias() const
{
	return Alias;
}

/*************************************
 * GetAliasID
 *************************************/
int SHVDataRowListCSQLite::GetAliasID() const
{
	return AliasID;
}

/*************************************
 * GetRowCount
 *************************************/
int SHVDataRowListCSQLite::GetRowCount() const
{
	return RowCount;
}

/*************************************
 * Find
 *************************************/
SHVDataRowC* SHVDataRowListCSQLite::Find(const SHVDataRowKey* key)
{
const SHVDataRowKey& k = *key;
SHVDataRowC* retVal = NULL;
	SHVASSERT(IsOk());
	if (IsOk())
	{
		if (RowCount < 0)
		{
			NextRow();
		}
		Reset();
		for (size_t i = key->Count(); i;)
		{
		SHVStringUTF8 keyParm;
			keyParm.Format("@%s", k[--i].Key.GetSafeBuffer());
			if (k[i].Value && !k[i].Value->IsNull())
			{
				if (k[i].Value->GetDataType() == SHVDataVariant::TypeInt)
					Statement->SetParameterLongUTF8(keyParm, k[i].Value->AsInt());
				else
				if (k[i].Value->GetDataType() == SHVDataVariant::TypeInt64)
					Statement->SetParameterInt64UTF8(keyParm, k[i].Value->AsInt64());
				else
				if (k[i].Value->GetDataType() == SHVDataVariant::TypeBool)
					Statement->SetParameterLongUTF8(keyParm, k[i].Value->AsInt());
				else
				if (k[i].Value->GetDataType() == SHVDataVariant::TypeDouble)
					Statement->SetParameterDoubleUTF8(keyParm, k[i].Value->AsDouble());
				else
				if (k[i].Value->GetDataType() == SHVDataVariant::TypeString)
					Statement->SetParameterStringUTF8(keyParm, k[i].Value->AsString().ToStrUTF8());
				else
				if (k[i].Value->GetDataType() == SHVDataVariant::TypeTime)
					Statement->SetParameterStringUTF8(keyParm, k[i].Value->AsString().ToStrUTF8());
				else
					Statement->SetParameterNullUTF8(keyParm);
			}
			else
				Statement->SetParameterNullUTF8(keyParm);
		}
		if (NextRow())
			retVal = CurrentRow;
	}
	return retVal;
}

/*************************************
 * GetPosition
 *************************************/
SHVDataRowKey* SHVDataRowListCSQLite::GetPosition()
{
SHVDataRowKey* retVal = NULL;
	if (GetCurrentRow())
		retVal = GetCurrentRow()->GetKey(SortIndex);
	return retVal;
}

/*************************************
 * SetPosition
 *************************************/
SHVBool SHVDataRowListCSQLite::SetPosition(const SHVDataRowKey* key)
{
SHVDataRowCRef row = Find(key);
	return !row.IsNull() && row->MatchKey(key);
}

/*************************************
 * NextRow
 *************************************/
SHVBool SHVDataRowListCSQLite::NextRow()
{
SHVBool retVal = IsOk();
	SHVASSERT(retVal);
	if (retVal)
	{
		if (RowCount < 0)
		{
			RowCount = 0;
			while (Statement->NextResult().GetError() == SHVSQLiteWrapper::SQLite_ROW) RowCount++;
			Reset();
		}
		LockShared();
		if (CurrentRow.IsNull())
			CurrentRow = new SHVDataRowCSQLite(this);
		retVal = Statement->NextResult();
		Eof = retVal.GetError() != SHVSQLiteWrapper::SQLite_ROW;
		if (StructCache->GetColumnCount() == 0 && !Eof)
		{
		SHVStringSQLite colName(NULL);
		SHVStringSQLite type(NULL);
		int shvtype;
		int len;
			for (int col = 0; col < Statement->GetColumnCount(); col++)
			{
				if (Statement->GetColumnNameUTF8(colName, col))
				{
					Statement->GetColumnTypeUTF8(type, col);
					len = -1;
					shvtype = SHVDataVariant::TypeUndefined;
					if (type == __SQLITE_TYPE_INT)
						shvtype = SHVDataVariant::TypeInt;
					else
					if (type == __SQLITE_TYPE_INT64)
						shvtype = SHVDataVariant::TypeInt64;
					else
					if (type == __SQLITE_TYPE_DOUBLE)
						shvtype = SHVDataVariant::TypeDouble;
					else
					if (type == __SQLITE_TYPE_BOOL)
						shvtype = SHVDataVariant::TypeBool;
					else
					if (type == __SQLITE_TYPE_DATETIME)
						shvtype = SHVDataVariant::TypeTime;
					else
					if (type.Find(__SQLITE_TYPE_STRING) == 0)
					{
						shvtype = SHVDataVariant::TypeString;
						len = SHVString8C::StrToL(type.Mid(SHVString8C::StrLen(__SQLITE_TYPE_STRING) + 1).GetBufferConst(), NULL, 10);
					}
					((SHVDataStruct&) *StructCache).Add(colName.GetSafeBuffer(), shvtype, len);
				}
			}		
		}
		if (retVal.GetError() == SHVSQLiteWrapper::SQLite_ROW)
			retVal = SHVBool::True;
	}
	return retVal;
}

/*************************************
 * Reverse
 *************************************/
SHVDataRowListC* SHVDataRowListCSQLite::Reverse(const SHVStringC& condition)
{
SHVBool ok;
SHVStringSQLite rest(NULL);
SHVSQLiteWrapperRef SQLite;
SHVSQLiteStatement* statement;
SHVDataRowListC* retVal = NULL;

	if (IsOk())
	{
		SQLite = (SHVSQLiteWrapper*) DataSession->GetProvider();
		statement = SQLite->PrepareUTF8(ok, BuildQuery(condition,true), rest);
		retVal = new SHVDataRowListCSQLite(DataSession, statement, StructCache, Alias, SortIndex);
	}
	return retVal;
}

/*************************************
 * Reset
 *************************************/
SHVBool SHVDataRowListCSQLite::Reset()
{
SHVBool retVal = IsOk();
	SHVASSERT(retVal);
	if (retVal)
	{
		Eof = SHVBool::False;
		retVal = Statement->Reset();
		if (GetStruct()->IndexCount())
		{
		const SHVDataRowKey& key = *GetStruct()->GetIndex(SortIndex);
			for (size_t i = 0; i < key.Count(); i++)
			{
			SHVStringUTF8 parm;
				parm.Format("@%s", key[i].Key.GetSafeBuffer());
				Statement->SetParameterNullUTF8(parm);
			}
		}
		if (!retVal)
			Eof = SHVBool::True;
		Unlock();
	}
	return retVal;
}

/*************************************
 * GetRowProvider
 *************************************/
const void* SHVDataRowListCSQLite::GetRowProvider() const
{
	return Statement.AsConst();
}

/*************************************
 * GetDataSession
 *************************************/
SHVDataSession* SHVDataRowListCSQLite::GetDataSession()
{
	return DataSession;
}

/*************************************
 * RowListValid
 *************************************/
SHVBool SHVDataRowListCSQLite::RowListValid() const
{
	return !DataSession.IsNull() && DataSession->SessionValid();
}

/*************************************
 * Destructor
 *************************************/
SHVDataRowListCSQLite::~SHVDataRowListCSQLite()
{
	if (HasShareLock)
		DataSession->GetFactory()->Unlock();
	Statement = NULL;
	DataSession = NULL;
}

/*************************************
 * BuildQuery
 *************************************/
SHVStringBufferUTF8 SHVDataRowListCSQLite::BuildQuery(const SHVStringC& condition, bool reverse)
{
SHVStringUTF8 queryUTF8;
SHVString8 condition8;
SHVString8 orderby8;

	GetDataSession()->GetFactory()->GetDataEngine().BuildKeySQL(StructCache->GetIndex(SortIndex), condition8, orderby8,"",reverse);
	if (!condition.IsNull() && condition != _T(""))
	{
		queryUTF8.Format("select * from %s where (%s) and %s order by %s", 
			Alias.GetSafeBuffer(),
			condition.ToStrUTF8().GetSafeBuffer(),
			condition8.GetSafeBuffer(),
			orderby8.GetSafeBuffer());
	}
	else
	{
		queryUTF8.Format("select * from %s where %s order by %s", 
			Alias.GetSafeBuffer(),
			condition8.GetSafeBuffer(),
			orderby8.GetSafeBuffer());
	}
	return queryUTF8.ReleaseBuffer();
}

/*************************************
 * AdjustRowCount
 *************************************/
void SHVDataRowListCSQLite::AdjustRowCount(int delta)
{
	RowCount += delta;
}

/*************************************
 * LockShared
 *************************************/
void SHVDataRowListCSQLite::LockShared()
{
	if (!HasShareLock && IsOk())
	{
		GetDataSession()->GetFactory()->LockShared();
		HasShareLock = true;
	}
}

/*************************************
 * Unlock
 *************************************/
void SHVDataRowListCSQLite::Unlock()
{
	if (HasShareLock && IsOk())
	{
		HasShareLock = false;
		GetDataSession()->GetFactory()->Unlock();
	}
}

/*************************************
 * InternalRowChanged
 *************************************/
SHVBool SHVDataRowListCSQLite::InternalRowChanged(SHVDataRow* row)
{
	return SHVBool::True;
}


