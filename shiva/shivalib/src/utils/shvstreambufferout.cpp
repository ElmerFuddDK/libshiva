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
#include "../../../include/utils/shvstreambufferout.h"


//=========================================================================================================
// SHVStreamBufferIn class - Stream input interface
//=========================================================================================================
/// \class SHVStreamBufferOut shvstreambufferout.h "shiva/include/utils/shvstreambufferout.h"

/*************************************
 * Constructor
 *************************************/
SHVStreamBufferOut::SHVStreamBufferOut(SHVBuffer* buffer): Buffer(buffer), Position(0)
{
}

SHVStreamBufferOut::SHVStreamBufferOut(): Position(0)
{
	Buffer = new SHVBufferStream();
}

/*************************************
 * IsOk
 *************************************/
SHVBool SHVStreamBufferOut::IsOk() const
{
	return !Buffer.IsNull();
}

/*************************************
 * WriteBuffer
 *************************************/
SHVBool SHVStreamBufferOut::WriteBuffer(const void* buffer, size_t len)
{
	SHVBool retVal = IsOk();
	if (retVal)
	{
		retVal = Buffer->WriteBytes((SHVByte*) buffer, len, Position);
	}
	return retVal;
}

/*************************************
 * WriteString16
 *************************************/
SHVBool SHVStreamBufferOut::WriteString16(const SHVWChar* buffer, size_t maxsize)
{
SHVBool retVal = IsOk();
	if (retVal)
	{
		if (maxsize == SIZE_T_MAX)
			maxsize = SHVString16::StrLenInBytes(buffer);
		retVal = WriteBuffer(buffer, maxsize);
	}
	return retVal;
}

/*************************************
 * WriteChar16
 *************************************/
SHVBool SHVStreamBufferOut::WriteChar16(SHVWChar ch)
{
	return WriteBuffer(&ch, sizeof(SHVWChar));
}

/*************************************
 * WriteString8
 *************************************/
SHVBool SHVStreamBufferOut::WriteString8(const SHVChar* buffer, size_t maxsize)
{
SHVBool retVal = IsOk();
	if (retVal)
	{
		if (maxsize == SIZE_T_MAX)
			maxsize = SHVString8::StrLenInBytes(buffer);
		retVal = WriteBuffer(buffer, maxsize);
	}
	return retVal;
}

/*************************************
 * WriteChar8
 *************************************/
SHVBool SHVStreamBufferOut::WriteChar8(const SHVChar ch)
{
	return WriteBuffer(&ch, sizeof(SHVChar));
}

/*************************************
 * WriteStringUTF8
 *************************************/
SHVBool SHVStreamBufferOut::WriteStringUTF8(const SHVChar* buffer, size_t maxsize)
{
SHVBool retVal = IsOk();
	if (retVal)
	{
		if (maxsize == SIZE_T_MAX)
			maxsize = SHVStringUTF8::StrLenInBytes(buffer);
		retVal = WriteBuffer(buffer, maxsize);
	}
	return retVal;
}

/*************************************
 * WriteCharUTF8
 *************************************/
SHVBool SHVStreamBufferOut::WriteCharUTF8(const SHVChar ch)
{
	return WriteBuffer(&ch, sizeof(SHVChar));
}

/*************************************
 * Close
 *************************************/
void SHVStreamBufferOut::Close()
{
	Buffer = NULL;
}

/*************************************
 * Flush
 *************************************/
SHVBool SHVStreamBufferOut::Flush()
{
	return true;
}
