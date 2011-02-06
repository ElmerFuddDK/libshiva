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
// SHVBuffer class - Buffer handling
//=========================================================================================================
/// \class SHVBuffer shvbuffer.h "shiva/include/utils/shvbuffer.h"

/*************************************
 * Write functions
 *************************************/
SHVBool SHVBuffer::WriteChar(const char c, size_t& pos)
{ return WriteBytes((SHVByte*)&c,sizeof(char),pos); }
SHVBool SHVBuffer::WriteShort(const short s, size_t& pos)
{ return WriteBytes((SHVByte*)&s,sizeof(short),pos); }
SHVBool SHVBuffer::WriteInt(const int i, size_t& pos)
{ return WriteBytes((SHVByte*)&i,sizeof(int),pos); }
SHVBool SHVBuffer::WriteSHVInt(const SHVInt i, size_t& pos)
{ return WriteBytes((SHVByte*)&i,sizeof(SHVInt),pos); }
SHVBool SHVBuffer::WriteSHVDouble(const SHVDouble i, size_t& pos)
{ return WriteBytes((SHVByte*)&i,sizeof(SHVDouble),pos); }
SHVBool SHVBuffer::WriteNullString8(const SHVString8C str, size_t& pos)
{
size_t strLen = str.GetLength()+1;
	return WriteBytes((SHVByte*)str.GetBufferConst(),strLen,pos);
}
SHVBool SHVBuffer::WriteNullString16(const SHVString16C str, size_t& pos)
{
size_t strLen = str.GetLength()+1;
	return WriteBytes((SHVByte*)str.GetBufferConst(),strLen*2,pos);
}
SHVBool SHVBuffer::WriteString8(const SHVString8C str, size_t& pos)
{
size_t strLen = str.GetLength();
	return WriteBytes((SHVByte*)str.GetBufferConst(),strLen,pos);
}
SHVBool SHVBuffer::WriteString16(const SHVString16C str, size_t& pos)
{
size_t strLen = str.GetLength();
	return WriteBytes((SHVByte*)str.GetBufferConst(),strLen*2,pos);
}
SHVBool SHVBuffer::WriteBytes(const SHVByte* buf, size_t bufLen, size_t& pos)
{
SHVByte* buffer = Buffer();
size_t sz = Size();

	if (pos > sz || (pos+bufLen)>sz)
		return SHVBufferC::ErrEof;
	
	memcpy(buffer+pos,buf,bufLen);
	pos += bufLen;
	
	return SHVBool::True;
}

/*************************************
 * SetBufferSize
 *************************************/
/// Sets the buffer size internally on supported buffer types
/**
 \param newSize The new size of the buffer
 \param toCopyFromOld The amount of bytes to copy over from the old
 *
 * This function will set the buffer size and optionally copy over
 * bytes from the old buffer.\n
 * On unsupported buffer types it will return SHVBufferC::ErrNotSupported
 * and in case the memory allocation fails it will return SHVBufferC::ErrAlloc.\n
 * In case of toCopyFromOld being larger than the old buffer size the
 * entire old buffer will be copied. Eg. if the old buffer was 10 bytes, and
 * toCopyFromOld was 11 the entire 10 bytes will be copied over.\n
 * Similar if the newSize is smaller than the amount of bytes to copy the
 * amount of bytes being copied are truncated to the new size.
 */
SHVBool SHVBuffer::SetBufferSize(size_t newSize, size_t toCopyFromOld)
{
	SHVUNUSED_PARAM(newSize);
	SHVUNUSED_PARAM(toCopyFromOld);
	return SHVBufferC::ErrNotSupported;
}


///\cond INTERNAL
/*************************************
 * AllocBuffer
 *************************************/
SHVByte* SHVBuffer::AllocBuffer(size_t len)
{
	return (SHVByte*)malloc(len);
}

/*************************************
 * DestroyBuffer
 *************************************/
void SHVBuffer::DestroyBuffer(SHVByte* buf)
{
	if (buf)
		free(buf);
}
///\endcond


//=========================================================================================================
// SHVBufferPtr class - Contains a heap allocated buffer
//=========================================================================================================
/// \class SHVBufferPtr shvbuffer.h "shiva/include/utils/shvbuffer.h"

/*************************************
 * Constructor
 *************************************/
SHVBufferPtr::SHVBufferPtr() : SHVBuffer(), BufferPtr(NULL), BufferSize(0)
{}
SHVBufferPtr::SHVBufferPtr(const SHVBufferPtr& str) : SHVBuffer(), BufferPtr(NULL), BufferSize(0)
{
	Add(str);
}

/*************************************
 * Destructor
 *************************************/
SHVBufferPtr::~SHVBufferPtr()
{
	DestroyBuffer(BufferPtr);
	BufferPtr = NULL;
}

// Buffer operations
/*************************************
 * SetBufferSize
 *************************************/
SHVBool SHVBufferPtr::SetBufferSize(size_t newSize, size_t toCopyFromOld)
{
SHVByte* oldBuf = BufferPtr;
size_t copySize = (BufferSize < toCopyFromOld ? BufferSize : toCopyFromOld);
SHVBool retVal(SHVBool::True);
	
	// test if the new buffer is smaller than what we want to copy - then truncate
	if (copySize > newSize)
		copySize = newSize;
	
	BufferSize = newSize;
	if (newSize == 0)
	{
		BufferPtr = NULL;
	}
	else
	{
		BufferPtr = AllocBuffer(newSize);
		if (BufferPtr == NULL)
		{
			BufferPtr = oldBuf;
			return SHVBufferC::ErrAlloc;
		}
		else if (copySize > 0)
		{
			memcpy(BufferPtr,oldBuf,copySize);
		}
	}
	
	DestroyBuffer(oldBuf);
	
	return SHVBool::True;
}

/*************************************
 * Expand
 *************************************/
SHVBool SHVBufferPtr::Expand(int expandBy)
{
	if ( (expandBy < 0 && ((size_t)(-expandBy) > BufferSize)) ||
	     (expandBy > 0 && ((size_t)(expandBy+BufferSize) < BufferSize)) )
	{
		return SHVBufferC::ErrGeneric;
	}
	
	return SetBufferSize(expandBy+BufferSize,BufferSize);
}

/*************************************
 * Add
 *************************************/
SHVBool SHVBufferPtr::Add(const SHVBufferC& buffer)
{
size_t oldSize = BufferSize;
SHVBool retVal;
	
	retVal = SetBufferSize(BufferSize+buffer.GetSize(),BufferSize);
	
	if (retVal)
	{
		memcpy(BufferPtr+oldSize,buffer.GetBufferConst(),buffer.GetSize());
	}
	
	return retVal;
}

///\cond INTERNAL
/*************************************
 * Buffer operator
 *************************************/
SHVByte* SHVBufferPtr::Buffer() const
{
	return (SHVByte*)BufferPtr;
}

/*************************************
 * Size operator
 *************************************/
size_t SHVBufferPtr::Size() const
{
	return BufferSize;
}
///\endcond
