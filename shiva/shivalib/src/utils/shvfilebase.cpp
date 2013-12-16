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

#include "../../../include/utils/shvfilebase.h"
#include "../../../include/utils/shvstringutf8.h"

#include "../../../include/utils/shvdir.h"

#ifdef __SHIVA_WIN32
# define wremove _wremove
# define wfopen _wfopen
#else
# include <sys/types.h>
# include <sys/stat.h>
# include <unistd.h>
# define FSUTF8MODE
#endif



//=========================================================================================================
// SHVFileBase class - basic file operations
//=========================================================================================================
/// \class SHVFileBase shvfilebase.h "shiva/include/utils/shvfilebase.h"

/*************************************
 * Wildcard "constant"
 *************************************/
/// Returns the system specific wildcard string for identifying files
const SHVStringC SHVFileBase::Wildcard()
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
 * LineSeperator "constant"
 *************************************/
/// Returns the system specific line seperator string
const SHVString8C SHVFileBase::LineSeparator8()
{
#ifdef __SHIVA_WIN32
	return "\r\n";
#elif defined(__SHIVA_EPOC)
	return "\r\n";
#else
	return "\n";
#endif
}
const SHVString16C SHVFileBase::LineSeparator16()
{
#ifdef __SHIVA_WIN32
	return (const SHVWChar*)L"\r\n";
#elif defined(__SHIVA_EPOC)
	return (const SHVWChar*)L"\r\n";
#else
static const SHVWChar retVal[] = { '\n', '\0' };
	return retVal;
#endif
}

/*************************************
 * Constructor
 *************************************/
SHVFileBase::SHVFileBase()
{
	Flags = 0;
	File = NULL;
}

/*************************************
 * Destructor
 *************************************/
SHVFileBase::~SHVFileBase()
{
	Close();
}

/*************************************
 * IsOpen
 *************************************/
/// Returns true if the file is open
bool SHVFileBase::IsOpen()
{
	return (File ? true : false);
}

/*************************************
 * GetSize
 *************************************/
/// Returns the size of the file
/**
 \return Size of file, or InvalidPosition if not open
 */
SHVFilePos SHVFileBase::GetSize()
{
SHVFilePos retVal;

	if (IsOpen())
	{
	SHVFilePos oldPos;

		oldPos = ftell(File); // store the old position
		fseek(File,0,SEEK_END); // get the length by seeking to the end
		retVal = ftell(File);

		fseek(File,oldPos,SEEK_SET);
	}
	else
		retVal = InvalidPosition;

	return retVal;
}

/*************************************
 * GetPath
 *************************************/
/// Extracts path from filename of open file
/**
 \return Path to open file
 *
 * This function will return a Null string if the file is not open, or
 * if there isn't a path specified.
 */
SHVStringBuffer SHVFileBase::GetPath()
{
	return SHVDir::ExtractPath(FileName);
}

/*************************************
 * GetName
 *************************************/
/// Extracts name from filename of open file
/**
 \return Name of file
 *
 * This function will return a Null string if the file is not open.
 */
SHVStringBuffer SHVFileBase::GetName()
{
	return SHVDir::ExtractName(FileName);
}

/*************************************
 * GetFileName
 *************************************/
/// Returns complete file name (path + name)
const SHVStringC& SHVFileBase::GetFileName()
{
	return FileName;
}

/*************************************
 * GetPosition
 *************************************/
/// Returns current position in file
SHVFilePos SHVFileBase::GetPosition()
{
SHVFilePos retVal;

	if (IsOpen())
	{
		retVal = ftell(File);
	}
	else
		retVal = InvalidPosition;

	return retVal;
}

/*************************************
 * SetPosition
 *************************************/
/// Sets the position in file
/**
 \param pos Position to move to
 \return Success
 *
 * Sets the position in the file pos bytes from the beginning.
 * Will fail if you try to set the position to a negative value,
 * or after EOF.\n
 * Position is unaffected in case of failure.
 */
bool SHVFileBase::SetPosition(SHVFilePos pos)
{
bool retVal = (IsOpen() && pos>=0 && pos<=GetSize());

	if (retVal)
	{
		fseek(File,pos,SEEK_SET);
	}

	return retVal;
}

/*************************************
 * Seek
 *************************************/
/// Mvoes the position
/**
 \param offset Bytes to move the position
 \return Success
 *
 * if offset is negative it will move the position backwards,
 * otherwise it will be moved forwards.\n
 * Will fail if position is to move before BOF or after EOF.
 * Position is unaffected in case of failure.
 */
