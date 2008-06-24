//
// C++ Implementation: main
//
// Description: 
//
//
// Author: Mogens Bak Nielsen <mbn@mbn-laptop>, (C) 2008
//
// Copyright: See COPYING file that comes with this distribution
//
//
#include "../../../include/platformspc.h"
#include "../../../include/framework/shvmodule.h"
#include "../../../include/frameworkimpl/shvmainthreadeventdispatchergeneric.h"
#include "../../../include/shvversion.h"
#include "../../../include/utils/shvdll.h"
#include "../../../include/mysql/shvmysqlresultset.h"

int main(int argc, char *argv[])
{
SHVDll dll;
SHVMySQLConnectionParmsRef Parms = new SHVMySQLConnectionParms("192.168.1.42", "cowdetect", "KoKasse", "CowDetect", 0);
SHVMySQLConnectionRef Connection;
SHVMySQLResultSetRef Result;
	printf(_T("%s\n"), dll.CreateLibFileName(_T("shivamysql")).GetSafeBuffer());
	if (!dll.Load(dll.CreateLibFileName(_T("shivamysql"))))
	{
		printf("Could not load\n");
	}
	else 
		printf("Success!!!!!!\n");	
	Connection = (SHVMySQLConnection*) dll.CreateObjectInt(NULL, SHVDll::ClassTypeUser);
	if (Connection->Connect(Parms))
	{
		printf("Hooray\r\n");
		Result = Connection->Query(SHVStringUTF8("select * from coordinates"));
		if (!Result->IsOk())
			printf("Error %s\n", Connection->GetError().GetSafeBuffer());
		while (Result->NextRow())
		{
		SHVStringUTF8 val;
		SHVStringUTF8 col;
			for (int i = 0; i < Result->GetColumnCount(); i++)
			{
				Result->GetColumnNameUTF8(col, i);
				Result->GetStringUTF8(val, i);
				printf("%s=%s\n", col.GetSafeBuffer(), val.GetSafeBuffer());
			}
			printf("=================");			
		}
	}
	else
		printf("Error %s", Connection->GetError().GetSafeBuffer());
}
