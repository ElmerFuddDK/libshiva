/*
 *   Copyright (C) 2008 by Lars Eriksen
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
#include "../../../include/platformspc.h"

#include "../../../include/utils/shvdir.h"

#ifdef __SHIVA_WIN32
# define wremove _wremove
# define wfopen _wfopen
# include <shellapi.h>
# if !defined(__SHIVA_WINCE)
#  include <direct.h>
# elif !defined(SHELLEXECUTEINFOW)
#  define SHELLEXECUTEINFOW SHELLEXECUTEINFO
#  define ShellExecuteExW ShellExecuteEx
# endif
# if defined(_MSC_VER) && defined(SHIVASTATICMODULELIB)
#  pragma comment(lib, "shell32.lib")
# endif
# ifndef INVALID_FILE_ATTRIBUTES
#  define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
# endif
#else
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# include <dirent.h>
# include <fnmatch.h>
# include <stdlib.h>
# define FSUTF8MODE
# if defined(__SHIVA_POSIX_IPHONEOS) || (defined(__SHIVA_POSIX_LINUX) && !defined(ANDROID))
#  define FSUSEPOSIXSPAWN
# endif
# ifdef FSUSEPOSIXSPAWN
#  include <spawn.h>
#  include <sys/wait.h>
extern char **environ;
int SHVDir_system(const char* cmd)
{
pid_t pid;
int retVal;
const char* argv[] = { "sh", "-c", NULL, NULL }; argv[2] = cmd;
	posix_spawnp(&pid,argv[0],NULL,NULL,(char*const*)argv,environ);
	return waitpid(pid,&retVal,0);
}
# else
int SHVDir_system(const char* cmd) { return ::system(cmd); }
# endif
#endif

// It is assumed that the file system is utf8 when running posix - so not possible to run utf8 native string and 8bit FS
#if !defined(FSUTF8MODE) && defined(__SHIVA_POSIX) && __SHVSTRINGDEFAULT == utf8
# error Invalid string configuration for shvdir
#endif


//=========================================================================================================
// SHVDir class - Directory management
//=========================================================================================================
/// \class SHVDir shvdir.h "shiva/include/utils/shvdir.h"

/*************************************
 * Delimiter
 *************************************/
/// Returns the file delimiter for the platform
/**
 * On windows and symbian it will return \\, otherwise /.
 */
const SHVStringC SHVDir::Delimiter()
{
#ifdef __SHIVA_WIN32
	return _S("\\");
#elif defined(__SHIVA_EPOC)
	return _S("\\");
#else
	return _S("/");
#endif
}

/*************************************
 * WildcardAll
 *************************************/
/// Returns the default wildcard for all files
/**
 * Some silly plaforms require a file extension when matching
 * all files. This function can be used to correct this.
 * On windows it will return "*.*2, otherwise "*".
 */
const SHVStringC SHVDir::WildcardAll()
{
#ifdef __SHIVA_WIN32
	return _S("*.*");
#elif defined(__SHIVA_EPOC)
	return _S("*.*");
#else
	return _S("*");
#endif
}

/*************************************
 * GetFileList
 *************************************/
/// Creates a list of files in a path
/**
 \param list List to contain the files in the path
 \param path Path to look for files in
 \param wildcard Optional wildcard
 \return true if any files were in the path
 \note The list will be cleared by the function
 */
SHVBool SHVDir::GetFileList(SHVFileList& list, const SHVStringC path, const SHVStringC wildcard)
{
SHVBool retVal;
SHVFileList lstDummy;

	retVal = SHVDir::GetDirContent(lstDummy, list, path, wildcard);

	if (retVal && list.GetCount() == 0)
		retVal.SetError(ErrEmpty);

	return retVal;
}

/*************************************
 * GetDirList
 *************************************/
/// Creates a list of sub directories in a path
/**
 \param list List to contain the sub directories in the path
 \param path Path to look for dirs in
 \param wildcard Optional wildcard
 \return true if any dirs were in the path
 \note The list will be cleared by the function
 */
SHVBool SHVDir::GetDirList(SHVFileList& list, const SHVStringC path, const SHVStringC wildcard)
{
SHVBool retVal;
SHVFileList lstDummy;

	retVal = SHVDir::GetDirContent(list, lstDummy, path, wildcard);

	if (retVal && list.GetCount() == 0)
		retVal.SetError(ErrEmpty);

	return retVal;
}

