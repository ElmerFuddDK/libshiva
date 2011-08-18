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

#include "../../../include/utils/shvbuffercstream.h"


//=========================================================================================================
// SHVBufferCStream class - Constant buffer stream class
//=========================================================================================================
/// \class SHVBufferCStream shvbuffercstream.h "shiva/include/utils/shvbuffercstream.h"

/*************************************
 * Constructor
 *************************************/
SHVBufferCStream::SHVBufferCStream()
{
	BufferSize = 0;
}

/*************************************
 * ReadBytes
 *************************************/
SHVBool SHVBufferCStream::ReadBytes(SHVByte* buf, size_t bufLen, size_t& pos) const
{
size_t index;
SHVDynArray<BufferContainer, size_t>* bufferList = (SHVDynArray<BufferContainer, size_t>*)&BufferList;

	if (pos > BufferSize || (pos+bufLen)>BufferSize)
		return SHVBufferC::ErrEof;
	
	if (bufLen == 0)
		return SHVBool::True;
	
	index = bufferList->FindMatchIndex(&pos, &SHVBufferCStream::ListCompare);
	
	if (index < bufferList->GetCount())
	{
	SHVBool retVal(true);
	BufferContainer* container;
	size_t tmpPos, newPos;
	size_t readLen;
		
		newPos = pos + bufLen;
		tmpPos = pos - (*bufferList)[index]->Start;
		
		while (retVal && bufLen && index < bufferList->GetCount())
		{
			container = (*bufferList)[index];
			readLen = container->Buffer->GetSize() - tmpPos;
			readLen = ( bufLen > readLen ? readLen : bufLen );
			
			retVal = container->Buffer->ReadBytes(buf,readLen,tmpPos);
			bufLen -= readLen;
			buf += readLen;
			tmpPos = 0;
			index++;
		}
		
		if (retVal)
			pos = newPos;
	
		return retVal;
	}
	
	return SHVBufferC::ErrGeneric;
}

/*************************************
 * AddBuffer
 *************************************/
SHVBool SHVBufferCStream::AddBuffer(SHVBufferC* buffer)
{
	if (buffer && buffer->GetSize())
	{
	BufferContainer* container = new BufferContainer(buffer,BufferSize);
		
		BufferSize += buffer->GetSize();
		BufferList.Add(container);
	}
	
	return true;
}

/*************************************
 * Truncate
 *************************************/
/// Truncates the internal buffer list, updating the position
/**
 \param pos The position to truncate to
 \return Success
 *
 * This function will remove all the buffers before the given position,
 * And update the position to point to the same location in the remaining
 * buffers.
 */
SHVBool SHVBufferCStream::Truncate(size_t& pos)
{
SHVBool retVal(true);

	// test if everything needs to be removed
	if (pos >= BufferSize)
	{
		BufferList.ClearAndInit();
	}
	// test if we can remove anything
	else if (BufferList.GetCount() > 1 && BufferList[0]->End < pos)
	{
	size_t index, i, j;
	size_t bufferReduction;
		
		// locate the buffer that contains pos
		for (index=1;index<BufferList.GetCount(); index++)
		{
			if (BufferList[index]->End > pos)
				break;
		}
		
		// get the amount of bytes to reduce buffer size with and update pos
		bufferReduction = BufferList[index-1]->End;
		pos -= bufferReduction;
		
		// Remove the redundant buffers
		for (i=0;i<index;i++)
		{
			BufferList.Replace(i,NULL);
		}
		
		// Move the remaining buffers down
		for (j=0; i<BufferList.GetCount(); i++, j++)
		{
			BufferList[i]->Start -= bufferReduction;
			BufferList[i]->End -= bufferReduction;
			BufferList.Swap(i,j);
		}
		
		// Update the internal vars
		BufferList.Truncate();
		BufferSize -= bufferReduction;
	}
	
	return retVal;
}

///\cond INTERNAL
/*************************************
 * Seek
 *************************************/
size_t SHVBufferCStream::Seek(SHVByte* seekBuf, size_t bufSize, size_t startPos) const
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

/*************************************
 * Buffer
 *************************************/
SHVByte* SHVBufferCStream::Buffer() const
{
	return NULL;
}

/*************************************
 * Size
 *************************************/
size_t SHVBufferCStream::Size() const
{
	return BufferSize;
}

/*************************************
 * ListCompare
 *************************************/
int SHVBufferCStream::ListCompare(size_t** a,SHVBufferCStream::BufferContainer** b)
{
size_t* pos = *a;

	if ( *pos < (*b)->Start )
		return -1;
	if ( *pos >= (*b)->End )
		return 1;
	
	return 0;
}
///\endcond
