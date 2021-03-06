#ifndef __SHIVA_UTILS_FILEBASE_H
#define __SHIVA_UTILS_FILEBASE_H

#include "shvstring.h"
#include <stdio.h>


typedef long SHVFilePos;

//-=========================================================================================================
/// SHVFileBase class - basic file operations
/**
 * Contains basic os specific file operations. This class only supports 2GB files - sorry.
 \bug Since it is based POSIX it will fail the memleak test on symbian - you'll need to do
      the following: Call CCoeEnv::Static()->DisableExitChecks(true); on startup and CloseSTDLIB(); on close.
      see: http://www3.symbian.com/faq.nsf/0/055490199FDB4D1580256A570051B8C8?OpenDocument
 */

class SHVAPI SHVFileBase
{
public:

	// defines/constants
	enum {
		FlagOpen = 1,		///< Open an existing file
		FlagCreate = 2,		///< Create if doesn't exist
		FlagRead = 4,		///< Read mode
		FlagWrite = 8,		///< Write mode ~ if set, position will be at the end
		FlagOverride = 16	///< Truncate if exists
	};
	enum Errors {
		ErrGeneric = SHVBool::False,
		ErrMissingFile = 1,
		ErrExists = 2,
		ErrInvalidParams = 3
	};
	enum {
		InvalidPosition = -1
	};
	static const SHVStringC Wildcard();
	static const SHVString8C LineSeparator8();
	static const SHVStringUTF8C LineSeparatorUTF8();
	static const SHVString16C LineSeparator16();
	inline static const SHVStringC LineSeparator();


	// constructor
	SHVFileBase();
	~SHVFileBase();


	// properties
	bool IsOpen();
	inline int GetFlags();
	SHVFilePos GetSize();
	SHVStringBuffer GetPath();
	SHVStringBuffer GetName();
	const SHVStringC& GetFileName();


	// positioning
	SHVFilePos GetPosition();
	bool SetPosition(SHVFilePos pos);
	bool Seek(SHVFilePos offset);


	// File state
	SHVBool Open(const SHVStringC& fileName, int flags = FlagOpen|FlagCreate|FlagRead|FlagWrite);
	inline SHVBool Create(const SHVStringC& fileName);
	void Flush();
	void Close();


	// File content operations
	SHVFilePos Read(void* buffer, SHVFilePos bufferLen);
	bool Write(const void* buffer, SHVFilePos bufferLen);


private:
	///\cond INTERNAL
	SHVString FileName;
	FILE* File;
	int Flags;
	SHV_DISABLE_COPY_AND_ASSIGNMENT(SHVFileBase);
	///\endcond
};



// ============================================= implementation ============================================= //

/*************************************
 * Create
 *************************************/
/// Creates a file, overriding it if it exists
/**
 \param fileName Name of file, with path to create
 \return Success, or one of the Err* enums
 *
 * Will create the file, or open and truncate it.
 \see Open
 */
SHVBool SHVFileBase::Create(const SHVStringC& fileName)
{
	return Open(fileName,FlagCreate|FlagWrite|FlagOverride);
}

/*************************************
 * GetFlags
 *************************************/
/// Gets the flags the file was opened with
/**
 \return Flags it was opened with, or 0
 \see Open
 */
int SHVFileBase::GetFlags()
{
	return Flags;
}

#if __SHVSTRINGDEFAULT == 8
const SHVStringC SHVFileBase::LineSeparator() { return LineSeparator8(); }
#elif __SHVSTRINGDEFAULT == 16
const SHVStringC SHVFileBase::LineSeparator() { return LineSeparator16(); }
#elif __SHVSTRINGDEFAULT == utf8
const SHVStringC SHVFileBase::LineSeparator() { return LineSeparatorUTF8(); }
#endif

#endif
