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
#include "../../../include/utils/shvbuffer.h"

#ifdef __SHIVA_EPOC
# include <string.h>
#endif


//=========================================================================================================
// SHVBufferC class - Constant buffer routines
//=========================================================================================================
/// \class SHVBufferC shvbufferc.h "shiva/include/utils/shvbufferc.h"

/*************************************
 * Constructor
 *************************************/
SHVBufferC::operator SHVHashValue() const
{
SHVByte* buf = Buffer();
size_t sz = Size();
size_t retVal = 0;
	for(size_t i = 0; i < sz && buf; i++)
		retVal = buf[i] + retVal * 5;
	return retVal;
}

/*************************************
 * Read functions
 *************************************/
/// Reads a char
/**
 \param c Result
 \param pos Buffer position
 \return True on success
 *
 * For an example see \ref SHVBufferCPtr
 */
SHVBool SHVBufferC::ReadChar(char& c, size_t& pos) const
{ return ReadBytes((SHVByte*)&c,sizeof(char),pos); }
/// Reads a short
/**
 \param s Result
 \param pos Buffer position
 \return True on success
 *
 * For an example see \ref SHVBufferCPtr
 */
SHVBool SHVBufferC::ReadShort(short& s, size_t& pos) const
{ return ReadBytes((SHVByte*)&s,sizeof(short),pos); }
/// Reads an integer
/**
 \param i Result
 \param pos Buffer position
 \return True on success
 *
 * For an example see \ref SHVBufferCPtr
 */
SHVBool SHVBufferC::ReadInt(int& i, size_t& pos) const
{ return ReadBytes((SHVByte*)&i,sizeof(int),pos); }
/// Reads a SHIVA integer
/**
 \param i Result
 \param pos Buffer position
 \return True on success
 *
 * For an example see \ref SHVBufferCPtr
 */
SHVBool SHVBufferC::ReadSHVInt(SHVInt& i, size_t& pos) const
{ return ReadBytes((SHVByte*)&i,sizeof(SHVInt),pos); }
/// Reads a SHIVA double
/**
 \param d Result
 \param pos Buffer position
 \return True on success
 *
 * For an example see \ref SHVBufferCPtr
 */
SHVBool SHVBufferC::ReadSHVDouble(SHVDouble& d, size_t& pos) const
{ return ReadBytes((SHVByte*)&d,sizeof(SHVDouble),pos); }
/// Reads a null terminated 8bit string
/**
 \param str Result
 \param pos Buffer position
 \return True on success
 *
 * For an example see \ref SHVBufferCPtr
 */
SHVBool SHVBufferC::ReadNullString8(SHVString8& str, size_t& pos) const
{
SHVBool retVal;
size_t newPos = SeekByte('\0', pos);

	if (newPos >= Size())
		return SHVBufferC::ErrEof;
	
	retVal = ReadString8(str, newPos-pos, pos);
	
	if (retVal)
		pos++;
	
	return retVal;
}
/// Reads a null terminated UCS2 string
/**
 \param str Result
 \param pos Buffer position
 \return True on success
 *
 * For an example see \ref SHVBufferCPtr
 */
SHVBool SHVBufferC::ReadNullString16(SHVString16& str, size_t& pos) const
{
SHVBool retVal;
size_t newPos = SeekShort(0, pos);

	if (newPos >= Size())
		return SHVBufferC::ErrEof;
	
	retVal = ReadString16(str, (newPos-pos)/2, pos);
	
	if (retVal)
		pos += 2;
	
	return retVal;
}
/// Reads an 8bit string with a provided length
/**
 \param str Result
 \param strLen Length of string in characters
 \param pos Buffer position
 \return True on success
 *
 * The string will be terminated after read.\n
 * For an example see \ref SHVBufferCPtr
 */
SHVBool SHVBufferC::ReadString8(SHVString8& str, size_t strLen, size_t& pos) const
{
SHVBool retVal(SHVBool::True);

	if (str.GetBufferLen() < strLen+1)
		str.SetBufferSize(strLen+1);
	
	if (str.GetBufferLen() < strLen+1) // if it is still too small (allocation failed)
	{
		return SHVBufferC::ErrAlloc;
	}
	else
	{
		retVal = ReadBytes((SHVByte*)str.GetBuffer(),strLen,pos);
		str[strLen] = '\0';
	}
	
	return retVal;
}
/// Reads a UCS2 string with a provided length
/**
 \param str Result
 \param strLen Length of string in characters
 \param pos Buffer position
 \return True on success
 *
 * The string will be terminated after read. Keep in mind that the
 * provided length is in chars, not bytes.\n
 * For an example see \ref SHVBufferCPtr
 */
SHVBool SHVBufferC::ReadString16(SHVString16& str, size_t strLen, size_t& pos) const
{
SHVBool retVal(SHVBool::True);

	if (str.GetBufferLen() < strLen+1)
		str.SetBufferSize(strLen+1);
	
	if (str.GetBufferLen() < strLen+1) // if it is still too small (allocation failed)
	{
		return SHVBufferC::ErrAlloc;
	}
	else
	{
		retVal = ReadBytes((SHVByte*)str.GetBuffer(),strLen*2,pos);
		str[strLen] = '\0';
	}
	
	return retVal;
}
/// Reads a number of bytes into a buffer
/**
 \param buf Result
 \param bufLen number of bytes to read
 \param pos Buffer position
 \return True on success
 *
 * For an example see \ref SHVBufferCPtr
 */
SHVBool SHVBufferC::ReadBytes(SHVByte* buf, size_t bufLen, size_t& pos) const
{
size_t sz = Size();
	if (pos > sz || (pos+bufLen)>sz)
		return SHVBufferC::ErrEof;
	
	memcpy(buf,Buffer()+pos,bufLen);
	pos += bufLen;
	
	return SHVBool::True;
}

/*************************************
 * Search functions
 *************************************/
size_t SHVBufferC::SeekByte(SHVByte b, size_t startPos) const
{
	return Seek(&b,sizeof(SHVByte),startPos);
}
size_t SHVBufferC::SeekShort(short s, size_t startPos) const
{
	return Seek((SHVByte*)&s,sizeof(short),startPos);
}
///\cond INTERNAL
size_t SHVBufferC::Seek(SHVByte* seekBuf, size_t bufSize, size_t startPos) const
{
SHVByte* buf = Buffer();
size_t sz = Size();
bool running = true;
size_t i;


	for (;startPos<sz;startPos+=bufSize)
	{
		switch (bufSize)
		{
		case 1:
			running = *((SHVByte*)(buf+startPos)) != *(SHVByte*)seekBuf;
			break;
		case 2:
			running = *((short*)(buf+startPos)) != *(short*)seekBuf;
			break;
		default:
			for (i=0;i<bufSize;i++) if (*((SHVByte*)(buf+startPos)) != *(SHVByte*)(seekBuf+i)) break;
			running = (i<bufSize);
			break;
		}
		
		if (!running)
			break;
	}
	return (running ? sz : startPos);
}
///\endcond


//=========================================================================================================
// SHVBufferCPtr class - wrapper for non BufferC stuff
//=========================================================================================================
/// \class SHVBufferCPtr shvbufferc.h "shiva/include/utils/shvbufferc.h"

///\cond INTERNAL
/*************************************
 * Buffer operator
 *************************************/
SHVByte* SHVBufferCPtr::Buffer() const
{
	return (SHVByte*)BufferPtr;
}

/*************************************
 * Size operator
 *************************************/
size_t SHVBufferCPtr::Size() const
{
	return BufferSize;
}
///\endcond
