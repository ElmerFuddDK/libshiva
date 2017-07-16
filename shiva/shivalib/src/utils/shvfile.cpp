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

#include "../../../include/utils/shvfile.h"

#define READUNTIL_BUFLEN 50



//=========================================================================================================
// SHVFile class - standard file operations
//=========================================================================================================
/// \class SHVFile shvfile.h "shiva/include/utils/shvfile.h"

/*************************************
 * Constructor
 *************************************/
SHVFile::SHVFile()
{
}

/*************************************
 * Read8
 *************************************/
/// Reads an 8-bit string into a pre-allocated string buffer
/**
 \param str String containing buffer
 \return Success
 \see ReadString
 */
bool SHVFile::ReadString8(SHVString8& str)
{
bool retVal = (IsOpen() && (GetFlags()&FlagRead) && str.GetBufferLen()>1 );

	if (retVal)
	{
	SHVFilePos bytes = SHVFileBase::Read(str.GetBuffer(),SHVFilePos(str.GetBufferLen()-sizeof(SHVChar)));
		str[bytes/sizeof(SHVChar)] = '\0';
		retVal = (bytes ? true : false);
	}

	return retVal;
}

/*************************************
 * Write8
 *************************************/
/// Writes the entire contents of an 8-bit string to the file
/**
 \param str String to write
 \return Success
 \see WriteString
 */
bool SHVFile::WriteString8(const SHVString8C& str)
{
	return SHVFileBase::Write(str.GetBufferConst(),SHVFilePos(str.GetSizeInBytes()));
}

/*************************************
 * ReadLine8
 *************************************/
/// Reads a line from the file into an 8-bit string
/**
 \param str String to contain the line
 \return Success
 \see ReadLine
 */
bool SHVFile::ReadLine8(SHVString8& str)
{
bool retVal = (IsOpen() && (GetFlags()&FlagRead));

	if (retVal)
	{
	char* tmpBuffer;
	SHVChar* buffer;
	SHVFilePos chars = ReadLineReal(tmpBuffer,sizeof(SHVChar));

		buffer = (SHVChar*)tmpBuffer;

		if (buffer)
		{
		size_t len = chars/sizeof(SHVChar);

			buffer[len] = '\0';

			while (len)
			{
				if (buffer[len-1] == '\r' || buffer[len-1] == '\n')
					buffer[--len] = '\0';
				else
					len = 0;
			}

			str = SHVStringBuffer8::Encapsulate(buffer);
		}
		else
		{
			str.SetToNull();
			retVal = false;
		}
	}

	return retVal;
}

/*************************************
 * WriteLine8
 *************************************/
/// Writes the entire contents of an 8-bit string to the file + newline
/**
 \param str String to write
 \return Success
 \see WriteLine
 */
bool SHVFile::WriteLine8(const SHVString8C& str)
{
SHVString8C newLine(SHVFileBase::LineSeparator8());
	return ((str.IsEmpty() ? true : WriteString8(str)) && WriteString8(newLine));
}

#ifndef __SHVSTRING_EXCLUDE_UNICODE
/*************************************
 * ReadUTF8
 *************************************/
/// Reads a UTF8 string into a pre-allocated string buffer
/**
 \param str String containing buffer
 \return Success
 \see ReadString
 */
bool SHVFile::ReadStringUTF8(SHVStringUTF8& str)
{
bool retVal = (IsOpen() && (GetFlags()&FlagRead) && str.GetBufferLen()>1 );

	if (retVal)
	{
	SHVFilePos bytes = SHVFileBase::Read(str.GetBuffer(),SHVFilePos(str.GetBufferLen()-sizeof(SHVChar)));
		str.GetBuffer()[bytes/sizeof(SHVChar)] = '\0';
		retVal = (bytes ? true : false);
	}

	return retVal;
}

/*************************************
 * WriteUTF8
 *************************************/
/// Writes the entire contents of a UTF8 string to the file
/**
 \param str String to write
 \return Success
 \see WriteString
 */
bool SHVFile::WriteStringUTF8(const SHVStringUTF8C& str)
{
	return SHVFileBase::Write(str.GetBufferConst(),SHVFilePos(str.GetSizeInBytes()));
}

/*************************************
 * ReadLine8
 *************************************/
/// Reads a line from the file into a UTF8 string
/**
 \param str String to contain the line
 \return Success
 \see ReadLine
 */
bool SHVFile::ReadLineUTF8(SHVStringUTF8& str)
{
bool retVal = (IsOpen() && (GetFlags()&FlagRead));

	if (retVal)
	{
	char* tmpBuffer;
	SHVChar* buffer;
	SHVFilePos chars = ReadLineReal(tmpBuffer,sizeof(SHVChar));

		buffer = (SHVChar*)tmpBuffer;

		if (buffer)
		{
		size_t len = chars/sizeof(SHVChar);

			buffer[len] = '\0';

			while (len)
			{
				if (buffer[len-1] == '\r' || buffer[len-1] == '\n')
					buffer[--len] = '\0';
				else
					len = 0;
			}

			str = SHVStringBufferUTF8::Encapsulate(buffer);
		}
		else
		{
			str.SetToNull();
			retVal = false;
		}
	}

	return retVal;
}

/*************************************
 * WriteLineUTF8
 *************************************/
/// Writes the entire contents of a UTF8 string to the file + newline
/**
 \param str String to write
 \return Success
 \see WriteLine
 */
