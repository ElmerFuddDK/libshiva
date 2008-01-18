// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SQLITEWRAPPER_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SQLITEWRAPPER_API functions as being imported from a DLL, whereas this DLL sees symbols
// defined with this macro as being exported.
#ifndef __SQLITEWRAPPER_H
#define __SQLITEWRAPPER_H

#include "sqlitestatement.h"

// This class is exported from the SQLiteWrapper.dll
class RFTSQLiteWrapper {
public:
	enum SQLiteType
	{
		SQLite_Int = 1,
		SQLite_Float = 2,
		SQLite_Text = 3,
		SQLite_Blob = 4,
		SQLite_Null = 5
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
public:
	virtual ~RFTSQLiteWrapper() {}
	virtual short Open(const char* fileName, int option = 6) = 0;
	virtual short OpenInMemory() = 0;
	virtual short Close() = 0;
	virtual short Prepare(RFTSQLiteStatement*& statement, const char* sql, const char*& notparsed) = 0;
	virtual const char* GetErrorMsg() = 0; 
};

#define __RFTSQLITEWRAPPER_CREATE_SYMBOL _T("CreateRFTSQLiteWrapper")

#endif