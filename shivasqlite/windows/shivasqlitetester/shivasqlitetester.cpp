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

// shivasqlitetester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../../../include/utils/shvdll.h"
#include "../../../include/utils/shvstring.h"
#include "../../../include/sqlite/sqlitewrapper.h"
#include "../../../include/sqlite/sqlitestatement.h"

void DumpRow(SHVSQLiteStatement* statement)
{
	for (int i = 0; i < statement->GetColumnCount(); i++)
	{
		SHVString value;
		SHVString columnName;
		SHVString columnType;
		short aff;
		int len = 0;
		if (statement->GetColumnName(columnName, i))
		{
			statement->GetColumnType(columnType, i);
			if (statement->GetString(value, len, i))
			{
				if (columnType.Left(7) == _S("varchar"))
					_tprintf(_T("%-20s "), value.GetSafeBuffer());
				else
				{
					if (statement->GetColumnAffinity(aff, i))
					{
						if (aff == SHVSQLiteWrapper::Affinity_Int)
							_tprintf(_T("%10s "), value.GetSafeBuffer());
						else
							_tprintf(_T("%20s "), value.GetSafeBuffer());
					}
				}
			}
		}
	}
	_tprintf(_T("\r\n"));
}
SHVBool DumpData(SHVSQLiteStatement* statement)
{
SHVBool errorCode;
SHVString columnName;
SHVString columnType;
short aff;
	for (int i = 0; i < statement->GetColumnCount(); i++)
	{
		if (statement->GetColumnName(columnName, i))
		{
			statement->GetColumnType(columnType, i);
			if (columnType.Left(7) == _S("varchar"))
				_tprintf(_T("%-20s "), columnName.GetSafeBuffer());
			else
			if (statement->GetColumnAffinity(aff, i))
			{
				if (aff == SHVSQLiteWrapper::Affinity_Int)
					_tprintf(_T("%10s "), columnName.GetSafeBuffer());
				else
					_tprintf(_T("%20s "), columnName.GetSafeBuffer());
			}
		}
	}
	_tprintf(_T("\r\n=======================================================================\r\n"));
	do 
	{
		DumpRow(statement);
	} while ((errorCode = statement->NextResult()).GetError() == SHVSQLiteWrapper::SQLite_ROW);
	return errorCode;
}

void TerrorTest(SHVDll& dll, const SHVStringC& fileName)
{
SHVStringSQLite rest("");
SHVBool ok;
SHVSQLiteWrapperRef sqlLite1 = (SHVSQLiteWrapper*) dll.CreateObjectInt(NULL, SHVDll::ClassTypeUser);
SHVSQLiteWrapperRef sqlLite2 = (SHVSQLiteWrapper*) dll.CreateObjectInt(NULL, SHVDll::ClassTypeUser);
SHVSQLiteStatementRef statement1;
SHVSQLiteStatementRef statement2;

	sqlLite1->Open(fileName);
	sqlLite2->Open(fileName);

	statement1 = sqlLite1->ExecuteUTF8(ok, "drop table ctest", rest);
	statement1 = sqlLite1->ExecuteUTF8(ok, "create table ctest(key integer primary key, val varchar(30))", rest);
	statement1 = sqlLite1->ExecuteUTF8(ok, "begin transaction", rest);
	statement1 = sqlLite1->ExecuteUTF8(ok, "insert into ctest values(1, 'a')", rest);
	statement1 = sqlLite1->ExecuteUTF8(ok, "insert into ctest values(2, 'b')", rest);
	statement1 = sqlLite1->ExecuteUTF8(ok, "insert into ctest values(3, 'c')", rest);
	statement1 = sqlLite1->ExecuteUTF8(ok, "insert into ctest values(4, 'd')", rest);
	statement1 = sqlLite1->ExecuteUTF8(ok, "insert into ctest values(5, 'e')", rest);
	statement1 = sqlLite1->ExecuteUTF8(ok, "insert into ctest values(6, 'f')", rest);
	statement1 = sqlLite1->ExecuteUTF8(ok, "insert into ctest values(7, 'g')", rest);
	statement1 = sqlLite1->ExecuteUTF8(ok, "insert into ctest values(8, 'h')", rest);
	statement1 = sqlLite1->ExecuteUTF8(ok, "insert into ctest values(9, 'i')", rest);
	statement1 = sqlLite1->ExecuteUTF8(ok, "insert into ctest values(10, 'j')", rest);
	statement1 = sqlLite1->ExecuteUTF8(ok, "end transaction", rest);
	statement1 = sqlLite1->ExecuteUTF8(ok, "update ctest set val = 'dette' where key = 3", rest);

	statement2 = sqlLite2->ExecuteUTF8(ok, "select * from ctest where (@key is null or key = @key)", rest);
	statement2->Reset();
	statement2->SetParameterLongUTF8("@key", 5);
	DumpData(statement2);
	statement2->Reset();
	statement2->SetParameterNullUTF8("@key");
	DumpData(statement2);
}

int _tmain(int argc, _TCHAR* argv[])
{
	// Lets start by create an instance of SQLLite
	SHVDll dll;

	if (dll.Load(dll.CreateLibFileName(_S("shivasqlite"))))
	{
		SHVStringUTF8 error;
		SHVSQLiteWrapperRef sqlLite = (SHVSQLiteWrapper*) dll.CreateObjectInt(NULL, SHVDll::ClassTypeUser);
		SHVSQLiteStatementRef statement;
		TCHAR input[255];
		SHVString reminder;
		SHVString sql;
		TCHAR* fileName = _T("test.db");
		if (argc > 1)
			fileName = argv[1];

		puts("SQLite tester");
		if (sqlLite->Open((const SHVTChar*)fileName) == SHVSQLiteWrapper::SQLite_OK)
		{
			do
			{
				if (reminder.GetLength())
				{
					_putts(_T("------------------>"));
					sql = reminder;
				}
				else
				{
					_tprintf(_T("=>"));
					sql = (SHVTChar*)_getts_s<255>(input);
				}
				if (sql != _S("exit") && sql != _S("cleanup"))
				{
				SHVBool errorCode;
				statement = sqlLite->Execute(errorCode, sql, reminder);
					if (errorCode.GetError() ==  SHVSQLiteWrapper::SQLite_ROW)
					{
						errorCode = DumpData(statement);
						if (errorCode.GetError() != SHVSQLiteWrapper::SQLite_DONE)
						{
							reminder = _S("");
							_tprintf(_T("Error %d: %s\r\n"), errorCode.GetError(), sqlLite->GetErrorMsg().GetSafeBuffer());
						}
					}
					if (errorCode.GetError() != SHVSQLiteWrapper::SQLite_DONE)
					{
						reminder = _S("");
						_tprintf(_T("Error %d: %s\r\n"), errorCode.GetError(), sqlLite->GetErrorMsg().GetSafeBuffer());
					}
				}
				if (sql == _S("cleanup"))
				{
					reminder = _S("");
					sqlLite = (SHVSQLiteWrapper*) dll.CreateObjectInt(NULL, SHVDll::ClassTypeUser);
					if (sqlLite->Open((const SHVTChar*)fileName) != SHVSQLiteWrapper::SQLite_OK)
					{
						_tprintf(_T("Could not open %s\r\n"), fileName);
						_tcscpy_s(input, 255, _T("exit"));
					}
				}
			} while (_tcscmp(input, _T("exit")));
			sqlLite = NULL;
			statement = NULL;
		}
		_putts(_T("Done"));
	}
	else
		_putts(_T("Could not load sqlite"));
#ifdef DEBUG	
	_CrtDumpMemoryLeaks();
#endif
	return 0;
}