/*************************************
 * GetDirContent
 *************************************/
/// Creates a list of sub directories and files in a path
/**
 \param dirs List to contain the sub directories in the path
 \param files List to contain the files in the path
 \param path Path to look for dirs in
 \param wildcard Optional wildcard
 \return true if any files or dirs were in the path
 \note The list will be cleared by the function
 */
SHVBool SHVDir::GetDirContent(SHVFileList& dirs, SHVFileList& files, const SHVStringC path, const SHVStringC wildcard)
{
SHVBool retVal(SHVBool::True);
SHVString fileName;
SHVStringC wc(wildcard.GetBufferConst());

	if (wc.IsNull())
	{
		wc = WildcardAll();
	}

	dirs.RemoveAll();
	files.RemoveAll();

	fileName.Format(_S("%s%s%s"),
		path.GetSafeBuffer(),
		path.Right(1) == Delimiter() ? _S("") : Delimiter().GetSafeBuffer(),
		wc.GetSafeBuffer());

#if defined(__SHIVA_WIN32)
WIN32_FIND_DATAW lpData;
int num;
HANDLE handle = ::FindFirstFileW(fileName.AsStr16C().GetSafeBufferWin32(), &lpData);

	num = lpData.dwFileAttributes;	// what we're doing here is isolating the bit
	num >>= 4;						// that tells us if the handle points to a 
	num &= 0x00000001;				// directory

	if(handle != INVALID_HANDLE_VALUE)
	{
		if(num != 1)
		{
# if __SHVSTRINGDEFAULT == 16
			files.AddTail((SHVWChar*)lpData.cFileName);
# else
			files.AddTail(SHVString16C((SHVWChar*)lpData.cFileName).ToStrT());
# endif
		}
		else if (SHVStringC(_S(".")).Compare((SHVTChar*)lpData.cFileName))
		{
# if __SHVSTRINGDEFAULT == 16
			dirs.AddTail((SHVWChar*)lpData.cFileName);
# else
			dirs.AddTail(SHVString16C((SHVWChar*)lpData.cFileName).ToStrT());
# endif
		}
	}

	while(handle != INVALID_HANDLE_VALUE)
	{
		if(::FindNextFileW(handle, &lpData) )
		{
			num = lpData.dwFileAttributes;	// what we're doing here is isolating the bit
			num >>= 4;						// that tells us if the handle points to a 
			num &= 0x00000001;				// directory

			if(num != 1)
			{
# if __SHVSTRINGDEFAULT == 16
				files.AddTail((SHVWChar*)lpData.cFileName);
# else
				files.AddTail(SHVString16C((SHVWChar*)lpData.cFileName).ToStrT());
# endif
			}
			else if (SHVStringC(_S("..")).Compare((SHVTChar*)lpData.cFileName))
			{
# if __SHVSTRINGDEFAULT == 16
				dirs.AddTail((SHVWChar*)lpData.cFileName);
# else
				dirs.AddTail(SHVString16C((SHVWChar*)lpData.cFileName).ToStrT());
# endif
			}
		}
		else
			break;
	}
	if (handle != INVALID_HANDLE_VALUE)
		::FindClose(handle);
#elif defined __SHIVA_EPOC
RFs fs;
CDir* fileList;
CDir* dirList;

	fs.Connect();
	fs.GetDir(fileName.ToPtr(), KEntryAttNormal, ESortByName, fileList, dirList);
	
	if(fileList)
	{
		for(int i=0; i<fileList->Count(); i++)
			files.AddTail( (SHVTChar*) (*fileList)[i].iName.Ptr() );
		delete fileList;
	}
	
	if(dirList)
	{
		for(int i=0; i<dirList->Count(); i++)
			dirs.AddTail( (SHVTChar*) (*dirList)[i].iName.Ptr() );
		delete dirList;
	}
#else
DIR *dp;
struct dirent *ep;
struct stat fStat;
#if defined(FSUTF8MODE) && __SHVSTRINGDEFAULT != utf8
SHVString dirName;
SHVStringUTF8 wcn(wc.ToStrUTF8());

	dirName.Format("%s%s",
		path.ToStrUTF8().GetSafeBuffer(),
		path.Right(1) == Delimiter() ? "" : Delimiter().GetSafeBuffer());
#else
SHVString dirName;
const SHVStringC wcn(wc);

	dirName.Format(_S("%s%s"),
		path.GetSafeBuffer(),
		path.Right(1) == Delimiter() ? _S("") : Delimiter().GetSafeBuffer());
#endif

	dp = opendir(dirName.GetSafeBuffer());
	if(dp != NULL)
	{
		while((ep = readdir(dp)))
		{
			if(!fnmatch(wcn.GetSafeBuffer(), ep->d_name, 0))
			{
				if(!stat(SHVStringC(dirName+SHVStringC(ep->d_name)).GetSafeBuffer(), &fStat))
				{
					if (S_ISREG(fStat.st_mode))
					{
						files.AddTail(ep->d_name);
					}
					else if (S_ISDIR(fStat.st_mode))
					{
						dirs.AddTail(ep->d_name);
					}
				}
			}
			//free(ep); ///\todo Find out if this is needed
		}
		(void) closedir(dp);
	}
#endif

	if (retVal && files.GetCount() == 0 && dirs.GetCount() == 0)
		retVal.SetError(ErrEmpty);

	return retVal;
}

