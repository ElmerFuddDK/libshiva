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
#include "../include/shvdatastructcsqlite.h"
#include "../include/shvdatarowkeyimpl.h"

/*************************************
 * Constructor
 *************************************/
SHVDataStructCSQLite::SHVDataStructCSQLite(SHVSQLiteWrapper* psqlite, const SHVString8C dbTableName)
{
SHVStringUTF8 query;
SHVStringSQLite notparsed(NULL);
SHVStringSQLite valUTF8(NULL);
SHVSQLiteStatementRef statement;
SHVSQLiteWrapperRef sqlite = psqlite;
int len;
SHVBool ok;
	Struct = new SHVDataStructImpl();
	Struct->SetTableName(dbTableName);
	Struct->SetIsMultiInstance(false);

	query.Format("PRAGMA table_info(%s)", dbTableName.GetSafeBuffer());

	statement = sqlite->PrepareUTF8(ok, query, notparsed);

	if (ok)
	{
		// Lets get the table structure
		while (statement->NextResult().GetError() == SHVSQLiteWrapper::SQLite_ROW)
		{
		SHVDataStructColumnImplRef col;
		SHVString val;
		SHVStringUTF8C type("");
			if (statement->GetStringUTF8(valUTF8, len, 1) && valUTF8 != "shv_alias")
			{
				col = new SHVDataStructColumnImpl();
				col->SetColumnName(SHVString8C(valUTF8.GetSafeBuffer()));
				if (statement->GetStringUTF8(valUTF8, len, 2))
				{
					col->SetDataLength(-1);
					if (!SHVStringUTF8::StrCaseCmp(valUTF8.GetSafeBuffer(), __SQLITE_TYPE_INT))
						col->SetDataType(SHVDataVariant::TypeInt);
					if (!SHVStringUTF8::StrCaseCmp(valUTF8.GetSafeBuffer(), __SQLITE_TYPE_INT64))
						col->SetDataType(SHVDataVariant::TypeInt64);
					if (!SHVStringUTF8::StrCaseCmp(valUTF8.GetSafeBuffer(), __SQLITE_TYPE_DOUBLE))
						col->SetDataType(SHVDataVariant::TypeDouble);
					if (!SHVStringUTF8::StrCaseCmp(valUTF8.GetSafeBuffer(), __SQLITE_TYPE_DATETIME))
						col->SetDataType(SHVDataVariant::TypeTime);
					if (!SHVStringUTF8::StrCaseCmp(valUTF8.GetSafeBuffer(), __SQLITE_TYPE_BOOL))
						col->SetDataType(SHVDataVariant::TypeBool);
					if (!SHVStringUTF8::StrCaseCmp(valUTF8.Left(SHVString8::StrLen(__SQLITE_TYPE_STRING)).GetSafeBuffer(), __SQLITE_TYPE_STRING))
					{
						col->SetDataType(SHVDataVariant::TypeString);
						col->SetDataLength(SHVString8C::StrToL(valUTF8.GetSafeBuffer() + SHVString8::StrLen(__SQLITE_TYPE_STRING) + 1, NULL, 10));
					}				
				}
				if (statement->GetStringUTF8(valUTF8, len, 3))
					col->SetAllowNull(valUTF8 == "1");
				else
					col->SetAllowNull(SHVBool::True);
				col->SetAutoInc(SHVBool::False);
				Struct->Add(col);
			}
			else
				Struct->SetIsMultiInstance(true);
		}
		// now lets find the primary index
		query.Format("sql is null", dbTableName.GetSafeBuffer());
		ResolveIndexes(psqlite, dbTableName.GetSafeBuffer(), query);
		// now lets find the secondary indexes
		query.Format("name like '%s%%'", dbTableName.GetSafeBuffer());
		ResolveIndexes(psqlite, dbTableName.GetSafeBuffer(), query);
	}
}

/*************************************
 * GetTableName
 *************************************/
const SHVString8C& SHVDataStructCSQLite::GetTableName() const
{
	return Struct->GetTableName();
}

/*************************************
 * GetIsMultiInstance
 *************************************/
bool SHVDataStructCSQLite::GetIsMultiInstance() const
{
	return Struct->GetIsMultiInstance();
}

/*************************************
 * operator[]
 *************************************/
const SHVDataStructColumnC* SHVDataStructCSQLite::operator[](size_t idx) const
{
	return (*Struct.AsConst())[idx];
}

/*************************************
 * FindColumnIndex
 *************************************/
SHVBool SHVDataStructCSQLite::FindColumnIndex(size_t& index, const SHVString8C& colName) const
{
	return Struct->FindColumnIndex(index, colName);
}

