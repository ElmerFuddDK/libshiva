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

SHVMySQLConnectionImpl::SHVMySQLConnectionImpl()
{
}
 
SHVMySQLConnectionImpl::~SHVMySQLConnectionImpl()
{
	if (Connection)
	{
		mysql_close(Connection);
		Connection = NULL;
	}		
}

const SHVMySQLConnectionParms* SHVMySQLConnectionImpl::GetParms() const
{
	return Parms.AsConst();
}
 
SHVBool SHVMySQLConnectionImpl::Connect(const SHVMySQLConnectionParms* parms)
{
	Connection = mysql_init(NULL);
	Parms = (SHVMySQLConnectionParms*) parms;
	if(!mysql_real_connect(Connection, 
		 					parms->GetHost().GetBufferConst(), 
							parms->GetUser().GetBufferConst(),
							parms->GetPasswd().GetBufferConst(),
							parms->GetDb().GetBufferConst(),
							parms->GetPort(),
							parms->GetUnix_Socket().GetBufferConst(),
							parms->GetClient_Flag()))
	{
		mysql_close(Connection);
		Connection = NULL;
	}
	return Connection != NULL;
}

SHVBool SHVMySQLConnectionImpl::IsOk()
{	
	return Connection != NULL;
}

SHVMySQLResultSet* SHVMySQLConnectionImpl::Query(const SHVStringUTF8 query)
{
	if (IsOk())
		return new SHVMySQLResultSetImpl(this, query);
	else
		return NULL;
}

SHVBool SHVMySQLConnectionImpl::Execute(const SHVStringUTF8 sql)
{
SHVBool retVal;
	if (IsOk())
		retVal.SetError(mysql_query(Connection, sql.GetSafeBuffer()));
	else
		retVal = SHVBool::False;
	return retVal;
}

void* SHVMySQLConnectionImpl::GetProvider()
{
	return Connection;
}

SHVStringBuffer SHVMySQLConnectionImpl::GetError()
{
	if (Connection)
		return SHVStringUTF8(mysql_error(Connection)).ToStrT();
	else
		return SHVString(_T("Not connected")).ReleaseBuffer();
}