/*************************************
 * IsDir
 *************************************/
/// Returns true if fileName is a directory
bool SHVDir::IsDir(const SHVStringC fileName)
{
SHVFileList dirList;
	return GetDirList(dirList,SHVDir::ExtractPath(fileName),SHVDir::ExtractName(fileName));
}

/*************************************
 * IsFile
 *************************************/
/// Returns true if fileName is a regular file
bool SHVDir::IsFile(const SHVStringC fileName)
{
SHVFileList fileList;
	return GetFileList(fileList,SHVDir::ExtractPath(fileName),SHVDir::ExtractName(fileName));
}

/*************************************
 * Move
 *************************************/
/// Moves a regular file
/**
 \param from Location of file to move from
 \param to Location of file to move to
 \return Success
 */
SHVBool SHVDir::Move(const SHVStringC from, const SHVStringC to)
{
SHVBool retVal;

	if ((SHVDir::IsFile(from) && !SHVDir::FileExist(from)) || (SHVDir::IsDir(from) && !SHVDir::DirExist(from)))
	{
		retVal.SetError(ErrDoesNotExist);
	}
	else if (SHVDir::FileExist(to) || SHVDir::DirExist(to))
	{
		retVal.SetError(ErrExists);
	}
	else
	{
#ifdef __SHIVA_WIN32
		retVal = (::MoveFileW(from.AsStr16C().GetSafeBufferWin32(),to.AsStr16C().GetSafeBufferWin32()) ? SHVBool::True : SHVBool::False);

#elif defined __SHIVA_EPOC
	RFs fs;
		fs.Connect();
	
		retVal = ( fs.Rename(from.ToStr16().ToPtr(),to.ToStr16().ToPtr()) ? SHVBool::False : SHVBool::True );
#elif defined FSUTF8MODE
		retVal = ( rename(from.AsStrUTF8C().GetSafeBuffer(),to.AsStrUTF8C().GetSafeBuffer()) ? SHVBool::False : SHVBool::True );
#else
		retVal = ( rename(from.AsStr8C().GetSafeBuffer(),to.AsStr8C().GetSafeBuffer()) ? SHVBool::False : SHVBool::True );
#endif
	}

	return retVal;
}

/*************************************
 * Copy
 *************************************/
/// Copies a regular file
/**
 \param from Location of file to copy
 \param to Location to copy to
 \return Success
 */
SHVBool SHVDir::Copy(const SHVStringC from, const SHVStringC to)
{
SHVBool retVal;

	if (!SHVDir::FileExist(from))
	{
		retVal.SetError(ErrDoesNotExist);
	}
	else if (SHVDir::FileExist(to) || SHVDir::DirExist(to))
	{
		retVal.SetError(ErrExists);
	}
	else
	{
#ifdef __SHIVA_WIN32
		retVal = (::CopyFileW(from.AsStr16C().GetSafeBufferWin32(),to.AsStr16C().GetSafeBufferWin32(),TRUE) ? SHVBool::True : SHVBool::False);
#else
	SHVFileBase filefrom,fileto;
	bool ok = true;
	long rlen = 512;
	char buffer[512];

		ok = (filefrom.Open(from,SHVFileBase::FlagOpen|SHVFileBase::FlagRead) && fileto.Create(to));
		while (ok && rlen == 512)
		{
			rlen = filefrom.Read(buffer,512);

			if (rlen > 0)
				retVal = fileto.Write(buffer,rlen);
		}

		retVal = ok;
#endif
	}

	return retVal;
}

