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

#include "../../../include/utils/shvbufferstream.h"


/*************************************
 * Constructor
 *************************************/
SHVBufferStream::SHVBufferStream(size_t chunkSize): ChunkSize(chunkSize), TotalBufferSize(0)
{
}

/*************************************
 * ReadBytes
 *************************************/
SHVBool SHVBufferStream::ReadBytes(SHVByte* buf, size_t bufLen, size_t& pos) const
{
size_t bufIndex;
size_t bufPos;
size_t bufRemainder;
size_t outBufPos = 0;
SHVBool retVal = pos < TotalBufferSize;
	if (retVal)
	{
		bufIndex = pos / ChunkSize;
		bufPos   = pos % ChunkSize;
		bufRemainder = ChunkSize - bufPos;
		if (bufLen + pos > TotalBufferSize)
		{
			retVal = false;
			bufLen  = TotalBufferSize - pos;
		}		
		if (bufRemainder > bufLen)
			bufRemainder = bufLen;
		while (bufLen)
		{
			Buffers[bufIndex++]->ReadBytes(buf + outBufPos, bufRemainder, bufPos);
			outBufPos += bufRemainder;
			pos += bufRemainder;
			bufPos = 0;
			bufLen -= bufRemainder;
			bufRemainder = (bufLen >= ChunkSize ? ChunkSize : bufLen);
		}
	}
	return retVal;
}


/*************************************
 * WriteBytes
 *************************************/
SHVBool SHVBufferStream::WriteBytes(const SHVByte* buf, size_t bufLen, size_t& pos)
{
size_t bufIndex;
size_t bufPos;
size_t bufRemainder;
size_t inBufPos = 0;
	
	if (pos + bufLen > TotalBufferSize)
		SetBufferSize(pos + bufLen);

	bufIndex = pos / ChunkSize;
	bufPos   = pos % ChunkSize;
	bufRemainder = ChunkSize - bufPos;
	if (bufRemainder > bufLen)
		bufRemainder = bufLen;
	if (pos > TotalBufferSize)
		TotalBufferSize = (pos + bufLen);
	while (bufLen)
	{
		Buffers[bufIndex++]->WriteBytes(buf + inBufPos, bufRemainder, bufPos);
		inBufPos += bufRemainder;
		pos += bufRemainder;
		bufLen -= bufRemainder;
		bufPos = 0;
		bufRemainder = (bufLen > ChunkSize ? ChunkSize : bufLen);
	}
	return SHVBool::True;
}

/*************************************
 * SetBufferSize
 *************************************/
SHVBool SHVBufferStream::SetBufferSize(size_t newSize, size_t toCopyFromOld)
{
size_t neededBuffers = (newSize / ChunkSize) + (newSize % ChunkSize ? 1 : 0);
	while (neededBuffers < Buffers.GetCount())
	{
		Buffers.Remove(Buffers.GetCount() - 1);
	}
	while (neededBuffers > Buffers.GetCount())
	{
	SHVBufferPtr* buf = new SHVBufferPtr();
		buf->SetBufferSize(ChunkSize);
		Buffers.Add(buf);
	}
	TotalBufferSize = newSize;
	return SHVBool::True;
}

/*************************************
 * Buffer
 *************************************/
SHVByte* SHVBufferStream::Buffer() const
{
	return NULL;
}

/*************************************
 * Size
 *************************************/
size_t SHVBufferStream::Size() const
{
	return TotalBufferSize;
}

/*************************************
 * Size
 *************************************/
size_t SHVBufferStream::Seek(SHVByte* seekBuf, size_t bufSize, size_t startPos) const
{
size_t sz = Size();
bool running = true, comparing = true;
SHVBool err(SHVBool::True);
size_t i;
size_t tmp;
SHVByte b;

	for (i=0,tmp=startPos; startPos<sz && running && err; )
	{
		err = ReadBytes(&b,sizeof(SHVByte),tmp);
		comparing = comparing && (b == *(SHVByte*)(seekBuf+i));
		
		i++;
		if (!(i%bufSize))
		{
			i=0;
			if (comparing)
				running = false;
			else
				startPos+=bufSize;
			comparing = true;
		}
	}
	return (running || !err ? sz : startPos);
}
