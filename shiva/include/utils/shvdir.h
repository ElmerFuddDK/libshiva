#ifndef __SHIVA_UTILS_DIR_H
#define __SHIVA_UTILS_DIR_H

#include "shvstring.h"
#include "shvlist.h"
#include "shvfilebase.h"
#include "shvtime.h"


typedef SHVList<SHVString,const SHVStringC> SHVFileList;
typedef SHVListIterator<SHVString,const SHVStringC> SHVFileListIterator;

//-=========================================================================================================
/// SHVDir class - Directory management
/**
 * Contains static functions for manipulating directories and files.
 */

class SHVAPI SHVDir
{
public:


	// defines/constants
	static const SHVStringC Delimiter();
	static const SHVStringC WildcardAll();

	enum Errors {
		ErrGeneric = SHVBool::False,
		ErrEmpty = 1,
		ErrExists = 2,
		ErrDoesNotExist = 3
	};


	// Directory operations
	static SHVBool GetFileList(SHVFileList& list, const SHVStringC path, const SHVStringC wildcard = NULL);
	static SHVBool GetDirList(SHVFileList& list, const SHVStringC path, const SHVStringC wildcard = NULL);
	static SHVBool GetDirContent(SHVFileList& dirs, SHVFileList& files, const SHVStringC path, const SHVStringC wildcard = NULL);
	static bool IsDir(const SHVStringC fileName);
	static bool IsFile(const SHVStringC fileName);
	static SHVBool Move(const SHVStringC from, const SHVStringC to);
	static SHVBool Copy(const SHVStringC from, const SHVStringC to);
	static SHVBool CreateDir(const SHVStringC dirName);


	// Operations involving a file
	static SHVFilePos GetSize(const SHVStringC fileName);
	static SHVBool GetModifyTime(const SHVStringC fileName, SHVTime& stamp);
	static SHVBool DeleteFile(const SHVStringC fileName);
	static bool FileExist(const SHVStringC fileName);
	static bool DirExist(const SHVStringC fileName);
	static void ExecuteFile(const SHVStringC fileName);


	// File name operations
	static SHVStringBuffer ExtractName(const SHVStringC fileName);
	static SHVStringBuffer ExtractExtension(const SHVStringC fileName);
	static SHVStringBuffer ExtractPath(const SHVStringC fileName);


	// Shell operations
	static void Execute(const SHVStringC program, const SHVStringC args = NULL);
	static void Execute(const SHVStringC program, SHVFileList& args);
};

#endif