/*************************************
 * CreateDir
 *************************************/
/// Creates a sub directory
SHVBool SHVDir::CreateDir(const SHVStringC dirName)
{
SHVBool retVal;

	if (SHVDir::FileExist(dirName) || SHVDir::DirExist(dirName))
	{
		retVal.SetError(ErrExists);
	}
	else
	{
#ifdef __SHIVA_WIN32
		if (dirName.Right(1) == Delimiter())
			retVal = SHVDir::CreateDir(dirName.Left(dirName.GetLength()-1));
		else
			retVal = (::CreateDirectoryW(dirName.AsStr16C().GetSafeBufferWin32(),NULL) ? SHVBool::True :  SHVBool::False);
#elif defined __SHIVA_EPOC
		retVal = (wmkdir((wchar_t*)dirName.AsStr16C().GetSafeBuffer(),0777) == 0);
#elif defined(FSUTF8MODE)
		retVal = (mkdir(dirName.AsStrUTF8C().GetSafeBuffer(),0777) == 0);
#else
		retVal = (mkdir(dirName.AsStr8C().GetSafeBuffer(),0777) == 0);
#endif
	}

	return retVal;
}

/*************************************
 * DeleteDir
 *************************************/
/// Deletes a directory
SHVBool SHVDir::DeleteDir(const SHVStringC dirName)
{
SHVBool retVal;

	if (!SHVDir::DirExist(dirName))
	{
		retVal.SetError(ErrDoesNotExist);
	}
	else
	{
#ifdef __SHIVA_WIN32
		retVal = (::RemoveDirectoryW(dirName.AsStr16C().GetSafeBufferWin32()) ? SHVBool::True : SHVBool::False);
#elif defined(__SHIVA_EPOC)
		retVal = (wremove(dirName.AsStr16C().GetSafeBufferWin32()) ? SHVBool::False : SHVBool::True);
#elif defined(FSUTF8MODE)
		retVal = (remove(dirName.AsStrUTF8C().GetSafeBuffer()) ? SHVBool::False : SHVBool::True);
#else
		retVal = (remove(dirName.AsStr8C().GetSafeBuffer()) ? SHVBool::False : SHVBool::True);
#endif
	}

	return retVal;
}

/*************************************
 * ChangeDir
 *************************************/
/// Change current working directory
SHVBool SHVDir::ChangeDir(const SHVStringC dirName)
{
SHVBool retVal(SHVBool::False);
#if defined(__SHIVA_WINCE)
#elif defined(__SHIVA_WIN32)
	retVal = _wchdir(dirName.ToStr16().GetBufferConstWin32()) == 0;
#elif defined(FSUTF8MODE)
retVal = chdir(dirName.AsStrUTF8C().GetBufferConst()) == 0;
#else
	retVal = chdir(dirName.AsStr8C().GetBufferConst()) == 0;
#endif
	return retVal;
}

/*************************************
 * GetSize
 *************************************/
/// Obtain file size of a file name
SHVFilePos SHVDir::GetSize(const SHVStringC fileName)
{
SHVFilePos retVal = 0;
SHVFileBase file;

	if (file.Open(fileName))
		retVal = file.GetSize();

	return retVal;
}

/*************************************
 * GetModifyTime
 *************************************/
/// Obtain a modify time of a file
/**
 \param fileName the file to obtaion modify time for
 \param stamp The variable to put the modify time in to
 \return Success
 *
 * The stamp will be Null on failure.
 */
