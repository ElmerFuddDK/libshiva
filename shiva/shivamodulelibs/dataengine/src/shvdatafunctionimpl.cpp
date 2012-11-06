#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../include/shvdatafunctionimpl.h"


/*************************************
 * Constructor
 *************************************/
SHVDataFunctionImpl::SHVDataFunctionImpl(SHVDataRowListCSQLite* rowList): RowList(rowList)
{
}

SHVDataFunctionImpl::SHVDataFunctionImpl(SHVDataSession* session, const SHVStringUTF8C &query, const SHVDataRowKey *sortKey)
{
	RowList = new SHVDataRowListCSQLite(session, query, sortKey);
}

/*************************************
 * SetParameterLongUTF8
 *************************************/
SHVBool SHVDataFunctionImpl::SetParameterLongUTF8(const SHVStringUTF8C &name, long val)
{
char pname[128] = "@";
	strcpy(pname+1,name.GetBufferConst());
	if (GetInUse() || RowList.IsNull())
		return SetError(SHVBool(SHVSQLiteWrapper::SQLite_BUSY));
	return SetError(RowList->Statement->SetParameterLongUTF8(pname, val));
}

/*************************************
 * SetParameterInt64UTF8
 *************************************/
SHVBool SHVDataFunctionImpl::SetParameterInt64UTF8(const SHVStringUTF8C &name, SHVInt64Val val)
{
char pname[128] = "@";
	strcpy(pname+1,name.GetBufferConst());
	if (GetInUse() || RowList.IsNull())
		return SetError(SHVBool(SHVSQLiteWrapper::SQLite_BUSY));
	return SetError(RowList->Statement->SetParameterInt64UTF8(pname, val));
}

/*************************************
 * SetParameterDoubleUTF8
 *************************************/
SHVBool SHVDataFunctionImpl::SetParameterDoubleUTF8(const SHVStringUTF8C &name, double val)
{
char pname[128] = "@";
	strcpy(pname+1,name.GetBufferConst());
	if (GetInUse() || RowList.IsNull())
		return SetError(SHVBool(SHVSQLiteWrapper::SQLite_BUSY));
	return SetError(RowList->Statement->SetParameterDoubleUTF8(pname, val));
}

/*************************************
 * SetParameterStringUTF8
 *************************************/
SHVBool SHVDataFunctionImpl::SetParameterStringUTF8(const SHVStringUTF8C &name, const SHVStringUTF8C &val)
{
char pname[128] = "@";
	strcpy(pname+1,name.GetBufferConst());
	if (GetInUse() || RowList.IsNull())
		return SetError(SHVBool(SHVSQLiteWrapper::SQLite_BUSY));
	return SetError(RowList->Statement->SetParameterStringUTF8(pname, val));
}

/*************************************
 * SetParameterNullUTF8
 *************************************/
SHVBool SHVDataFunctionImpl::SetParameterNullUTF8(const SHVStringUTF8C &name)
{
char pname[128] = "@";
	strcpy(pname+1,name.GetBufferConst());
	if (GetInUse() || RowList.IsNull())
		return SetError(SHVBool(SHVSQLiteWrapper::SQLite_BUSY));
	return SetError(RowList->Statement->SetParameterNullUTF8(pname));
}

/*************************************
 * Exec
 *************************************/
SHVDataRowListC* SHVDataFunctionImpl::Exec()
{
	if (GetInUse())
	{
		SetError(SHVBool(SHVSQLiteWrapper::SQLite_BUSY));
		return NULL;
	}
	LastError = RowList->NextRow();
	if (!LastError)
		return RowList;
	else
		return NULL;
}

/*************************************
 * GetLastError
 *************************************/
SHVBool SHVDataFunctionImpl::GetLastError()
{
	return LastError;
}


/*************************************
 * GetInUse
 *************************************/
bool SHVDataFunctionImpl::GetInUse()
{
	return RowList->HasShareLock;
}

