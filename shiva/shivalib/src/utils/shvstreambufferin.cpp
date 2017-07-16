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

#include "../../../include/utils/shvstreambufferin.h"


//=========================================================================================================
// SHVStreamBufferIn class - Stream input interface
//=========================================================================================================
/// \class SHVStreamBufferIn shvstreambufferin.h "shiva/include/utils/shvstreambufferin.h"

/*************************************
 * Constructor
 *************************************/
SHVStreamBufferIn::SHVStreamBufferIn(SHVBufferC* buffer): Buffer(buffer)
{
	Position = 0;
	BufferEof = buffer->GetSize() == 0;
}

/*************************************
 * Eof
 *************************************/
bool SHVStreamBufferIn::Eof() const
{
	return !IsOk() || Position >= Buffer->GetSize();
}

/*************************************
 * IsOk
 *************************************/
SHVBool SHVStreamBufferIn::IsOk() const
{
	return !Buffer.IsNull();
}

/*************************************
 * ReadBuffer
 *************************************/
SHVBool SHVStreamBufferIn::ReadBuffer(void* buffer, size_t bufferSize, size_t& actualLength)
{
size_t remainingBuffer = Buffer->GetSize() - Position;
	if (remainingBuffer < bufferSize)
		bufferSize = remainingBuffer;
	Buffer->ReadBytes((SHVByte*) buffer, bufferSize, Position);
	actualLength = bufferSize;
	return actualLength != 0;
}

/*************************************
 * ReadString16
 *************************************/
SHVBool SHVStreamBufferIn::ReadString16(SHVWChar* buffer, size_t maxsize)
{
SHVWChar ch;
size_t pos = 0;
	if (maxsize)
	{
		do 
		{
			ch = buffer[pos++] = ReadChar16();
		} while (ch && pos < maxsize - 1);
		buffer[pos] = 0;
	}
	return !Eof();
}

/*************************************
 * ReadChar16
 *************************************/
SHVWChar SHVStreamBufferIn::ReadChar16()
{
SHVWChar ch = 0;
size_t aLen;
	ReadBuffer(&ch, sizeof(SHVWChar), aLen);
	if (aLen != sizeof(SHVWChar))
		ch = 0;
	return ch;
}

/*************************************
 * ReadString8
 *************************************/
SHVBool SHVStreamBufferIn::ReadString8(SHVChar* buffer, size_t maxsize)
{
SHVChar ch;
size_t pos = 0;
	if (maxsize)
	{
		do 
		{
			ch = buffer[pos++] = ReadChar8();
		} while (ch && pos < maxsize - 1);
		buffer[pos] = 0;
	}
	return !Eof();
}

/*************************************
 * ReadChar8
 *************************************/
SHVChar SHVStreamBufferIn::ReadChar8()
{
SHVChar ch = 0;
size_t aLen;
	ReadBuffer(&ch, sizeof(SHVChar), aLen);
	if (aLen != sizeof(SHVChar))
		ch = 0;
	return ch;
}

/*************************************
 * ReadStringUTF8
 *************************************/
SHVBool SHVStreamBufferIn::ReadStringUTF8(SHVChar* buffer, size_t maxsize)
{
SHVChar ch;
size_t pos = 0;
	if (maxsize)
	{
		do 
		{
			ch = buffer[pos++] = ReadCharUTF8();
		} while (ch && pos < maxsize - 1);
		buffer[pos] = 0;
	}
	return !Eof();
}

/*************************************
 * ReadCharUTF8
 *************************************/
SHVChar SHVStreamBufferIn::ReadCharUTF8()
{
SHVChar ch = 0;
size_t aLen;
	ReadBuffer(&ch, sizeof(SHVChar), aLen);
	if (aLen != sizeof(SHVChar))
		ch = 0;
	return ch;
}

/*************************************
 * Close
 *************************************/
void SHVStreamBufferIn::Close()
{
	Buffer = NULL;
}

