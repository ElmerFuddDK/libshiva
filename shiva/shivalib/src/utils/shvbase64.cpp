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
#include "../../../include/utils/shvbase64.h"


//=========================================================================================================
// SHVBase64Encoder class
//=========================================================================================================
/// \class SHVBase64Encoder shvbase64.h "shiva/include/utils/shvbase64.h"

/*************************************
 * Constructor
 *************************************/
SHVBase64Encoder::SHVBase64Encoder()
{
	Initialized = false;
}

/*************************************
 * Destructor
 *************************************/
SHVBase64Encoder::~SHVBase64Encoder()
{
}

/*************************************
 * Initialize
 *************************************/
void SHVBase64Encoder::Initialize()
{
	Data.RemoveAll();
	Initialized = true;
	RemainingDataBuffer = 0;
	WorksetIndex = 0;
}

/*************************************
 * IsInitialized
 *************************************/
bool SHVBase64Encoder::IsInitialized()
{
	return Initialized;
}

/*************************************
 * Calculate
 *************************************/
void SHVBase64Encoder::Calculate(const SHVBufferC* buffer)
{
	if (!Initialized)
		Initialize();

	if (buffer && buffer->GetSize())
	{
	const SHVByte* byte = buffer->GetBufferConst();
		for (size_t i=0; i<buffer->GetSize();i++)
		{
			Workset[WorksetIndex++] = (unsigned char)byte[i];
			if (WorksetIndex == 3)
			{
				if (RemainingDataBuffer < 4)
				{
					Data.AddTail(NULL);
					Data.GetLast().SetBufferSize(RemainingDataBuffer = 512);
				}
				CommitWorkset();
			}
		}
	}
}

/*************************************
 * GetDataAsString
 *************************************/
SHVStringBuffer8 SHVBase64Encoder::GetDataAsString(int lineWrap, const SHVString8C lineToken)
{
size_t bufLen = 0;
SHVListIterator<SHVString8,const SHVString8C> itr(Data);
SHVString8 retVal;

	if ( lineWrap < 0 || (lineWrap > 0 && lineToken.IsEmpty()) )
		lineWrap = 0;

	Finalize();

	while (itr.MoveNext())
		bufLen += itr.Get().GetBufferLen();

	if (bufLen)
	{
	SHVChar* index;
	size_t len;

		if (lineWrap)
			bufLen += (bufLen/lineWrap) * lineToken.GetLength();

		retVal.SetBufferSize(bufLen+1);
		index = retVal.GetBuffer();

		if (lineWrap)
		{
		size_t lineWrapMarker = (size_t)lineWrap;
		const SHVChar* buf;
		size_t lineTokenLen = lineToken.GetLength();

			while (itr.MoveNext())
			{
				buf = itr.Get().GetBufferConst();
				len = itr.Get().GetBufferLen();
				while (len >= lineWrapMarker)
				{
					if (lineWrapMarker)
					{
						memcpy(index,buf,lineWrapMarker);
						index += lineWrapMarker;
						buf += lineWrapMarker;
						len -= lineWrapMarker;
					}
					memcpy(index,lineToken.GetBufferConst(),lineTokenLen);
					index += lineTokenLen;
					lineWrapMarker = (size_t)lineWrap;
				}
				if (len)
				{
					memcpy(index,buf,len);
					index += len;
					lineWrapMarker -= len;
				}
			}
		}
		else
		{
			while (itr.MoveNext())
			{
				memcpy(index,itr.Get().GetBufferConst(),len = itr.Get().GetBufferLen());
				index += len;
			}
		}

		retVal[bufLen] = '\0';
	}

	return retVal.ReleaseBuffer();
}

/*************************************
 * GetDataAsBuffer
 *************************************/
SHVBuffer* SHVBase64Encoder::GetDataAsBuffer()
{
size_t bufLen = 0;
SHVListIterator<SHVString8,const SHVString8C> itr(Data);
SHVBuffer* retVal = new SHVBufferPtr();

	Finalize();

	while (itr.MoveNext())
		bufLen += itr.Get().GetBufferLen();

	if (bufLen)
	{
	SHVBufferIterator bufItr(*retVal);

		retVal->SetBufferSize(bufLen);

		while (itr.MoveNext())
		{
			SHVVERIFY( bufItr.WriteBytes((const SHVByte*)itr.Get().GetBufferConst(), itr.Get().GetBufferLen()) );
		}
	}

	return retVal;
}

