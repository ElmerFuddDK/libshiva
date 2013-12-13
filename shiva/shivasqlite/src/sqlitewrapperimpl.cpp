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
#include "../../include/platformspc.h"

#include "../include/sqlitewrapperimpl.h"
#include "../include/sqlitestatementimpl.h"

/*************************************
 * Constructor
 *************************************/
SHVSQLiteWrapperImpl::SHVSQLiteWrapperImpl(void)
{
#ifdef DEBUG
	OwnerThread = SHVThreadBase::GetCurrentThreadID();
#endif
	Sqlite = NULL;
}

/*************************************
 * Destructor
 *************************************/
SHVSQLiteWrapperImpl::~SHVSQLiteWrapperImpl(void)
{
	if (Sqlite)
	{
		sqlite3_close(Sqlite);
		Sqlite = NULL;
	}
}

/*************************************
 * OpenUTF8
 *************************************/
SHVBool SHVSQLiteWrapperImpl::OpenUTF8(const SHVStringUTF8C& fileName, int option)
{
SHVBool retVal;
	SHVTHREADCHECK(OwnerThread);
	if (Sqlite)
	{
		sqlite3_close(Sqlite);
	}
	retVal = SHVBool(sqlite3_open_v2(fileName.GetSafeBuffer(), &Sqlite, option, NULL));
	if (Sqlite &&  retVal)
		sqlite3_busy_timeout(Sqlite, 10000);

	return retVal;
}

/*************************************
 * OpenInMemory
 *************************************/
SHVBool SHVSQLiteWrapperImpl::OpenInMemory()
{
	SHVTHREADCHECK(OwnerThread);
	if (Sqlite)
	{
		sqlite3_close(Sqlite);
	}
	return SHVBool(sqlite3_open(":memory", &Sqlite));
}

/*************************************
 * SetBusyTimeout
 *************************************/
SHVBool SHVSQLiteWrapperImpl::SetBusyTimeout(int ms)
{
	SHVTHREADCHECK(OwnerThread);
	if (Sqlite)
		sqlite3_busy_timeout(Sqlite, ms);
	return Sqlite != NULL;
}

/*************************************
 * Close
 *************************************/
SHVBool SHVSQLiteWrapperImpl::Close()
{
	SHVTHREADCHECK(OwnerThread);
	if (Sqlite)
	{
		short res = (short) sqlite3_close(Sqlite);
		Sqlite = NULL;
		return SHVBool(res);
	}
	else
		return SHVBool(SHVSQLiteWrapper::SQLite_ERROR);
}


/*************************************
 * GetRecentChanges
 *************************************/
int SHVSQLiteWrapperImpl::GetRecentChanges()
{
	if (Sqlite)
		return sqlite3_changes(Sqlite);
	else
		return -1;
}

/*************************************
 * GetTotalChanges
 *************************************/
int SHVSQLiteWrapperImpl::GetTotalChanges()
{
	if (Sqlite)
		return sqlite3_total_changes(Sqlite);
	else
		return -1;
}

/*************************************
 * PrepareUTF8
 *************************************/
SHVSQLiteStatement* SHVSQLiteWrapperImpl::PrepareUTF8(SHVBool& ok, const SHVStringUTF8C& sql, SHVStringSQLite& notparsed)
{
sqlite3_stmt* sqlite_statement;
int res = SHVSQLiteWrapper::SQLite_ERROR;;
const char* rest = NULL;
SHVSQLiteStatement* retVal = NULL;
	
	SHVTHREADCHECK(OwnerThread);
	if (Sqlite)
	{
		ok = SHVBool(sqlite3_prepare_v2(Sqlite, sql.GetSafeBuffer(), -1, &sqlite_statement, &rest));
		notparsed = rest;
		retVal = new SHVSQLiteStatementImpl(sqlite_statement, this, sqlite3_data_count(sqlite_statement));
	}
	else
		ok.SetError(SHVSQLiteWrapper::SQLite_ERROR);
	return retVal;
} 

/*************************************
 * ExecuteUTF8
 *************************************/
SHVSQLiteStatementRef SHVSQLiteWrapperImpl::ExecuteUTF8(SHVBool& ok, const SHVStringUTF8C& sql, SHVStringSQLite& notparsed)
{
SHVTHREADCHECK(OwnerThread);
sqlite3_stmt* sqlite_statement;
int res = SHVSQLiteWrapper::SQLite_ERROR;;
SHVSQLiteStatement* retVal = NULL;
const char* rest = NULL;
	
	SHVTHREADCHECK(OwnerThread);
	if (Sqlite)
	{
		ok = SHVBool(sqlite3_prepare_v2(Sqlite, sql.GetSafeBuffer(), -1, &sqlite_statement, &rest));
		retVal = new SHVSQLiteStatementImpl(sqlite_statement, this, sqlite3_data_count(sqlite_statement));
		if (ok)
			ok = SHVBool(retVal->NextResult().GetError());
		notparsed = rest;
	}
	else
		ok.SetError(SHVSQLiteWrapper::SQLite_ERROR);
	return retVal;
} 

/*************************************
 * GetErrorMsgUTF8
 *************************************/
SHVStringUTF8C SHVSQLiteWrapperImpl::GetErrorMsgUTF8()
{
	SHVTHREADCHECK(OwnerThread);
	if (Sqlite)
		return sqlite3_errmsg(Sqlite);
	else
		return "Database is not open";
}


