#include "stdafx.h"
#include "../../../../include/platformspc.h"
#include "../../../../include/utils/shvstringutf8.h"
#include "../../include/dataengineimpl/shvdatarowlist_sqlite.h"
#include "../../include/dataengineimpl/shvdatarowc_sqlite.h"
#include "../../include/dataengineimpl/shvdatarow_impl.h"


/*************************************
 * Constructor
 *************************************/
SHVDataRowList_SQLite::SHVDataRowList_SQLite(SHVDataSession* dataSession, SHVDataRowListC *rowList)
{
	RowList = rowList;
	DataSession = dataSession;
}
SHVDataRowList_SQLite::~SHVDataRowList_SQLite()
{
}

/*************************************
 * GetCurrentRow
 *************************************/
const SHVDataRowC* SHVDataRowList_SQLite::GetCurrentRow() const
{
	return RowList->GetCurrentRow();
}

/*************************************
 * IsOk
 *************************************/
SHVBool SHVDataRowList_SQLite::IsOk() const
{
	return RowList->IsOk();
}

/*************************************
 * GetStruct
 *************************************/
const SHVDataStructC* SHVDataRowList_SQLite::GetStruct() const
{
	return RowList->GetStruct();
}

/*************************************
 * GetRowCount
 *************************************/
int SHVDataRowList_SQLite::GetRowCount() const
{
	return RowList->GetRowCount();
}

/*************************************
 * Find
 *************************************/
SHVDataRowC* SHVDataRowList_SQLite::Find(const SHVDataRowKey* key)
{
	return RowList->Find(key);
}

/*************************************
 * NextRow
 *************************************/
SHVBool SHVDataRowList_SQLite::NextRow()
{
	return RowList->NextRow();
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
	return DataSession;
}

/*************************************
 * EditCurrentRow
 *************************************/
SHVDataRow* SHVDataRowList_SQLite::EditCurrentRow()
{	
	SHVASSERT(GetCurrentRow());
	if (GetCurrentRow())
		return NULL;

	return new SHVDataRow_impl(GetCurrentRow(), this);
}

/*************************************
 * AddRow
 *************************************/
SHVDataRow* SHVDataRowList_SQLite::AddRow()
{
	SHVASSERT(IsOk() == SHVBool::True);
	if (!IsOk())
		return NULL;

	return new SHVDataRow_impl(this);
}