///\cond INTERNAL
/*************************************
 * CommitWorkset
 *************************************/
void SHVBase64Encoder::CommitWorkset()
{
SHVChar* index;
static const SHVChar table[] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
								'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
								'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
								'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'};

	SHVASSERT(RemainingDataBuffer >= 4);

	index = Data.GetLast().GetBuffer()+(Data.GetLast().GetBufferLen()-RemainingDataBuffer);
	index[0] = table[  (Workset[0]&0xFC) >> 2 ];
	index[1] = table[ ((Workset[0]&0x03) << 4) + ((Workset[1]&0xF0) >> 4) ];
	index[2] = table[ ((Workset[1]&0x0F) << 2) + ((Workset[2]&0xC0) >> 6) ];
	index[3] = table[  (Workset[2]&0x3F) ];

	WorksetIndex = 0;
	RemainingDataBuffer -= 4;
}

/*************************************
 * Finalize
 *************************************/
void SHVBase64Encoder::Finalize()
{
	if (Initialized)
	{
		Initialized = false;

		if (WorksetIndex)
		{
		char equalchars = 3-WorksetIndex;

			if (RemainingDataBuffer < 4)
			{
				Data.AddTail(NULL);
				Data.GetLast().SetBufferSize(RemainingDataBuffer = 4);
			}

			for (int i=WorksetIndex; i<3; i++)
				Workset[i] = 0;
			CommitWorkset();

			for ( ;equalchars > 0; equalchars--)
			{
				*(Data.GetLast().GetBuffer() + Data.GetLast().GetBufferLen() - RemainingDataBuffer - equalchars) = '=';
			}

		}

		if (RemainingDataBuffer)
		{
		SHVString8 tmpStr;
			tmpStr.AddChars( Data.GetLast().GetBufferConst(), Data.GetLast().GetBufferLen()-RemainingDataBuffer );
			Data.GetLast() = tmpStr.ReleaseBuffer();
			RemainingDataBuffer = 0;
		}
	}
}
///\endcond



//=========================================================================================================
// SHVBase64Decoder class
//=========================================================================================================
/// \class SHVBase64Decoder shvbase64.h "shiva/include/utils/shvbase64.h"

/*************************************
 * Constructor
 *************************************/
SHVBase64Decoder::SHVBase64Decoder()
{
	Initialized = false;
}

/*************************************
 * Destructor
 *************************************/
SHVBase64Decoder::~SHVBase64Decoder()
{
}

/*************************************
 * GetError
 *************************************/
SHVBool SHVBase64Decoder::GetError()
{
	return Error;
}

/*************************************
 * Initialize
 *************************************/
void SHVBase64Decoder::Initialize()
{
	Data = new SHVBufferCStream();
	Error.SetError(ErrNone);
	Initialized = true;
	RemainingDataBuffer = 0;
	WorksetIndex = 0;
	WorkingBuffer = NULL;
}

/*************************************
 * IsInitialized
 *************************************/
bool SHVBase64Decoder::IsInitialized()
{
	return Initialized;
}

/*************************************
 * Calculate
 *************************************/
SHVBool SHVBase64Decoder::Calculate(const SHVBufferC* buffer)
{
	if (!Initialized)
		Initialize();

	if (!Error)
		return Error;

	if (buffer && buffer->GetSize())
	{
	const SHVByte* byte = buffer->GetBufferConst();
		for (size_t i=0; i<buffer->GetSize() && Error; i++)
		{
			if (byte[i] != '=' && byte[i] != '\r' && byte[i] != '\n' && byte[i] != ' ' && byte[i] != '\t')
			{
				Workset[WorksetIndex++] = (unsigned char)byte[i];
				if (WorksetIndex == 4)
				{
					if (RemainingDataBuffer < 3)
					{
						if (!WorkingBuffer.IsNull())
							Data->AddBuffer(WorkingBuffer);
						WorkingBuffer = new SHVBufferPtr();
						WorkingBuffer->SetBufferSize(RemainingDataBuffer = 3*128);
					}
					CommitWorkset();
				}
			}
		}
	}

	return Error;
}