SHVBool SHVDir::GetModifyTime(const SHVStringC fileName, SHVTime& stamp)
{
SHVBool retVal(SHVBool::False);
#ifdef __SHIVA_WIN32
HANDLE f;
	if ((f = CreateFileW(fileName.AsStr16C().GetSafeBufferWin32(),GENERIC_READ,FILE_SHARE_READ|FILE_SHARE_WRITE,NULL,OPEN_EXISTING,FILE_ATTRIBUTE_NORMAL,NULL)))
	{
	FILETIME mTime;

		stamp = SHVTime();

		if (::GetFileTime(f,NULL,NULL,&mTime))
		{
		SYSTEMTIME sysTime;
			FileTimeToSystemTime(&mTime,&sysTime);
			stamp.SetYear(sysTime.wYear);
			stamp.SetMonth(sysTime.wMonth);
			stamp.SetDay(sysTime.wDay);
			stamp.SetHour(sysTime.wHour);
			stamp.SetMinute(sysTime.wMinute);
			stamp.SetSecond(sysTime.wSecond);
			retVal.SetError(SHVBool::True);
		}

		CloseHandle(f);
	}
#elif defined(__SHIVA_EPOC)
struct stat fileStat;
	stamp.SetNull();
	if (!wstat( fileName.ToStr16().GetSafeBuffer(), &fileStat ))
	{
		retVal.SetError(SHVBool::True);
		stamp = SHVTime::FromUnixTime(fileStat.st_mtime);
	}
#else
# if defined(FSUTF8MODE) && __SHVSTRINGDEFAULT != utf8
SHVStringUTF8 fName(fileName.ToStrUTF8());
# else
const SHVStringC fName(fileName);
# endif
struct stat fileStat;
	stamp.SetNull();
	if (!stat( fName.GetSafeBuffer(), &fileStat ))
	{
		retVal.SetError(SHVBool::True);
		stamp = SHVTime::FromUnixTime(fileStat.st_mtime);
	}
#endif
	else
	{
		stamp = SHVTime();
	}

	return retVal;
}

/*************************************
 * DeleteFile
 *************************************/
/// Deletes a file
SHVBool SHVDir::DeleteFile(const SHVStringC fileName)
{
SHVBool retVal;

	if (!SHVDir::FileExist(fileName))
	{
		retVal.SetError(ErrDoesNotExist);
	}
	else
	{
#ifdef __SHIVA_WINCE
		retVal = (::DeleteFileW(fileName.AsStr16C().GetSafeBufferWin32()) ? SHVBool::True : SHVBool::False);
#elif defined(__SHIVA_WIN32)
		retVal = (wremove(fileName.AsStr16C().GetSafeBufferWin32()) ? SHVBool::False : SHVBool::True);
#elif defined(__SHIVA_EPOC)
		retVal = (wremove((const WCHAR*)fileName.AsStr16C().GetSafeBuffer()) ? SHVBool::False : SHVBool::True);
#elif defined(FSUTF8MODE)
		retVal = (remove(fileName.AsStrUTF8C().GetSafeBuffer()) ? SHVBool::False : SHVBool::True);
#else
		retVal = (remove(fileName.AsStr8C().GetSafeBuffer()) ? SHVBool::False : SHVBool::True);
#endif
	}

	return retVal;
}

/*************************************
 * FileExist
 *************************************/
/// Tests if a file exists
/**
 \param fileName Name of file to check for, with path
 \return true if it exists and is not a directory
 */
bool SHVDir::FileExist(const SHVStringC fileName)
{
#ifdef __SHIVA_WIN32
DWORD attrib = ::GetFileAttributesW( fileName.AsStr16C().GetSafeBufferWin32()); // returns 0xFFFFFFFF on failure
	return ( !(attrib&FILE_ATTRIBUTE_DIRECTORY) );
#elif defined(__SHIVA_EPOC)
struct stat fileStat;
	return ( !wstat( fileName.AsStr16C().GetSafeBuffer(), &fileStat ) && fileStat.st_mode&S_IFREG );
#elif defined(FSUTF8MODE)
struct stat fileStat;
	return ( !stat( fileName.AsStrUTF8C().GetSafeBuffer(), &fileStat ) && fileStat.st_mode&S_IFREG );
#else
struct stat fileStat;
	return ( !stat( fileName.AsStr8C().GetSafeBuffer(), &fileStat ) && fileStat.st_mode&S_IFREG );
#endif
}

/*************************************
 * DirExist
 *************************************/
/// Tests if a directory exists
/**
 \param fileName Name of dir to check for, with path
 \return true if it exists and is a directory
 */
bool SHVDir::DirExist(const SHVStringC fileName)
{
#ifdef __SHIVA_WIN32
DWORD attrib = ::GetFileAttributesW( fileName.AsStr16C().GetSafeBufferWin32()); // returns 0xFFFFFFFF on failure
	return ( attrib != INVALID_FILE_ATTRIBUTES && (attrib&FILE_ATTRIBUTE_DIRECTORY) );
#elif defined(__SHIVA_EPOC)
struct stat fileStat;
	return ( !wstat( fileName.AsStr16C().GetSafeBufferWin32(), &fileStat ) && fileStat.st_mode&S_IFDIR );
#elif defined(FSUTF8MODE)
struct stat fileStat;
	return ( !stat( fileName.AsStrUTF8C().GetSafeBuffer(), &fileStat ) && fileStat.st_mode&S_IFDIR );
#else
struct stat fileStat;
	return ( !stat( fileName.AsStr8C().GetSafeBuffer(), &fileStat ) && fileStat.st_mode&S_IFDIR );
#endif
}