bool SHVFile::WriteLineUTF8(const SHVStringUTF8C& str)
{
SHVStringUTF8C newLine(SHVFileBase::LineSeparatorUTF8());
	return ((str.IsEmpty() ? true : WriteStringUTF8(str)) && WriteStringUTF8(newLine));
}

/*************************************
 * Read16
 *************************************/
/// Reads a UCS2 string into a pre-allocated string buffer
/**
 \param str String containing buffer
 \return Success
 \see ReadString
 */
bool SHVFile::ReadString16(SHVString16& str)
{
bool retVal = (IsOpen() && (GetFlags()&FlagRead) && str.GetBufferLen()>1 );

	if (retVal)
	{
	SHVFilePos bytes = SHVFileBase::Read(str.GetBuffer(),SHVFilePos((str.GetBufferLen()-1)*sizeof(SHVWChar)));
		str[bytes/sizeof(SHVWChar)] = '\0';
		retVal = (bytes ? true : false);
	}

	return retVal;
}

/*************************************
 * Write16
 *************************************/
/// Writes the entire contents of a UCS2 string to the file
/**
 \param str String to write
 \return Success
 \see WriteString
 */
bool SHVFile::WriteString16(const SHVString16C& str)
{
	return SHVFileBase::Write(str.GetBufferConst(),SHVFilePos(str.GetSizeInBytes()));
}

/*************************************
 * ReadLine16
 *************************************/
/// Reads a line from the file into a UCS2 string
/**
 \param str String to contain the line
 \return Success
 \see ReadLine
 */
bool SHVFile::ReadLine16(SHVString16& str)
{
bool retVal = (IsOpen() && (GetFlags()&FlagRead));

	if (retVal)
	{
	char* tmpBuffer;
	SHVWChar* buffer;
	SHVFilePos chars = ReadLineReal(tmpBuffer,sizeof(SHVWChar));

		buffer = (SHVWChar*)tmpBuffer;

		if (buffer)
		{
		size_t len = chars/sizeof(SHVWChar);

			buffer[len] = '\0';

			while (len)
			{
				if (buffer[len-1] == '\r' || buffer[len-1] == '\n')
					buffer[--len] = '\0';
				else
					len = 0;
			}

			str = SHVStringBuffer16::Encapsulate(buffer);
		}
		else
		{
			str.SetToNull();
			retVal = false;
		}
	}

	return retVal;
}

/*************************************
 * WriteLine16
 *************************************/
/// Writes the entire contents of a UCs2 string to the file + newline
/**
 \param str String to write
 \return Success
 \see WriteLine
 */
bool SHVFile::WriteLine16(const SHVString16C& str)
{
SHVString16C newLine(SHVFileBase::LineSeparator16());
	return ((str.IsEmpty() ? true : WriteString16(str)) && WriteString16(newLine));
}
#endif

///\cond INTERNAL
/*************************************
 * ReadLineReal
 *************************************/
SHVFilePos SHVFile::ReadLineReal(char*& result, size_t charSize)
{
SHVFilePos retVal;

	result = NULL;

	if (IsOpen() && (GetFlags()&FlagRead))
	{
	size_t bufLen = READUNTIL_BUFLEN*charSize;
	char* buffer = (char*)::malloc(bufLen);
	SHVFilePos oldPos, pos, count;
	bool running = true;
	bool found = false;
	
		oldPos = GetPosition();
	
		// search
		while (running && !found)
		{

			// read the buffer
			count = SHVFileBase::Read(buffer,(SHVFilePos)bufLen);
	
			if (!count) // eof
			{
				running = false;
			}
			else // search for newline
			{
				for (pos = 0; pos<count && !found; pos += (SHVFilePos)charSize)
				{
					found = (buffer[pos] == '\n'); //TODO: Check for endianness at some point
				}

				if (found)
					Seek(pos-count); // move back to just after the newline
			}
		};
	
		pos = GetPosition();
	
		if (pos>oldPos)
		{
			bufLen = size_t(pos-oldPos);
			result = (char*)::malloc(bufLen+charSize);
	
			SetPosition(oldPos); // restore position
			retVal = SHVFileBase::Read(result,(SHVFilePos)bufLen);
		}
		else
			retVal = 0;

		::free(buffer);
	}
	else
		retVal = -1;

	return retVal;
}
///\endcond


// Deocumentation

/** \fn bool SHVFile::ReadString(SHVString& str)
 \brief Reads a string into a pre-allocated string buffer
 \param str String that contains the buffer
 \return Success
 *
 * This function will read a portion of the file into str.
 * It will return false if there is no buffer allocated in str, of if
 * wasm't read anything into the buffer.\n
 * Keep in mind that you need to make room for a null termination in the
 * string. Here's an example:
\code
SHVFile file;
SHVString str;

	// open the file
	file.Open( _S("file.txt") );

	// set the buffer size
	str.SetBufferSize(51); // room for 50 chars

	while (file.ReadString(str))
		printf(str);
\endcode
 */


/** \fn bool SHVFile::WriteString(const SHVStringC& str)
 \brief Writes the entire contents of a string to the file
 \param str String to write
 \return Success
 */


/** \fn bool SHVFile::ReadLine(SHVString& str)
 \brief Reads a line from the file into a string
 \param str String to contain the line
 \return Success
 *
 * Will read up to a newline or EOF, and put it into str. It will strip the
 * endline if present.
 */


/** \fn bool SHVFile::WriteLine(const SHVStringC& str)
 \brief Writes the entire contents of a string to the file + newline
 \param str String to write
 \return Success
 */
