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
#include "../../../include/utils/shvstringstream.h"


//=========================================================================================================
// SHVStringStream8 class - Streaming of strings
//=========================================================================================================
/// \class SHVStringStream8 shvstringstream.h "shiva/include/utils/shvstringstream.h"

/*************************************
 * Constructor
 *************************************/
/// Constructor
/**
 \param incrementSize Size to increment the buffer with
 *
 * Constructs an empty stream ready for streaming data to.
 */
SHVStringStream8::SHVStringStream8(size_t incrementSize) : Increment(incrementSize)
{
	Buffer = BufferPos = NULL;
	BufferLeft = 0;
	
	// Safety measure in case someone would set increment size to below 16
	SHVASSERT(Increment);
	if (Increment == 0)
		Increment = 512;
	else if (Increment < 16)
		Increment = 16;
}

/*************************************
 * Destructor
 *************************************/
SHVStringStream8::~SHVStringStream8()
{
	if (Buffer)
		::free(Buffer);
}

/*************************************
 * WriteString8
 *************************************/
SHVBool SHVStringStream8::WriteString8(const SHVString8C str)
{
size_t len = str.GetLength();

	if (len)
	{
		EnsureBufferSpaceFree(len);
		::memcpy(BufferPos,str.GetBufferConst(),len);
		BufferPos += len;
		BufferLeft -= len;
	}
	return SHVBool::True;
}

/*************************************
 * WriteString16
 *************************************/
SHVBool SHVStringStream8::WriteString16(const SHVString16C str)
{
const SHVWChar* buf = str.GetSafeBuffer();
size_t converted = 1;

	if (Conv16.IsNull())
		Conv16 = new SHVStringConv(SHVStringConv::Enc16,SHVStringConv::Enc8);
	
	while (*buf && converted)
	{
		EnsureBufferSpaceFree();
		buf = (const SHVWChar*)Conv16->Convert((const SHVByte*)buf,BufferPos,BufferLeft-1,&converted);
		BufferPos += converted;
		BufferLeft -= converted;
	}
	SHVASSERT(*buf == 0);
	
	return *buf == 0;
}

/*************************************
 * WriteStringUTF8
 *************************************/
SHVBool SHVStringStream8::WriteStringUTF8(const SHVStringUTF8C str)
{
const SHVChar* buf = str.GetSafeBuffer();
size_t converted = 1;

	if (ConvUtf8.IsNull())
		ConvUtf8 = new SHVStringConv(SHVStringConv::EncUtf8,SHVStringConv::Enc8);
	
	while (*buf && converted)
	{
		EnsureBufferSpaceFree();
		buf = (const SHVChar*)ConvUtf8->Convert((const SHVByte*)buf,BufferPos,BufferLeft-1,&converted);
		BufferPos += converted;
		BufferLeft -= converted;
	}
	SHVASSERT(*buf == 0);
	
	return *buf == 0;
}

/*************************************
 * AddChars8
 *************************************/
SHVBool SHVStringStream8::AddChars8(const SHVChar* chars, size_t len)
{
	if (len)
	{
		EnsureBufferSpaceFree(len);
		::memcpy(BufferPos,chars,len);
		BufferPos += len;
		BufferLeft -= len;
	}
	return SHVBool::True;
}

/*************************************
 * AddChars16
 *************************************/
SHVBool SHVStringStream8::AddChars16(const SHVWChar* chars, size_t len)
{
size_t converted = 1;

	if (Conv16.IsNull())
		Conv16 = new SHVStringConv(SHVStringConv::Enc16,SHVStringConv::Enc8);
	
	while (len && converted)
	{
		EnsureBufferSpaceFree();
		chars = (const SHVWChar*)Conv16->Convert((const SHVByte*)chars,BufferPos,BufferLeft-1,&converted);
		BufferPos += converted;
		BufferLeft -= converted;
		len--;
	}
	SHVASSERT(len == 0);
	
	return len == 0;
}

/*************************************
 * AddCharsUTF8
 *************************************/