/*************************************
 * ExecuteFile
 *************************************/
/// Executes a file in the shell
/**
 \param fileName The file to execute
 *
 * The file will be opened with whatever program is associated with it.
 */
void SHVDir::ExecuteFile(const SHVStringC fileName)
{
#if defined(__SHIVA_WIN32)
SHELLEXECUTEINFOW info;
SHVString16 fileNameW = fileName.ToStr16();
SHVString16 pathW = SHVDir::ExtractPath(fileName).ToStr16();

	memset(&info, 0, sizeof(SHELLEXECUTEINFOW));
	info.cbSize = sizeof(SHELLEXECUTEINFOW);
	info.fMask  = SEE_MASK_NOCLOSEPROCESS;
	info.lpVerb = L"open";
	info.lpFile = fileNameW.GetSafeBufferWin32();
	info.lpDirectory = pathW.GetSafeBufferWin32();
	info.nShow = SW_SHOWNORMAL;
	ShellExecuteExW(&info);

#elif defined(FSUSEPOSIXSPAWN)
pid_t pid;
const char* argv[] = { "xdg-open", NULL, NULL };
# ifdef FSUTF8MODE
SHVStringUTF8 fileNameUTF8(fileName.ToStrUTF8());
	argv[1] = fileNameUTF8.GetSafeBuffer();
	::posix_spawnp(&pid, "xdg-open", NULL, NULL, (char*const*)argv, environ);
# else
	argv[1] = fileName.GetSafeBuffer();
	::posix_spawnp(&pid, "xdg-open", NULL, NULL, (char*const*)argv, environ);
# endif
#elif defined(__SHIVA_POSIX)
SHVString execstr;
	execstr.Format(_S("xdg-open \"%s\" &"), fileName.GetSafeBuffer());
# ifdef FSUTF8MODE
	::system(execstr.ToStrUTF8().GetSafeBuffer());
# else
	::system(execstr.GetSafeBuffer());
# endif
#elif defined(__SHIVA_EPOC)
	///\todo Implement SHVDir::ExecuteFile for symbian
#endif
}

/*************************************
 * ExtractName
 *************************************/
/// Extracts the path from a filename
/**
 \param fileName name of file, with path
 \return Name of file
 *
 * Will return a the whole string if fileName does not contain a path.
 */
SHVStringBuffer SHVDir::ExtractName(const SHVStringC fileName)
{
long pos = fileName.ReverseFind(SHVDir::Delimiter());
	return (pos >= 0 ? fileName.Mid(pos+1) : SHVString(fileName).ReleaseBuffer());
}

/*************************************
 * ExtractExtension
 *************************************/
/// Extracts the extension from a filename without the .
/**
 \param fileName name of file
 \return extension of file without .
 *
 * Will return the extension without .
 */
SHVStringBuffer SHVDir::ExtractExtension(const SHVStringC fileName)
{
long pos = fileName.ReverseFind(_S("."));
	return (pos >= 0 ? fileName.Mid(pos+1) : SHVString(fileName).ReleaseBuffer());
}

/*************************************
 * ExtractPath
 *************************************/
/// Extracts the path from a filename
/**
 \param fileName name of file, with path
 \return Path to file without the final delimiter
 *
 * Will return a Null string if fileName does not contain a path.
 */
SHVStringBuffer SHVDir::ExtractPath(const SHVStringC fileName)
{
long pos = fileName.ReverseFind(SHVDir::Delimiter());
	return (pos >= 0 ? fileName.Mid(0,pos) : SHVString().ReleaseBuffer());
}

/*************************************
 * Execute
 *************************************/
