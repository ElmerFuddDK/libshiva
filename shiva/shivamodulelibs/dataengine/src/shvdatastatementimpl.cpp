#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../include/shvdatastatementimpl.h"
#include "../../../include/sqlite/sqlitewrapper.h"


/*************************************
 * Constructor
 *************************************/
SHVDataStatementImpl::SHVDataStatementImpl(SHVSQLiteWrapper* sqlite, const SHVStringUTF8C query)
{
SHVStringSQLite unparsed("");
SHVSQLiteStatementRef statement = sqlite->PrepareUTF8(Ok,query,unparsed);

	if (Ok && !statement.IsNull())
	{
	SHVStringUTF8 oldQuery, query;
		Statements.Add(statement);
		
		query = unparsed;
		while (Ok && !query.IsEmpty())
		{
			statement = sqlite->PrepareUTF8(Ok,query,unparsed);
			
			if (Ok)
			{
				Statements.Add(statement);
			}
			// Ensures the buffer doesn't get deleted before query is refilled - unparsed points into query
			oldQuery = query.ReleaseBuffer();
			query = unparsed;
			oldQuery = NULL;
		}
	}
	
	if (!Ok)
		Statements.Clear();
	
	CurrentStatement = SIZE_T_MAX;
}

/*************************************
 * IsOk
 *************************************/
SHVBool SHVDataStatementImpl::IsOk() const
{
	return Ok;
}

/*************************************
 * GetLong
 *************************************/
SHVBool SHVDataStatementImpl::GetLong(long& val, int columnIdx) const
{
SHVSQLiteStatement* statement = GetCurrentStatement();

	if (statement)
		return statement->GetLong(val,columnIdx);
	
	return SHVBool::False;
}

/*************************************
 * GetInt64
 *************************************/
SHVBool SHVDataStatementImpl::GetInt64(SHVInt64Val& val, int columnIdx) const
{
SHVSQLiteStatement* statement = GetCurrentStatement();

	if (statement)
		return statement->GetInt64(val,columnIdx);
	
	return SHVBool::False;
}

/*************************************
 * GetDouble
 *************************************/
SHVBool SHVDataStatementImpl::GetDouble(double& val, int columnIdx) const
{
SHVSQLiteStatement* statement = GetCurrentStatement();

	if (statement)
		return statement->GetDouble(val,columnIdx);
	
	return SHVBool::False;
}

/*************************************
 * GetStringUTF8
 *************************************/
SHVBool SHVDataStatementImpl::GetStringUTF8(SHVStringUTF8C& text, int& len, int columnIdx) const
{
SHVSQLiteStatement* statement = GetCurrentStatement();

	if (statement)
	{
	SHVBool retVal;
	SHVStringSQLite str(NULL);
		retVal = statement->GetStringUTF8(str,len,columnIdx);
		text = str;
		return retVal;
	}
	
	return SHVBool::False;
}

/*************************************
 * GetColumnName*
 *************************************/
SHVBool SHVDataStatementImpl::GetColumnNameUTF8(SHVStringUTF8C& name, int columnIdx) const
{
SHVSQLiteStatement* statement = GetCurrentStatement();

	if (statement)
	{
	SHVBool retVal;
	SHVStringSQLite str(NULL);
		retVal = statement->GetColumnNameUTF8(str,columnIdx);
		name = str;
		return retVal;
	}
	
	return SHVBool::False;
}
SHVBool SHVDataStatementImpl::GetColumnName8(SHVString8& name, int columnIdx) const
{
SHVSQLiteStatement* statement = GetCurrentStatement();

	if (statement)
		return statement->GetColumnName8(name,columnIdx);
	
	return SHVBool::False;
}

SHVBool SHVDataStatementImpl::GetColumnName16(SHVString16& name, int columnIdx) const
{
SHVSQLiteStatement* statement = GetCurrentStatement();

	if (statement)
		return statement->GetColumnName16(name,columnIdx);
	
	return SHVBool::False;
}

/*************************************
 * GetColumnAffinity
 *************************************/
SHVBool SHVDataStatementImpl::GetColumnAffinity(short& type, int columnIdx) const
{
SHVSQLiteStatement* statement = GetCurrentStatement();

	if (statement)
		return statement->GetColumnAffinity(type,columnIdx);
	
	return SHVBool::False;
}

/*************************************
 * GetColumnType*
 *************************************/
SHVBool SHVDataStatementImpl::GetColumnTypeUTF8(SHVStringUTF8C& colType, int columnIdx) const
{
SHVSQLiteStatement* statement = GetCurrentStatement();

	if (statement)
	{
	SHVBool retVal;
	SHVStringSQLite str(NULL);
		retVal = statement->GetColumnTypeUTF8(str,columnIdx);
		colType = str;
		return retVal;
	}
	
	return SHVBool::False;
}
SHVBool SHVDataStatementImpl::GetColumnType8(SHVString8& colType, int columnIdx) const
{
SHVSQLiteStatement* statement = GetCurrentStatement();

	if (statement)
		return statement->GetColumnType8(colType,columnIdx);
	
	return SHVBool::False;
}
SHVBool SHVDataStatementImpl::GetColumnType16(SHVString16& colType, int columnIdx) const
{
SHVSQLiteStatement* statement = GetCurrentStatement();

	if (statement)
		return statement->GetColumnType16(colType,columnIdx);
	
	return SHVBool::False;
}

/*************************************
 * GetcolumnCount
 *************************************/
int SHVDataStatementImpl::GetColumnCount() const
{
SHVSQLiteStatement* statement = GetCurrentStatement();

	if (statement)
		return statement->GetColumnCount();
	
	return 0;
}