bool SHVFileBase::Seek(SHVFilePos offset)
{
bool retVal = IsOpen();

	if (retVal)
	{
		retVal = SetPosition(GetPosition() + offset);
	}

	return retVal;
}

/*************************************
 * Open
 *************************************/
/// Opens or creates a file
/**
 \param fileName Name of file to open, with path
 \param flags An or'ed list of flags from the Flag* enums (optional)
 \return Success, or an Err* enum
 *
 * This function will behave according to the following rules:
 \arg One or more of \c FlagRead and \c FlagWrite must be given
 \arg One or more of \c FlagOpen and \c FlagCreate must be given
 \arg If file does not exist, and \c FlagCreate is not given
 *    it will fail with ErrMissingFile
 \arg If file exists, and \c FlagOpen is not given it will fail
 *    with ErrExists
 \arg If \c FlagWrite is given, it will seek to the end of the
 *    file. Otherwise it will seek to the start
 \arg If \c FlagOverride is given, the file will be truncated
 *    to size 0
 *
 * In case of failure an error code will be returned, otherwise
 * it will return true.
 */
SHVBool SHVFileBase::Open(const SHVStringC& fileName, int flags)
{
SHVBool retVal( (flags&FlagRead) || (flags&FlagWrite) ? (int)SHVBool::True : (int)ErrInvalidParams );

	Close();

	// check for file state
	if (retVal)
	{
	bool exists = SHVDir::FileExist(fileName);

		if (exists && !(flags&FlagOpen))
			retVal.SetError(ErrExists);
		else if (!exists && !(flags&FlagCreate))
			retVal.SetError(ErrMissingFile);
	}

	if (retVal)
	{
	SHVString mode;

		if (flags&FlagOverride)
			mode += _S("w");
		else if ( (flags&FlagWrite) )
			mode += _S("a");
		else if (flags&FlagRead)
			mode += _S("r");

		if ( (flags&FlagRead) && (flags&FlagWrite) )
			mode += _S("+b");
		else
			mode += _S("b");

#if defined(UNICODE)
		File = wfopen((const wchar_t*)fileName.GetSafeBuffer(),(const wchar_t*)mode.GetSafeBuffer());
#elif defined(__SHIVA_WIN32) && defined(UNICODE)
		File = _tfopen(fileName.GetSafeBuffer(),mode.GetSafeBuffer());
#elif defined(FSUTF8MODE)
		File = fopen(fileName.ToStrUTF8().GetSafeBuffer(),mode.GetSafeBuffer());
#else
		File = fopen(fileName.GetSafeBuffer(),mode.GetSafeBuffer());
#endif

		if (!File)
			retVal.SetError(ErrGeneric);
		else
		if (flags&FlagWrite)
		{
			SetPosition(GetSize());
		}
	}

	if (retVal) // are we all done ?
	{
		Flags = flags;
		FileName = fileName;
	}

	return retVal;
}

/*************************************
 * Flush
 *************************************/
/// Flushes the internal file buffer to disk
void SHVFileBase::Flush()
{
	if (IsOpen())
	{
		fflush(File);
	}
}

/*************************************
 * Close
 *************************************/
/// Flushes and closes the file
void SHVFileBase::Close()
{
	if (IsOpen())
	{
		Flags = 0;
		FileName.SetToNull();

		fflush(File);
		fclose(File);
		File = NULL;
	}
}

/*************************************
 * Read
 *************************************/
/// Reads contents from current position
/**
 \param buffer Buffer to read content into
 \param bufferLen Length of buffer in bytes
 \return Number of bytes read
 */
SHVFilePos SHVFileBase::Read(void* buffer, SHVFilePos bufferLen)
{
SHVFilePos retVal;

	if (IsOpen() && (Flags&FlagRead))
	{
		retVal = (SHVFilePos)fread(buffer,1,bufferLen,File);
	}
	else
		retVal = InvalidPosition;

	return retVal;
}

/*************************************
 * Write
 *************************************/
/// Writes contents of a buffer to the file
/**
 \param buffer Pointer to the contents to write
 \param bufferLen Length of buffer
 \return Success
 *
 \bug It is currently unknown what will happen if you write to anything but end of file.
 */
bool SHVFileBase::Write(const void* buffer, SHVFilePos bufferLen)
{
bool retVal;

	if (IsOpen() && (Flags&FlagWrite))
		retVal = ( fwrite(buffer,1,bufferLen,File) ? true : false );
	else
		retVal = false;

	return retVal;
}
