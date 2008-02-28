// shivasqlitetester.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../../../include/utils/shvdll.h"
#include "../../../include/utils/shvstring.h"
#include "../../include/sqlitewrapper.h"
#include "../../include/sqlitestatement.h"

void DumpRow(SHVSQLiteStatement* statement)
{
	for (int i = 0; i < statement->GetColumnCount(); i++)
	{
		SHVString value;
		SHVString columnName;
		SHVString columnType;
		int len = 0;
		if (statement->GetColumnName(columnName, i))
		{
			statement->GetColumnType(columnType, i);
			if (statement->GetString(value, len, i))
			{

				_tprintf(_T("%s:%s = %s\r\n"), columnName.GetSafeBuffer(), columnType.GetSafeBuffer(), value.GetSafeBuffer());
			}
		}
	}
}
SHVBool DumpData(SHVSQLiteStatement* statement)
{
SHVBool errorCode;
	do 
	{
		DumpRow(statement);
	} while ((errorCode = statement->NextResult()).GetError() == SHVSQLiteWrapper::SQLite_ROW);
	return errorCode;
}



int _tmain(int argc, _TCHAR* argv[])
{
	// Lets start by create an instance of SQLLite
	SHVDll dll;

	if (dll.Load(dll.CreateLibFileName(_T("shivasqlite"))))
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
		if (sqlLite->Open(fileName) == SHVSQLiteWrapper::SQLite_OK)
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
					sql = _getts_s<255>(input);
				}
				if (sql != _T("exit") && sql != _T("cleanup"))
				{
				SHVBool errorCode;
				statement = sqlLite->Execute(errorCode, sql, reminder);
					if (errorCode.GetError() ==  SHVSQLiteWrapper::SQLite_ROW)
					{
						errorCode = DumpData(statement);
						if (errorCode.GetError() != SHVSQLiteWrapper::SQLite_DONE)
						{
							reminder = _T("");
							_tprintf(_T("Error %d: %s\r\n"), errorCode.GetError(), sqlLite->GetErrorMsg().GetSafeBuffer());
						}
					}
					if (errorCode.GetError() != SHVSQLiteWrapper::SQLite_DONE)
					{
						reminder = _T("");
						_tprintf(_T("Error %d: %s\r\n"), errorCode.GetError(), sqlLite->GetErrorMsg().GetSafeBuffer());
					}
				}
				if (sql == _T("cleanup"))
				{
					reminder = _T("");
					sqlLite = (SHVSQLiteWrapper*) dll.CreateObjectInt(NULL, SHVDll::ClassTypeUser);
					if (sqlLite->Open(fileName) != SHVSQLiteWrapper::SQLite_OK)
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

