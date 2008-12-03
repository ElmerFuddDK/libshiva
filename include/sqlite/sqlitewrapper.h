#ifndef __SHIVA_SQLITE_WRAPPER_H
#define __SHIVA_SQLITE_WRAPPER_H


#include "sqlitestatement.h"
#include "../utils/shvstring.h"
#include "../utils/shvstringutf8.h"
#include "../utils/shvrefobject.h"
#include "../threadutils/shvmutex.h"

#define __SHVSQLITEWRAPPER_CREATE_SYMBOL _T("CreateSHVSQLiteWrapper")


//-=========================================================================================================
///  SHVSQLiteWrapper class - The shiva C++ interface for SQLite
/**
 */
class SHVSQLiteWrapper: public SHVRefObject
{
public:
	enum SQLiteAffinity
	{
		Affinity_Int = 1,
		Affinity_Float = 2,
		Affinity_Text = 3,
		Affinity_Blob = 4,
		Affinity_Null = 5
	};
	enum SQLiteResult
	{
		SQLite_OK  = 0,			/* Successful result */
		SQLite_ERROR = 1,		/* SQL error or missing database */
		SQLite_INTERNAL = 2,	/* Internal logic error in SQLite */
		SQLite_PERM = 3,		/* Access permission denied */
		SQLite_ABORT = 4,		/* Callback routine requested an abort */
		SQLite_BUSY = 5,		/* The database file is locked */
		SQLite_LOCKED = 6,		/* A table in the database is locked */
		SQLite_NOMEM = 7,		/* A malloc() failed */
		SQLite_READONLY = 8,	/* Attempt to write a readonly database */
		SQLite_INTERRUPT = 9,   /* Operation terminated by sqlite3_interrupt()*/
		SQLite_IOERR = 10,		/* Some kind of disk I/O error occurred */
		SQLite_CORRUPT = 11,    /* The database disk image is malformed */
		SQLite_NOTFOUND = 12,   /* NOT USED. Table or record not found */
		SQLite_FULL = 13,       /* Insertion failed because database is full */
		SQLite_CANTOPEN = 14,   /* Unable to open the database file */
		SQLite_PROTOCOL = 15,   /* NOT USED. Database lock protocol error */
		SQLite_EMPTY = 16,      /* Database is empty */
		SQLite_SCHEMA = 17,     /* The database schema changed */
		SQLite_TOOBIG = 18,     /* String or BLOB exceeds size limit */
		SQLite_CONSTRAINT = 19, /* Abort due to constraint violation */
		SQLite_MISMATCH = 20,   /* Data type mismatch */
		SQLite_MISUSE = 21,     /* Library used incorrectly */
		SQLite_NOLFS = 22,      /* Uses OS features not supported on host */
		SQLite_AUTH = 23,       /* Authorization denied */
		SQLite_FORMAT = 24,     /* Auxiliary database format error */
		SQLite_RANGE = 25,      /* 2nd parameter to sqlite3_bind out of range */
		SQLite_NOTADB = 26,     /* File opened that is not a database file */
		SQLite_ROW = 100,       /* sqlite3_step() has another row ready */
		SQLite_DONE = 101		/* sqlite3_step() has finished executing */
	};
protected:
	virtual ~SHVSQLiteWrapper() {}
public:
	virtual SHVBool OpenUTF8(const SHVStringUTF8C& fileName, int option = 6) = 0;
	inline SHVBool Open(const SHVStringC& fileName, int option = 6);
	virtual SHVBool OpenInMemory() = 0;
	virtual SHVBool Close() = 0;
	virtual SHVSQLiteStatement* PrepareUTF8(SHVBool& ok, const SHVStringUTF8C& sql, SHVStringSQLite& notparsed) = 0;
	inline SHVSQLiteStatement* Prepare(SHVBool& ok, const SHVStringC& sql, SHVString& notparsed);
	virtual SHVSQLiteStatementRef ExecuteUTF8(SHVBool& ok, const SHVStringUTF8C& sql, SHVStringSQLite& notparsed) = 0;
	inline SHVSQLiteStatementRef Execute(SHVBool& ok, const SHVStringC& sql, SHVString& notparsed);
	virtual SHVStringUTF8C GetErrorMsgUTF8() = 0; 
	inline SHVStringBuffer GetErrorMsg();
};
typedef SHVRefObjectContainer<SHVSQLiteWrapper> SHVSQLiteWrapperRef;

// ============================================ implementation ============================================ //

/*************************************
 * Open
 *************************************/
SHVBool SHVSQLiteWrapper::Open(const SHVStringC& fileName, int option)
{
	return OpenUTF8(fileName.ToStrUTF8(), option);
}

/*************************************
 * Prepare
 *************************************/
SHVSQLiteStatement*  SHVSQLiteWrapper::Prepare(SHVBool& ok, const SHVStringC& sql, SHVString& notparsed)
{
SHVStringUTF8 sqlUTF8 = sql.ToStrUTF8();
SHVSQLiteStatement* res;
	SHVStringSQLite rest(NULL);
	res = PrepareUTF8(ok, sqlUTF8, rest);
	notparsed = rest.ToStrT();
	return res;
}

/*************************************
 * Execute
 *************************************/
SHVSQLiteStatementRef  SHVSQLiteWrapper::Execute(SHVBool& ok, const SHVStringC& sql, SHVString& notparsed)
{
SHVStringUTF8 sqlUTF8 = sql.ToStrUTF8();
SHVSQLiteStatementRef res;
	SHVStringSQLite rest(NULL);
	res = ExecuteUTF8(ok, sqlUTF8, rest);
	notparsed = rest.ToStrT();
	return res;
}

/*************************************
 * GetErrorMsg
 *************************************/
SHVStringBuffer SHVSQLiteWrapper::GetErrorMsg()
{
	return GetErrorMsgUTF8().ToStrT();
}

#endif