/*************************************
 * ClearAllParameters
 *************************************/
void SHVDataStatementImpl::ClearAllParameters()
{
size_t len = Statements.CalculateCount();

	for (size_t i=0; i<len; i++)
		Statements[i]->ClearAllParameters();
}

/*************************************
 * SetParameterLongUTF8
 *************************************/
SHVBool SHVDataStatementImpl::SetParameterLongUTF8(const SHVStringUTF8C& name, long val)
{
size_t len = Statements.CalculateCount();
SHVBool retVal(SHVBool::True);
SHVBool ok;
bool set = false;

	for (size_t i=0; i<len; i++)
	{
		ok = Statements[i]->SetParameterLongUTF8(name,val);
		if (ok)
			set = true;
		if (retVal && !ok && ok.GetError() != SHVSQLiteWrapper::SQLite_ERROR)
			retVal = ok;
	}
	
	if (retVal && !set)
		retVal.SetError(SHVSQLiteWrapper::SQLite_ERROR);
	
	return retVal;
}

/*************************************
 * SetParameterInt64UTF8
 *************************************/
SHVBool SHVDataStatementImpl::SetParameterInt64UTF8(const SHVStringUTF8C& name, SHVInt64Val val)
{
size_t len = Statements.CalculateCount();
SHVBool retVal(SHVBool::True);
SHVBool ok;
bool set = false;

	for (size_t i=0; i<len; i++)
	{
		ok = Statements[i]->SetParameterInt64UTF8(name,val);
		if (ok)
			set = true;
		if (retVal && !ok && ok.GetError() != SHVSQLiteWrapper::SQLite_ERROR)
			retVal = ok;
	}
	
	if (retVal && !set)
		retVal.SetError(SHVSQLiteWrapper::SQLite_ERROR);
	
	return retVal;
}

/*************************************
 * SetParameterDoubleUTF8
 *************************************/
SHVBool SHVDataStatementImpl::SetParameterDoubleUTF8(const SHVStringUTF8C& name, double val)
{
size_t len = Statements.CalculateCount();
SHVBool retVal(SHVBool::True);
SHVBool ok;
bool set = false;

	for (size_t i=0; i<len; i++)
	{
		ok = Statements[i]->SetParameterDoubleUTF8(name,val);
		if (ok)
			set = true;
		if (retVal && !ok && ok.GetError() != SHVSQLiteWrapper::SQLite_ERROR)
			retVal = ok;
	}
	
	if (retVal && !set)
		retVal.SetError(SHVSQLiteWrapper::SQLite_ERROR);
	
	return retVal;
}

/*************************************
 * SetParameterStringUTF8
 *************************************/
SHVBool SHVDataStatementImpl::SetParameterStringUTF8(const SHVStringUTF8C& name, const SHVStringUTF8C& val)
{
size_t len = Statements.CalculateCount();
SHVBool retVal(SHVBool::True);
SHVBool ok;
bool set = false;

	for (size_t i=0; i<len; i++)
	{
		ok = Statements[i]->SetParameterStringUTF8(name,val);
		if (ok)
			set = true;
		if (retVal && !ok && ok.GetError() != SHVSQLiteWrapper::SQLite_ERROR)
			retVal = ok;
	}
	
	if (retVal && !set)
		retVal.SetError(SHVSQLiteWrapper::SQLite_ERROR);
	
	return retVal;
}

/*************************************
 * SetParameterNullUTF8
 *************************************/
SHVBool SHVDataStatementImpl::SetParameterNullUTF8(const SHVStringUTF8C& name)
{
size_t len = Statements.CalculateCount();
SHVBool retVal(SHVBool::True);
SHVBool ok;
bool set = false;

	for (size_t i=0; i<len; i++)
	{
		ok = Statements[i]->SetParameterNullUTF8(name);
		if (ok)
			set = true;
		if (retVal && !ok && ok.GetError() != SHVSQLiteWrapper::SQLite_ERROR)
			retVal = ok;
	}
	
	if (retVal && !set)
		retVal.SetError(SHVSQLiteWrapper::SQLite_ERROR);
	
	return retVal;
}

/*************************************
 * NextResult
 *************************************/
SHVBool SHVDataStatementImpl::NextResult()
{
size_t len = Statements.CalculateCount();

	if (CurrentStatement >= len)
		CurrentStatement = 0;
	else
		CurrentStatement++;
	
	return (CurrentStatement < len);
}

/*************************************
 * NextRow
 *************************************/
SHVBool SHVDataStatementImpl::NextRow()
{
SHVSQLiteStatement* statement = GetCurrentStatement();

	if (!statement)
	{
		NextResult();
		statement = GetCurrentStatement();
	}

	if (statement)
	{
	SHVBool retVal = statement->NextResult();
	
		if (retVal.GetError() == SHVSQLiteWrapper::SQLite_ROW)
			retVal = SHVBool::True;
		
		return retVal;
	}
	
	return SHVBool::False;
}

/*************************************
 * Reset
 *************************************/
SHVBool SHVDataStatementImpl::Reset()
{
size_t len = Statements.CalculateCount();
SHVBool retVal(SHVBool::True);
SHVBool ok;

	CurrentStatement = len;

	for (size_t i=0; i<len; i++)
	{
		ok = Statements[i]->Reset();
		if (!retVal && !ok)
			retVal = ok;
	}
	
	return retVal;
}

/*************************************
 * GetCurrentStatement
 *************************************/
SHVSQLiteStatement* SHVDataStatementImpl::GetCurrentStatement() const
{
	return ( CurrentStatement < Statements.CalculateCount() ? (SHVSQLiteStatement*)Statements[CurrentStatement] : NULL );
}
