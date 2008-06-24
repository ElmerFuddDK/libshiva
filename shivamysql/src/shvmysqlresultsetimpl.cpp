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

#include "../../../include/platformspc.h"

#include "../include/shvmysqlresultsetimpl.h"
#include "../../../include/utils/shvstring16.h"

SHVMySQLResultSetImpl::SHVMySQLResultSetImpl(SHVMySQLConnection* connection, SHVStringUTF8 query): Connection(connection), Query(query)
{
	NumFields = -1;
	Result = NULL;
	Reset();
}
	
SHVMySQLResultSetImpl::~SHVMySQLResultSetImpl()
{
	if (Result)
		mysql_free_result(Result);
	Result = NULL;
}

SHVBool SHVMySQLResultSetImpl::GetLong(long& val, int columnIdx) const
{
SHVBool retVal = SHVBool::False;
	if (columnIdx < NumFields && CurrentRow && CurrentRow[columnIdx] && GetColumnType(columnIdx) == SHVMySQLResultSet::TypeLong)
	{
		val = SHVStringUTF8C::StrToL(CurrentRow[columnIdx], NULL);
		retVal = SHVBool::True;
	}
	return retVal;
}

SHVBool SHVMySQLResultSetImpl::GetInt64(SHVInt64Val& val, int columnIdx) const
{
SHVBool retVal = SHVBool::False;
	if (columnIdx < NumFields && CurrentRow && CurrentRow[columnIdx] && (GetColumnType(columnIdx) == SHVMySQLResultSet::TypeInt64 ||
		   GetColumnType(columnIdx) == SHVMySQLResultSet::TypeLong))
	{
		val = SHVStringUTF8C::StrToInt64(CurrentRow[columnIdx], NULL);
		retVal = SHVBool::True;
	}
	return retVal;
}

SHVBool SHVMySQLResultSetImpl::GetDouble(double& val, int columnIdx) const
{
	SHVBool retVal = SHVBool::False;
	if (columnIdx < NumFields && CurrentRow && CurrentRow[columnIdx] && (
		GetColumnType(columnIdx) == SHVMySQLResultSet::TypeDouble ||
		GetColumnType(columnIdx) == SHVMySQLResultSet::TypeInt64 ||
		GetColumnType(columnIdx) == SHVMySQLResultSet::TypeLong)
	   )
	{
		val = SHVStringUTF8C::StrToDouble(CurrentRow[columnIdx], NULL);
		retVal = SHVBool::True;
	}
	return retVal;
}

SHVBool SHVMySQLResultSetImpl::GetStringUTF8(SHVStringUTF8& val, int columnIdx) const
{
SHVBool retVal = SHVBool::False;

	if (columnIdx < NumFields && CurrentRow && CurrentRow[columnIdx])
	{
		val = CurrentRow[columnIdx];
		retVal = SHVBool::True;
	}
	return retVal;
}

SHVBool SHVMySQLResultSetImpl::GetDateTime(SHVTime& val, int columnIdx) const
{
SHVBool retVal = SHVBool::False;

	if (columnIdx < NumFields && CurrentRow && CurrentRow[columnIdx])
	{
		val.SetFromDateString(CurrentRow[columnIdx]);
		retVal = SHVBool::True;
	}
	return retVal;
}

SHVBool SHVMySQLResultSetImpl::GetBool(SHVBool& val, int columnIdx) const
{
SHVBool retVal = SHVBool::False;

	if (columnIdx < NumFields && CurrentRow && CurrentRow[columnIdx])
	{
		val = SHVStringUTF8C(CurrentRow[columnIdx]) != "0";
		retVal = SHVBool::True;
	}
	return retVal;
}

const char* SHVMySQLResultSetImpl::GetRawData(int columnIdx) const
{
const char* retVal = NULL;
	if (columnIdx < NumFields && CurrentRow && CurrentRow[columnIdx])
		retVal = CurrentRow[columnIdx];
	return retVal;
}

SHVBool SHVMySQLResultSetImpl::GetColumnNameUTF8(SHVStringUTF8& name, int columnIdx) const
{
SHVBool retVal = SHVBool::False;

	if (columnIdx < NumFields && Result)
	{
		name = mysql_fetch_field_direct(Result, columnIdx)->name;
		retVal = SHVBool::True;
	}
	return retVal;
}

