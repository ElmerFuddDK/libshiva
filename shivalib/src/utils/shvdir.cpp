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
#else
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
#endif


//=========================================================================================================
// SHVDir class - Directory management
//=========================================================================================================

/*************************************
 * Delimiter
 *************************************/
const SHVStringC SHVDir::Delimiter()
{
#ifdef __SHIVA_WIN32
	return _T("\\");
#elif defined(__SHIVA_EPOC)
	return _T("\\");
#else
	return _T("/");
#endif
}

/*************************************
 * WildcardAll
 *************************************/
const SHVStringC SHVDir::WildcardAll()
{
#ifdef __SHIVA_WIN32
	return _T("*.*");
#elif defined(__SHIVA_EPOC)
	return _T("*.*");
#else
	return _T("*");
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

	fileName.Format(_T("%s%s%s"),
		path.GetSafeBuffer(),
		path.Right(1) == Delimiter() ? _T("") : Delimiter(),
		wc.GetSafeBuffer());

#if defined(__SHIVA_WIN32)
WIN32_FIND_DATA lpData;
int num;
HANDLE handle = ::FindFirstFile(fileName.GetSafeBuffer(), &lpData);

	num = lpData.dwFileAttributes;	// what we're doing here is isolating the bit
	num >>= 4;						// that tells us if the handle points to a 
	num &= 0x00000001;				// directory

	if(handle != INVALID_HANDLE_VALUE)
	{
		if(num != 1)
			files.AddTail(lpData.cFileName);
		else if (SHVStringC(_T(".")).Compare(lpData.cFileName))
			dirs.AddTail(lpData.cFileName);
	}

	while(handle != INVALID_HANDLE_VALUE)
	{
		if(::FindNextFile(handle, &lpData) )
		{
			num = lpData.dwFileAttributes;	// what we're doing here is isolating the bit
			num >>= 4;						// that tells us if the handle points to a 
			num &= 0x00000001;				// directory

			if(num != 1)
				files.AddTail(lpData.cFileName);
			else if (SHVStringC(_T("..")).Compare(lpData.cFileName))
				dirs.AddTail(lpData.cFileName);
		}
		else
			break;
	}
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
SHVString dirName;
struct stat fStat;
char* tmpStr;

	dirName.Format((_T("%s%s"),
		path.GetSafeBuffer(),
		path.Right(1) == Delimiter() ? _T("") : Delimiter());

	dp = opendir(dirName.GetSafeBuffer());
	if(dp != NULL)
	{
		while((ep = readdir(dp)))
		{
			if(!fnmatch(wc.GetSafeBuffer(), ep->d_name, 0))
			{
				if(!stat(dirName.GetSafeBuffer()+ep->d_name, &fStat))
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
bool SHVDir::IsDir(const SHVStringC fileName)
{
SHVFileList dirList;
	return GetDirList(dirList,SHVDir::ExtractPath(fileName),SHVDir::ExtractName(fileName));
}

/*************************************
 * IsFile
 *************************************/
bool SHVDir::IsFile(const SHVStringC fileName)
{
SHVFileList fileList;
	return GetFileList(fileList,SHVDir::ExtractPath(fileName),SHVDir::ExtractName(fileName));
}

/*************************************
 * Move
 *************************************/
SHVBool SHVDir::Move(const SHVStringC from, const SHVStringC to)
{
SHVBool retVal;

	if (!SHVDir::Exist(from))
	{
		retVal.SetError(ErrDoesNotExist);
	}
	else if (SHVDir::Exist(to))
	{
		retVal.SetError(ErrExists);
	}
	else
	{
#ifdef __SHIVA_WIN32
		retVal = (::MoveFile(from.GetSafeBuffer(),to.GetSafeBuffer()) ? SHVBool::True : SHVBool::False);

#elif defined __SHIVA_EPOC
	RFs fs;
		fs.Connect();
	
		retVal = ( fs.Rename(from.ToPtr(),to.ToPtr()) ? SHVBool::False : SHVBool::True );
#else
		retVal = ( rename(from.GetSafeBuffer(),to.GetSafeBuffer()) ? SHVBool::False : SHVBool::True );
#endif
	}

	return retVal;
}

/*************************************
 * Copy
 *************************************/
SHVBool SHVDir::Copy(const SHVStringC from, const SHVStringC to)
{
SHVBool retVal;

	if (!SHVDir::Exist(from))
	{
		retVal.SetError(ErrDoesNotExist);
	}
	else if (SHVDir::Exist(to))
	{
		retVal.SetError(ErrExists);
	}
	else
	{
#ifdef __SHIVA_WIN32
		retVal = (::CopyFile(from.GetSafeBuffer(),to.GetSafeBuffer(),TRUE) ? SHVBool::True : SHVBool::False);
#else
	SHVFileBase filefrom,fileto;
	bool ok = true;
	long rlen = 512;
	char buffer[512];

		ok = (filefrom.Open(from) && fileto.Create(to));
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
SHVBool SHVDir::CreateDir(const SHVStringC dirName)
{
SHVBool retVal;

	if (SHVDir::Exist(dirName))
	{
		retVal.SetError(ErrExists);
	}
	else
	{
#ifdef __SHIVA_WIN32
		if (dirName.Right(1) == Delimiter())
			retVal = SHVDir::CreateDir(dirName.Left(dirName.GetLength()-1));
		else
			retVal = (::CreateDirectory(dirName.GetSafeBuffer(),NULL) ? SHVBool::True :  SHVBool::False);
#elif defined __SHIVA_EPOC
# ifdef UNICODE
		retVal = (wmkdir((wchar_t*)(WCHAR*)dirName.GetSafeBuffer(),448) == 0);
# else
		retVal = (mkdir(dirName.GetSafeBuffer(),448) == 0);
# endif
#else
		retVal = (mkdir(dirName.GetSafeBuffer(),448) == 0);
#endif
	}

	return retVal;
}

/*************************************
 * GetSize
 *************************************/
SHVFilePos SHVDir::GetSize(const SHVStringC fileName)
{
SHVFilePos retVal = 0;
SHVFileBase file;

	if (file.Open(fileName))
		retVal = file.GetSize();

	return retVal;
}

/*************************************
 * DeleteFile
 *************************************/
SHVBool SHVDir::DeleteFile(const SHVStringC fileName)
{
SHVBool retVal;

	if (!SHVDir::Exist(fileName))
	{
		retVal.SetError(ErrDoesNotExist);
	}
	else
	{
#ifdef __SHIVA_WINCE
		retVal = (::DeleteFile(fileName.GetSafeBuffer()) ? SHVBool::False : SHVBool::True);
#elif defined(UNICODE)
		retVal = (wremove(fileName.GetSafeBuffer()) ? SHVBool::False : SHVBool::True);
#else
		retVal = (remove(fileName.GetSafeBuffer()) ? SHVBool::False : SHVBool::True);
#endif
	}

	return retVal;
}

/*************************************
 * Exist
 *************************************/
/// Tests if a file exists
/**
 \param fileName Name of file to check for, with path
 \return true if it exists and is not a directory
 */
bool SHVDir::Exist(const SHVStringC fileName)
{
#ifdef __SHIVA_WIN32
DWORD attrib = ::GetFileAttributes( (const TCHAR*)fileName.GetSafeBuffer()); // returns 0xFFFFFFFF on failure
	return ( !(attrib&FILE_ATTRIBUTE_DIRECTORY) );
#elif defined(UNICODE)
struct stat fileStat;
	return ( !wstat( fileName.GetSafeBuffer(), &fileStat ) && fileStat.st_mode&S_IFREG );
#else
struct stat fileStat;
	return ( !stat( fileName.GetSafeBuffer(), &fileStat ) && fileStat.st_mode&S_IFREG );
#endif
}

/*************************************
 * ExecuteFile
 *************************************/
void SHVDir::ExecuteFile(const SHVStringC fileName)
{
#if defined(__SHIVA_WIN32)
SHELLEXECUTEINFO info;
SHVString path = SHVDir::ExtractPath(fileName);

	memset(&info, 0, sizeof(SHELLEXECUTEINFO));
	info.cbSize = sizeof(SHELLEXECUTEINFO);
	info.fMask  = SEE_MASK_NOCLOSEPROCESS;
	info.lpVerb = _T("open");
	info.lpFile = fileName.GetSafeBuffer();
	info.lpDirectory = path.GetSafeBuffer();
	info.nShow = SW_SHOWNORMAL;
	ShellExecuteEx(&info);

#elif defined(__SHIVA_LINUX)
SHVString execstr;
	execstr.Format(_T("fileHandler.sh \"%s\" &"), fileName.GetSafeBuffer());
	::system(execstr);
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
long pos = fileName.ReverseFind(_T("."));
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
void SHVDir::Execute(const SHVStringC program, const SHVStringC args)
{
#if defined(__SHIVA_WIN32)
SHELLEXECUTEINFO info;
	
	memset(&info, 0, sizeof(SHELLEXECUTEINFO));
	info.cbSize = sizeof(SHELLEXECUTEINFO);
	info.lpFile = program.GetSafeBuffer();
	info.lpParameters = args.GetSafeBuffer();
	info.nShow = SW_SHOWNORMAL;
	ShellExecuteEx(&info);
#elif defined(__SHIVA_EPOC)
	///\todo Implement SHVDir::Execute for symbian
#else
SHVString execstr;
	execstr.Format(_T("%s %s &"), program..GetSafeBuffer(), args.GetSafeBuffer());
	::system(execstr);
#endif
}

/*************************************
 * Execute
 *************************************/
void SHVDir::Execute(const SHVStringC program, SHVFileList& args)
{
#if defined(__SHIVA_WIN32)
SHELLEXECUTEINFO info;
SHVString arg;
SHVFileListIterator itr(args);

	while (itr.MoveNext())
	{
		if (arg.IsNull())
			arg = itr.Get();
		else
			arg += _T(" ") + itr.Get();
	}

	memset(&info, 0, sizeof(SHELLEXECUTEINFO));
	info.cbSize = sizeof(SHELLEXECUTEINFO);
	info.lpFile = program.GetSafeBuffer();
	info.lpParameters = arg.GetSafeBuffer();
	info.nShow = SW_SHOWNORMAL;
	ShellExecuteEx(&info);
#elif defined(__SHIVA_EPOC)
	///\todo Implement SHVDir::Execute for symbian
#else
SHVString execstr;
SHVString arg;
SHVFileListIterator itr(args);

	while (itr.MoveNext())
	{
		if (arg.IsNull())
			arg = itr.Get();
		else
			arg += _T(" ") + itr.Get();
	}

	execstr.Format(_T("%s %s &"), program..GetSafeBuffer(), arg.GetSafeBuffer());
	::system(execstr);
#endif
}