SHVBool SHVStringStream8::AddCharsUTF8(const SHVChar *chars, size_t len)
{
size_t converted = 1;

	if (ConvUtf8.IsNull())
		ConvUtf8 = new SHVStringConv(SHVStringConv::EncUtf8,SHVStringConv::Enc8);
	
	while (len && converted)
	{
		EnsureBufferSpaceFree();
		chars = (const SHVChar*)ConvUtf8->Convert((const SHVByte*)chars,BufferPos,BufferLeft-1,&converted);
		BufferPos += converted;
		BufferLeft -= converted;
		len--;
	}
	SHVASSERT(len == 0);
	
	return len == 0;
}

/*************************************
 * Reset
 *************************************/
void SHVStringStream8::Reset()
{
	BufferLeft += BufferPos - Buffer;
	BufferPos = Buffer;
}

/*************************************
 * Finalize
 *************************************/
void SHVStringStream8::Finalize()
{
	if (BufferPos)
		*BufferPos = 0;
}

/*************************************
 * EnsureBufferSpaceFree
 *************************************/
void SHVStringStream8::EnsureBufferSpaceFree(size_t minChars)
{
	if (BufferLeft < minChars+1) // leave space for \0
	{
	size_t bytesUsed;
	size_t newSize;
	
		bytesUsed =  (BufferPos-Buffer);
		BufferLeft += ( ((minChars+1-BufferLeft+Increment)/Increment)*Increment );
		newSize = bytesUsed + BufferLeft;
		
		Buffer = (SHVChar*)::realloc(Buffer,newSize);
		SHVASSERT(Buffer);
		BufferPos = Buffer + bytesUsed;
	}
}


//=========================================================================================================
// SHVStringStreamUTF8 class - Streaming of strings
//=========================================================================================================
/// \class SHVStringStreamUTF8 shvstringstream.h "shiva/include/utils/shvstringstream.h"

/*************************************
 * Constructor
 *************************************/
/// Constructor
/**
 \param incrementSize Size to increment the buffer with
 *
 * Constructs an empty stream ready for streaming data to.
 */
SHVStringStreamUTF8::SHVStringStreamUTF8(size_t incrementSize) : Increment(incrementSize)
{
	Buffer = BufferPos = NULL;
	BufferLeft = 0;
	
	// Safety measure in case someone would set increment size to below 16
	SHVASSERT(Increment);
	if (Increment == 0)
		Increment = 512;
	else if (Increment < 16)
		Increment = 16;
}

/*************************************
 * Destructor
 *************************************/
SHVStringStreamUTF8::~SHVStringStreamUTF8()
{
	if (Buffer)
		::free(Buffer);
}

/*************************************
 * WriteString8
 *************************************/
SHVBool SHVStringStreamUTF8::WriteString8(const SHVString8C str)
{
const SHVChar* buf = str.GetSafeBuffer();
size_t converted = 1;

	if (Conv8.IsNull())
		Conv8 = new SHVStringConv(SHVStringConv::Enc8,SHVStringConv::EncUtf8);
	
	while (*buf && converted)
	{
		EnsureBufferSpaceFree();
		buf = (const SHVChar*)Conv8->Convert((const SHVByte*)buf,BufferPos,BufferLeft-1,&converted);
		BufferPos += converted;
		BufferLeft -= converted;
	}
	SHVASSERT(*buf == 0);
	
	return *buf == 0;
}

/*************************************
 * WriteString16
 *************************************/
SHVBool SHVStringStreamUTF8::WriteString16(const SHVString16C str)
{
const SHVWChar* buf = str.GetSafeBuffer();
size_t converted = 1;

	if (Conv16.IsNull())
		Conv16 = new SHVStringConv(SHVStringConv::Enc16,SHVStringConv::EncUtf8);
	
	while (*buf && converted)
	{
		EnsureBufferSpaceFree();
		buf = (const SHVWChar*)Conv16->Convert((const SHVByte*)buf,BufferPos,BufferLeft-1,&converted);
		BufferPos += converted;
		BufferLeft -= converted;
	}
	SHVASSERT(*buf == 0);
	
	return *buf == 0;
}

/*************************************
 * WriteStringUTF8
 *************************************/
SHVBool SHVStringStreamUTF8::WriteStringUTF8(const SHVStringUTF8C str)
{
size_t len = SHVString8C(str.GetBufferConst()).GetLength();

	if (len)
	{
		EnsureBufferSpaceFree(len);
		::memcpy(BufferPos,str.GetBufferConst(),len);
		BufferPos += len;
		BufferLeft -= len;
	}
	return SHVBool::True;
}