/*************************************
 * GetColumnCount
 *************************************/
size_t SHVDataStructCSQLite::GetColumnCount() const
{
	return Struct->GetColumnCount();
}

/*************************************
 * GetPrimaryIndex
 *************************************/
const SHVDataRowKey* SHVDataStructCSQLite::GetPrimaryIndex() const
{
	return Struct->GetPrimaryIndex();
}

/*************************************
 * GetIndex
 *************************************/
const SHVDataRowKey* SHVDataStructCSQLite::GetIndex(size_t IdxID) const
{
	return Struct->GetIndex(IdxID);
}

/*************************************
 * FindIndex
 *************************************/
size_t SHVDataStructCSQLite::FindIndex(const SHVDataRowKey* key) const
{
	return Struct->FindIndex(key);
}

/*************************************
 * IndexCount
 *************************************/
size_t SHVDataStructCSQLite::IndexCount() const
{
	return Struct->IndexCount();
}

/*************************************
 * IsEqual
 *************************************/
SHVBool SHVDataStructCSQLite::IsEqual(const SHVDataStructC* dataStruct, bool strict) const
{
	return Struct->IsEqual(dataStruct, strict);
}
/*************************************
 * IsEqual
 *************************************/
SHVBool SHVDataStructCSQLite::IsIndexesEqual(const SHVDataStructC* dataStruct) const
{
	return Struct->IsIndexesEqual(dataStruct);
}

/*************************************
 * Destructor
 *************************************/
SHVDataStructCSQLite::~SHVDataStructCSQLite()
{
}

/*************************************
 * ResolveIndexes
 *************************************/
void SHVDataStructCSQLite::ResolveIndexes(SHVSQLiteWrapper* psqlite, const SHVStringUTF8C& tableName, const SHVStringUTF8C& condition)
{
SHVStringUTF8 query;
SHVSQLiteStatementRef statement;
SHVSQLiteStatementRef indexInfo;
SHVStringSQLite name(NULL);
SHVStringSQLite sql(NULL);
SHVStringSQLite notparsed(NULL);
SHVStringSQLite valUTF8(NULL);
SHVStringUTF8 nameDesc;
SHVSQLiteWrapperRef sqlite = psqlite;
SHVBool ok;
bool hasSQL;
int len;
short affinity;

	query.Format("select name, sql from sqlite_master where tbl_name='%s' and type='index' and %s order by name", tableName.GetSafeBuffer(), condition.GetSafeBuffer());
	statement = sqlite->PrepareUTF8(ok, query, notparsed);
	SHVASSERT(!statement.IsNull() || !ok);
	if (ok)
	{
		while (statement->NextResult().GetError() == SHVSQLiteWrapper::SQLite_ROW)
		{
			if (statement->GetStringUTF8(name, len, 0))
			{
				if (statement->GetColumnAffinity(affinity, 1) && affinity != SHVSQLiteWrapper::Affinity_Null)
					hasSQL = statement->GetStringUTF8(sql, len, 1);
				else
					hasSQL = false;
				query.Format("PRAGMA index_info(%s)", name.GetSafeBuffer());
				indexInfo = sqlite->PrepareUTF8(ok, query, notparsed);
				SHVASSERT(!indexInfo.IsNull() || !ok);
				if (ok)
				{
				SHVDataRowKeyImplRef index = new SHVDataRowKeyImpl();
					while (indexInfo->NextResult().GetError() == SHVSQLiteWrapper::SQLite_ROW)
					{
						if (indexInfo->GetStringUTF8(valUTF8, len, 2))
						{
						size_t colIdx;
							if (valUTF8 != "shv_alias" && Struct->FindColumnIndex(colIdx, valUTF8.GetSafeBuffer()))
							{
								if (hasSQL)
								{
								bool desc = false;
									nameDesc.Format(" %s desc", valUTF8.GetSafeBuffer());
									desc = sql.Find(nameDesc) >= 0;
									if (!desc)
									{
										nameDesc.Format("(%s desc", valUTF8.GetSafeBuffer()); 
										desc = sql.Find(nameDesc) >= 0;
									}
									index->AddKey(valUTF8.ToStr8(), desc);
								}
								else
									index->AddKey(valUTF8.ToStr8(), false);
							}
						}
					}
					Struct->AddIndex(index);
				}
			}
		}
	}
}

/*************************************
 * GetInternalStruct
 *************************************/
SHVDataStruct* SHVDataStructCSQLite::GetInternalStruct()
{
	return Struct;
}