/*************************************
 * GetData
 *************************************/
SHVBuffer* SHVBase64Decoder::GetData()
{
size_t bufLen = 0;
SHVBuffer* retVal = new SHVBufferPtr();

	if (!Error)
		return retVal;

	Finalize();

	bufLen = Data->GetSize();

	if (bufLen)
	{
	size_t pos = 0;

		retVal->SetBufferSize(bufLen);
		SHVVERIFY( Data->ReadBytes(retVal->GetBuffer(),bufLen,pos) );
	}

	return retVal;
}

///\cond INTERNAL
/*************************************
 * CommitWorkset
 *************************************/
void SHVBase64Decoder::CommitWorkset()
{
size_t pos;
size_t byteCount = WorksetIndex - 1;
static const unsigned char table[] = {0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 000-015
									  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 016-031
									  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,  62,0xFF,0xFF,0xFF,  63, // 032-047
										52,  53,  54,  55,  56,  57,  58,  59,  60,  61,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 048-063
									  0xFF,   0,   1,   2,   3,   4,   5,   6,   7,   8,   9,  10,  11,  12,  13,  14, // 064-079
										15,  16,  17,  18,  19,  20,  21,  22,  23,  24,  25,0xFF,0xFF,0xFF,0xFF,0xFF, // 080-095
									  0xFF,  26,  27,  28,  29,  30,  31,  32,  33,  34,  35,  36,  37,  38,  39,  40, // 096-111
										41,  42,  43,  44,  45,  46,  47,  48,  49,  50,  51,0xFF,0xFF,0xFF,0xFF,0xFF, // 112-127
									  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 128-143
									  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 144-159
									  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 160-175
									  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 176-191
									  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 192-207
									  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 208-223
									  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF, // 224-239
									  0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF};// 240-255

	SHVASSERT(RemainingDataBuffer >= 3);
	SHVASSERT(WorksetIndex == 4 || !Initialized); // Either we have a full set, or are initializing
	SHVASSERT(byteCount > 0 && byteCount < 4);

	pos = WorkingBuffer->GetSize() - RemainingDataBuffer;

	// Decode the chars
	for (int i=0; i<WorksetIndex; i++)
	{
		Workset[i] = table[ Workset[i] ];
		if (Workset[i] == 0xFF)
		{
			Error.SetError(ErrInvalidChar);
			return;
		}
	}

	if (byteCount > 0)
		WorkingBuffer->WriteChar( (Workset[0] << 2) + ((Workset[1]&0x30) >> 4), pos );
	if (byteCount > 1)
		WorkingBuffer->WriteChar( ((Workset[1]&0x0F) << 4) + ((Workset[2]&0x3C) >> 2), pos );
	if (byteCount > 2)
		WorkingBuffer->WriteChar( ((Workset[2]&0x03) << 6) + Workset[3], pos );

	WorksetIndex = 0;
	RemainingDataBuffer -= byteCount;
}

/*************************************
 * Finalize
 *************************************/
void SHVBase64Decoder::Finalize()
{
	if (Initialized)
	{
		Initialized = false;

		if (WorksetIndex)
		{
			if (RemainingDataBuffer < 3)
			{
				if (!WorkingBuffer.IsNull())
					Data->AddBuffer(WorkingBuffer);
				WorkingBuffer = new SHVBufferPtr();
				WorkingBuffer->SetBufferSize(RemainingDataBuffer = 3*128);
			}

			CommitWorkset();
		}

		if (!WorkingBuffer.IsNull())
		{
			if (RemainingDataBuffer)
			{
			size_t newSize = WorkingBuffer->GetSize() - RemainingDataBuffer;
				WorkingBuffer->SetBufferSize( newSize, newSize );
			}
			Data->AddBuffer(WorkingBuffer);
			WorkingBuffer = NULL;
			RemainingDataBuffer = 0;
		}
	}
}
///\endcond