/*************************************
 * AddChars8
 *************************************/
SHVBool SHVStringStreamUTF8::AddChars8(const SHVChar* chars, size_t len)
{
size_t converted = 1;

	if (Conv8.IsNull())
		Conv8 = new SHVStringConv(SHVStringConv::Enc8,SHVStringConv::EncUtf8);
	
	while (len && converted)
	{
		EnsureBufferSpaceFree();
		chars = (const SHVChar*)Conv8->Convert((const SHVByte*)chars,BufferPos,BufferLeft-1,&converted);
		BufferPos += converted;
		BufferLeft -= converted;
		len--;
	}
	SHVASSERT(len == 0);
	
	return len == 0;
}

/*************************************
 * AddChars16
 *************************************/
SHVBool SHVStringStreamUTF8::AddChars16(const SHVWChar* chars, size_t len)
{
size_t converted = 1;

	if (Conv16.IsNull())
		Conv16 = new SHVStringConv(SHVStringConv::Enc16,SHVStringConv::EncUtf8);
	
	while (len && converted)
	{
		EnsureBufferSpaceFree();
		chars = (const SHVWChar*)Conv16->Convert((const SHVByte*)chars,BufferPos,BufferLeft-1,&converted);
		BufferPos += converted;
		BufferLeft -= converted;
		len--;
	}
	SHVASSERT(len == 0);
	
	return len == 0;
}

/*************************************
 * AddCharsUTF8
 *************************************/
SHVBool SHVStringStreamUTF8::AddCharsUTF8(const SHVChar* chars, size_t len)
{
	if (len)
	{
		EnsureBufferSpaceFree(len);
		::memcpy(BufferPos,chars,len);
		BufferPos += len;
		BufferLeft -= len;
	}
	return SHVBool::True;
}

/*************************************
 * Reset
 *************************************/
void SHVStringStreamUTF8::Reset()
{
	BufferLeft += BufferPos - Buffer;
	BufferPos = Buffer;
}

/*************************************
 * Finalize
 *************************************/
void SHVStringStreamUTF8::Finalize()
{
	if (BufferPos)
		*BufferPos = 0;
}

/*************************************
 * EnsureBufferSpaceFree
 *************************************/
void SHVStringStreamUTF8::EnsureBufferSpaceFree(size_t minChars)
{
	if (BufferLeft < minChars+5) // leave space for \0 and a multibyte string
	{
	size_t bytesUsed;
	size_t newSize;
	
		bytesUsed =  (BufferPos-Buffer);
		BufferLeft += ( ((minChars+1-BufferLeft+Increment)/Increment)*Increment );
		newSize = bytesUsed + BufferLeft;
		
		Buffer = (SHVChar*)::realloc(Buffer,newSize);
		SHVASSERT(Buffer);
		BufferPos = Buffer + bytesUsed;
	}
}


//=========================================================================================================
// SHVStringStream16 class - Streaming of strings
//=========================================================================================================
/// \class SHVStringStream16 shvstringstream.h "shiva/include/utils/shvstringstream.h"

/*************************************
 * Constructor
 *************************************/
/// Constructor
/**
 \param incrementSize Size to increment the buffer with
 *
 * Constructs an empty stream ready for streaming data to.
 */
SHVStringStream16::SHVStringStream16(size_t incrementSize) : Increment(incrementSize)
{
	Buffer = BufferPos = NULL;
	BufferLeft = 0;
	
	// Safety measure in case someone would set increment size to below 16
	SHVASSERT(Increment);
	if (Increment == 0)
		Increment = 512;
	else if (Increment < 16)
		Increment = 16;
}

/*************************************
 * Destructor
 *************************************/
SHVStringStream16::~SHVStringStream16()
{
	if (Buffer)
		::free(Buffer);
}

/*************************************
 * WriteString8
 *************************************/
SHVBool SHVStringStream16::WriteString8(const SHVString8C str)
{
const SHVChar* buf = str.GetSafeBuffer();
size_t converted = 1;

	if (Conv8.IsNull())
		Conv8 = new SHVStringConv(SHVStringConv::Enc8,SHVStringConv::Enc16);
	
	while (*buf && converted)
	{
		EnsureBufferSpaceFree();
		buf = (const SHVChar*)Conv8->Convert((const SHVByte*)buf,BufferPos,BufferLeft-1,&converted);
		BufferPos += converted;
		BufferLeft -= converted;
	}
	SHVASSERT(*buf == 0);
	
	return *buf == 0;
}