/// Will execute a program with optional argument string
void SHVDir::Execute(const SHVStringC program, const SHVStringC args)
{
#if defined(__SHIVA_WIN32)
SHELLEXECUTEINFOW info;
SHVString16 programW = program.ToStr16();
SHVString16 argsW = args.ToStr16();
	
	memset(&info, 0, sizeof(SHELLEXECUTEINFOW));
	info.cbSize = sizeof(SHELLEXECUTEINFOW);
	info.lpFile = programW.GetSafeBufferWin32();
	info.lpParameters = argsW.GetSafeBufferWin32();
	info.nShow = SW_SHOWNORMAL;
	ShellExecuteExW(&info);
#elif defined(__SHIVA_EPOC)
	///\todo Implement SHVDir::Execute for symbian
#else
SHVString execstr;
	execstr.Format(_S("%s %s &"), EscapeParameter(program).GetSafeBuffer(), args.GetSafeBuffer());
# ifdef FSUTF8MODE
	::SHVDir_system(execstr.AsStrUTF8C().GetSafeBuffer());
# else
	::SHVDir_system(execstr.GetSafeBuffer());
# endif
#endif
}

/*************************************
 * Execute
 *************************************/
/// Will execute a program with a provided argument list
void SHVDir::Execute(const SHVStringC program, SHVFileList& args)
{
#if defined(__SHIVA_WIN32)
SHELLEXECUTEINFOW info;
SHVString16 programW = program.ToStr16();
SHVString16 argW;
SHVFileListIterator itr(args);

	while (itr.MoveNext())
	{
		if (argW.IsNull())
			argW = itr.Get().ToStr16();
		else
			argW += (const SHVWChar*)L" " + itr.Get().ToStr16();
	}

	memset(&info, 0, sizeof(SHELLEXECUTEINFOW));
	info.cbSize = sizeof(SHELLEXECUTEINFOW);
	info.lpFile = programW.GetSafeBufferWin32();
	info.lpParameters = argW.GetSafeBufferWin32();
	info.nShow = SW_SHOWNORMAL;
	ShellExecuteExW(&info);
#elif defined(__SHIVA_EPOC)
	///\todo Implement SHVDir::Execute for symbian
#elif defined(FSUSEPOSIXSPAWN)
const char** argv = (const char**)malloc(sizeof(const char*)*(args.GetCount()+2));
int i=0;
SHVListPos pos=NULL;
pid_t pid;

# ifdef FSUTF8MODE
SHVStringUTF8 programUTF8(EscapeParameter(program).ToStrUTF8());
SHVList<SHVStringUTF8,SHVStringBufferUTF8> argsUTF8;
	while (args.MoveNext(pos))
		argsUTF8.AddTail(args.GetAt(pos).ToStrUTF8());

	argv[i++] = programUTF8.GetSafeBuffer();
	while (argsUTF8.MoveNext(pos))
		argv[i++] = argsUTF8.GetAt(pos).GetSafeBuffer();
# else
	argv[i++] = program.GetSafeBuffer();
	while (args.MoveNext(pos))
		argv[i++] = args.GetAt(pos).GetSafeBuffer();
# endif

	argv[i] = NULL;

	posix_spawnp(&pid,argv[0],NULL,NULL,(char*const*)argv,environ);
	free(argv);
#else
SHVString execstr;
SHVString arg;
SHVFileListIterator itr(args);
SHVString tmpArg;

	while (itr.MoveNext())
	{
		tmpArg = EscapeParameter(itr.Get());
		if (arg.IsNull())
			arg = tmpArg.ReleaseBuffer();
		else
			arg += SHVStringC::Format(_S(" %s"), tmpArg.GetSafeBuffer());
	}

	execstr.Format(_S("%s %s &"), EscapeParameter(program).GetSafeBuffer(), arg.GetSafeBuffer());
# ifdef FSUTF8MODE
	::system(execstr.ToStrUTF8().GetSafeBuffer());
# else
	::system(execstr.GetSafeBuffer());
# endif
#endif
}

/*************************************
 * ExecuteAndWait
 *************************************/
/// Will execute a program with optional argument string
SHVInt SHVDir::ExecuteAndWait(const SHVStringC program, const SHVStringC args)
{
SHVInt retVal;
#if defined(__SHIVA_WINCE)
#elif defined(__SHIVA_WIN32)
SHVString execstr;
	execstr.Format(_S("\"%s %s\""), EscapeParameter(program).GetSafeBuffer(), args.GetSafeBuffer());
	retVal = ::system(execstr.ToStr8().GetSafeBuffer());
#elif defined(__SHIVA_EPOC)
	///\todo Implement SHVDir::Execute for symbian
#else
SHVString execstr;
	execstr.Format(_S("%s %s"), EscapeParameter(program).GetSafeBuffer(), args.GetSafeBuffer());
# ifdef FSUTF8MODE
	retVal = ::SHVDir_system(execstr.AsStrUTF8C().GetSafeBuffer());
# else
	retVal = ::SHVDir_system(execstr.AsStr8C().GetSafeBuffer());
# endif
#endif
	return retVal;
}