SHVBool SHVMySQLResultSetImpl::GetColumnName8(SHVString8& name, int columnIdx) const
{
SHVStringUTF8 utf8name;
SHVBool retVal = GetColumnNameUTF8(utf8name, columnIdx);
	if (retVal)
		name = utf8name.ToStr8();
	return retVal;
}

SHVBool SHVMySQLResultSetImpl::GetColumnName16(SHVString16& name, int columnIdx) const
{
SHVStringUTF8 utf8name;
SHVBool retVal = GetColumnNameUTF8(utf8name, columnIdx);
	if (retVal)
		name = utf8name.ToStr16();
	return retVal;
}

int SHVMySQLResultSetImpl::GetColumnCount() const
{
	return NumFields;
}

SHVMySQLResultSet::Types SHVMySQLResultSetImpl::GetColumnType(int columnIdx) const
{
SHVMySQLResultSet::Types retVal = SHVMySQLResultSet::TypeRaw;

	if (columnIdx < NumFields && Result)
	{
	int type = mysql_fetch_field_direct(Result, columnIdx)->type;
		switch (type)
		{
			case MYSQL_TYPE_TINY:
			case MYSQL_TYPE_SHORT:
   			case MYSQL_TYPE_LONG:
				retVal = SHVMySQLResultSet::TypeLong;
				break;
			case MYSQL_TYPE_NEWDECIMAL:
			case MYSQL_TYPE_DECIMAL:
			case MYSQL_TYPE_FLOAT:
			case MYSQL_TYPE_DOUBLE:
				retVal = SHVMySQLResultSet::TypeDouble;
				break;
			case MYSQL_TYPE_TIMESTAMP:
			case MYSQL_TYPE_DATE:
			case MYSQL_TYPE_TIME:
			case MYSQL_TYPE_DATETIME: 
			case MYSQL_TYPE_YEAR:
			case MYSQL_TYPE_NEWDATE:
				retVal = SHVMySQLResultSet::TypeDateTime;
				break;
 			case MYSQL_TYPE_LONGLONG:
			case MYSQL_TYPE_INT24:
				retVal = SHVMySQLResultSet::TypeInt64;
				break;
			case MYSQL_TYPE_VARCHAR:
			case MYSQL_TYPE_VAR_STRING:
			case MYSQL_TYPE_STRING:
				retVal = SHVMySQLResultSet::TypeString;
				break;			
			case MYSQL_TYPE_ENUM:
			case MYSQL_TYPE_SET:
			case MYSQL_TYPE_TINY_BLOB:
			case MYSQL_TYPE_MEDIUM_BLOB:
			case MYSQL_TYPE_LONG_BLOB:
			case MYSQL_TYPE_BLOB:
			case MYSQL_TYPE_GEOMETRY:
				retVal = SHVMySQLResultSet::TypeRaw;
				break;
			case MYSQL_TYPE_BIT:
				retVal = SHVMySQLResultSet::TypeBool;
				break;
		}
		
	}
	return retVal;
}
	
int SHVMySQLResultSetImpl::GetNativeType(int columnIdx) const
{
int retVal = -1;
	if (columnIdx < NumFields && Result)
	{
		retVal = mysql_fetch_field_direct(Result, columnIdx)->type;
	}
	return retVal;
}

int SHVMySQLResultSetImpl::GetColumnLen(int columnIdx) const
{
int retVal = -1;
	if (columnIdx < NumFields && Result)
	{
		retVal = mysql_fetch_field_direct(Result, columnIdx)->length;
	}
	return retVal;
}

SHVBool SHVMySQLResultSetImpl::NextRow()
{
	return Result && (CurrentRow = mysql_fetch_row(Result)) != NULL;
}

void SHVMySQLResultSetImpl::Reset()
{
	Ok = PerformQuery();
}

SHVBool SHVMySQLResultSetImpl::IsOk() const
{
	return Ok;	
}

SHVMySQLConnection* SHVMySQLResultSetImpl::GetConnection()
{
	return Connection;
}
	
SHVBool SHVMySQLResultSetImpl::PerformQuery()
{
SHVBool retVal;
	if (Result)
	{
		mysql_free_result(Result);
		Result = NULL;		
	}
	retVal.SetError(mysql_query(GetMYSQL(), Query.GetSafeBuffer()));
	if (retVal)
	{ 
		Result = mysql_use_result(GetMYSQL());
		NumFields = mysql_field_count(GetMYSQL());
	}
	else
		Result = NULL;
	return retVal;
}