/*************************************
 * WriteString16
 *************************************/
SHVBool SHVStringStream16::WriteString16(const SHVString16C str)
{
size_t len = str.GetLength();

	if (len)
	{
		EnsureBufferSpaceFree(len);
		::memcpy(BufferPos,str.GetBufferConst(),len*2);
		BufferPos += len;
		BufferLeft -= len;
	}
	return SHVBool::True;
}

/*************************************
 * WriteStringUTF8
 *************************************/
SHVBool SHVStringStream16::WriteStringUTF8(const SHVStringUTF8C str)
{
const SHVChar* buf = str.GetSafeBuffer();
size_t converted = 1;

	if (ConvUtf8.IsNull())
		ConvUtf8 = new SHVStringConv(SHVStringConv::EncUtf8,SHVStringConv::Enc16);
	
	while (*buf && converted)
	{
		EnsureBufferSpaceFree();
		buf = (const SHVChar*)ConvUtf8->Convert((const SHVByte*)buf,BufferPos,BufferLeft-1,&converted);
		BufferPos += converted;
		BufferLeft -= converted;
	}
	SHVASSERT(*buf == 0);
	
	return *buf == 0;
}

/*************************************
 * AddChars8
 *************************************/
SHVBool SHVStringStream16::AddChars8(const SHVChar* chars, size_t len)
{
size_t converted = 1;

	if (Conv8.IsNull())
		Conv8 = new SHVStringConv(SHVStringConv::Enc8,SHVStringConv::Enc16);
	
	while (len && converted)
	{
		EnsureBufferSpaceFree();
		chars = (const SHVChar*)Conv8->Convert((const SHVByte*)chars,BufferPos,BufferLeft-1,&converted);
		BufferPos += converted;
		BufferLeft -= converted;
		len--;
	}
	SHVASSERT(len == 0);
	
	return len == 0;
}

/*************************************
 * AddChars16
 *************************************/
SHVBool SHVStringStream16::AddChars16(const SHVWChar* chars, size_t len)
{
	if (len)
	{
		EnsureBufferSpaceFree(len);
		::memcpy(BufferPos,chars,len*2);
		BufferPos += len;
		BufferLeft -= len;
	}
	return SHVBool::True;
}

/*************************************
 * AddCharsUTF8
 *************************************/
SHVBool SHVStringStream16::AddCharsUTF8(const SHVChar* chars, size_t len)
{
size_t converted = 1;

	if (ConvUtf8.IsNull())
		ConvUtf8 = new SHVStringConv(SHVStringConv::EncUtf8,SHVStringConv::Enc16);
	
	while (len && converted)
	{
		EnsureBufferSpaceFree();
		chars = (const SHVChar*)ConvUtf8->Convert((const SHVByte*)chars,BufferPos,BufferLeft-1,&converted);
		BufferPos += converted;
		BufferLeft -= converted;
		len--;
	}
	SHVASSERT(len == 0);
	
	return len == 0;
}

/*************************************
 * Reset
 *************************************/
void SHVStringStream16::Reset()
{
	BufferLeft += BufferPos - Buffer;
	BufferPos = Buffer;
}

/*************************************
 * Finalize
 *************************************/
void SHVStringStream16::Finalize()
{
	if (BufferPos)
		*BufferPos = 0;
}

/*************************************
 * EnsureBufferSpaceFree
 *************************************/
void SHVStringStream16::EnsureBufferSpaceFree(size_t minChars)
{
	if (BufferLeft < minChars+1) // leave space for \0
	{
	size_t bytesUsed;
	size_t newSize;
	
		bytesUsed =  (BufferPos-Buffer);
		BufferLeft += ( ((minChars+1-BufferLeft+Increment)/Increment)*Increment );
		newSize = bytesUsed + BufferLeft;
		
		Buffer = (SHVWChar*)::realloc(Buffer,newSize*2);
		SHVASSERT(Buffer);
		BufferPos = Buffer + bytesUsed;
	}
}