/*************************************
 * ExecuteAndWait
 *************************************/
/// Will execute a program with a provided argument list
SHVInt SHVDir::ExecuteAndWait(const SHVStringC program, SHVFileList& args)
{
SHVInt retVal;
#if defined(__SHIVA_WINCE)
#elif defined(__SHIVA_WIN32)
SHVString execstr;
SHVString arg;
SHVFileListIterator itr(args);
SHVString tmpArg;

	while (itr.MoveNext())
	{
		tmpArg = EscapeParameter(itr.Get());
		if (arg.IsNull())
			arg = tmpArg.ReleaseBuffer();
		else
			arg += SHVStringC::Format(_S(" %s"), tmpArg.GetSafeBuffer());
	}
	execstr.Format(_S("\"%s %s\""), EscapeParameter(program).GetSafeBuffer(), arg.GetSafeBuffer());
	retVal = ::system(execstr.ToStr8().GetSafeBuffer());
#elif defined(__SHIVA_EPOC)
	///\todo Implement SHVDir::Execute for symbian
#elif defined(FSUSEPOSIXSPAWN)
const char** argv = (const char**)malloc(sizeof(const char*)*(args.GetCount()+2));
int i=0;
SHVListPos pos=NULL;
pid_t pid;

# ifdef FSUTF8MODE
SHVStringUTF8 programUTF8(EscapeParameter(program).ToStrUTF8());
SHVList<SHVStringUTF8,SHVStringBufferUTF8> argsUTF8;
	while (args.MoveNext(pos))
		argsUTF8.AddTail(args.GetAt(pos).ToStrUTF8());

	argv[i++] = programUTF8.GetSafeBuffer();
	while (argsUTF8.MoveNext(pos))
		argv[i++] = argsUTF8.GetAt(pos).GetSafeBuffer();
# else
	argv[i++] = program.GetSafeBuffer();
	while (args.MoveNext(pos))
		argv[i++] = args.GetAt(pos).GetSafeBuffer();
# endif

	argv[i] = NULL;

	posix_spawnp(&pid,argv[0],NULL,NULL,(char*const*)argv,environ);
	free(argv);
	waitpid(pid,&i,0);
	retVal = i;
#else
SHVString execstr;
SHVString arg;
SHVFileListIterator itr(args);
SHVString tmpArg;

	while (itr.MoveNext())
	{
		tmpArg = EscapeParameter(itr.Get());
		if (arg.IsNull())
			arg = tmpArg.ReleaseBuffer();
		else
			arg += SHVStringC::Format(_S(" %s"), tmpArg.GetSafeBuffer());
	}

	execstr.Format(_S("%s %s"), EscapeParameter(program).GetSafeBuffer(), arg.GetSafeBuffer());
# ifdef FSUTF8MODE
	retVal = ::system(execstr.ToStrUTF8().GetSafeBuffer());
# else
	retVal = ::system(execstr.GetSafeBuffer());
# endif
#endif
	return retVal;
}

///\cond INTERNAL
/*************************************
 * EscapeParameter
 *************************************/
SHVStringBuffer SHVDir::EscapeParameter(const SHVStringC param)
{
SHVString retVal(param);
#ifdef __SHIVA_WIN32
	if (param.Find(_S("\"")) >= 0)
	{
		retVal.Replace(_S("\""),_S("\"\""));		
	}
	retVal = SHVStringC::Format(_S("\"%s\""),retVal.GetSafeBuffer());
#else
	if (param.Find(_S(" ")) >= 0 || param.Find(_S("\\")) >= 0 || param.Find(_S("(")) >= 0 || param.Find(_S(")")) >= 0)
	{
		retVal.Replace(_S("\\"),_S("\\\\"));
		retVal.Replace(_S("\""),_S("\\\""));
		retVal = SHVStringC::Format(_S("\"%s\""),retVal.GetSafeBuffer());
	}
#endif
	return retVal.ReleaseBuffer();
}
///\endcond

