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
