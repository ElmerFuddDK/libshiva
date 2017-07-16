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
#include "../../../include/utils/shvstringconv.h"
#include "../../../include/utils/shvstring.h"

#ifndef __SHVSTRING_EXCLUDE_UNICODE

#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
# include <f32file.h>
# include <charconv.h>
# define UNICODE_INIT()
#else
# if defined(__SHIVA_USELIBUNICODE) || defined(__SHIVA_LIBUNICODESTATIC)
#  include "../libunicode/unicode.h"
#  define iconv_open unicode_iconv_open
#  define iconv unicode_iconv
#  define iconv_close unicode_iconv_close
#  define iconv_t unicode_iconv_t
#  define ICONV_IBUFTYPE const char**
#  define ICONV_ERR -1
#  ifdef __SHIVA_LIBUNICODESTATIC
#   include "../../../include/threadutils/shvmutexbase.h"
	static void UNICODE_INIT()
	{
	static int initialized = 0;
	static SHVMutexBase initLock;
		if (!initialized)
		{
			initLock.Lock();
			if (!initialized)
			{
				unicode_init();
				initialized++;
			}
			initLock.Unlock();
		}
	}
#  else
#   define UNICODE_INIT()
#  endif
# else
#  include <stdlib.h>
#  include <errno.h>
#  include <iconv.h>
#  ifdef __SHIVA_POSIX_FREEBSD
#   define ICONV_IBUFTYPE const char**
#  else
#   define ICONV_IBUFTYPE char**
#  endif
#  define ICONV_ERR (size_t)-1
#  define UNICODE_INIT()
# endif
const char* SHVStringC_Get8BitCharSet()
{
const char* retVal = getenv("CHARSET");
	if (!retVal || !SHVString8C::StrCmp(retVal,"UTF-8")) //run away, it is UTF8
		retVal = "ISO-8859-1";
	return retVal;
}
#endif

//=========================================================================================================
// SHVStringConv class - Conversion between strings
//=========================================================================================================
/// \class SHVStringConv shvstringconv.h "shiva/include/utils/shvstringconv.h"

/*************************************
 * Constructor
 *************************************/
/// Constructor
/**
 \param from Input encoding
 \param to Output encoding
 *
 * Initializes a conversion routine between from and to.
 */
SHVStringConv::SHVStringConv(Enc from, Enc to) : From(from), To(to)
{
#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
	convTo = convFrom = NULL;
	fs.Connect();
#else
	UNICODE_INIT();
	iconvData = -1;
	
	SHVASSERT(sizeof(iconv_t) == sizeof(iconvData)); // They have to be of same size

#endif
	
	switch (From)
	{
	case Enc8:
		switch (To)
		{
		case Enc16:
#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
			convTo = CCnvCharacterSetConverter::NewLC();
			((CCnvCharacterSetConverter*)convTo)->PrepareToConvertToOrFromL(KCharacterSetIdentifierCodePage1252, fs);
#else
			iconvData = (size_t)iconv_open("UCS-2",SHVStringC_Get8BitCharSet());
		
			if (iconvData == (size_t)-1) // failure, try ascii as fallback
				iconvData = (size_t)iconv_open("UCS-2","ASCII");
		
			if (iconvData == (size_t)-1)
				fprintf(stderr, "local->UNICODE conversion error %d\n", errno);
#endif
			break;
		case EncUtf8:
#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
			convTo = CCnvCharacterSetConverter::NewLC();
			convFrom = CCnvCharacterSetConverter::NewLC();
			((CCnvCharacterSetConverter*)convTo)->PrepareToConvertToOrFromL(KCharacterSetIdentifierCodePage1252, fs);
			((CCnvCharacterSetConverter*)convFrom)->PrepareToConvertToOrFromL(KCharacterSetIdentifierUtf8, fs);
#else
			iconvData = (size_t)iconv_open("UTF-8",SHVStringC_Get8BitCharSet());
		
			if (iconvData == (size_t)-1) // failure, try ascii as fallback
				iconvData = (size_t)iconv_open("UTF-8","ASCII");
		
			if (iconvData == (size_t)-1)
				fprintf(stderr, "UTF-8 conversion error %d\n", errno);
#endif
			break;
		default:
			SHVASSERT(false);
		}
		break;
	case Enc16:
		switch (To)
		{
		case Enc8:
#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
			convTo = CCnvCharacterSetConverter::NewLC();
			((CCnvCharacterSetConverter*)convTo)->PrepareToConvertToOrFromL(KCharacterSetIdentifierCodePage1252, fs);
#else
			iconvData = (size_t)iconv_open(SHVStringC_Get8BitCharSet(),"UCS-2");
		
			if (iconvData == (size_t)-1) // failure, try ascii as fallback
				iconvData = (size_t)iconv_open("ASCII","UCS-2");
		
			if (iconvData == (size_t)-1)
				fprintf(stderr, "UNICODE->local conversion error %d\n", errno);
#endif
			break;
		case EncUtf8:
#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
			convTo = CCnvCharacterSetConverter::NewLC();
			((CCnvCharacterSetConverter*)convTo)->PrepareToConvertToOrFromL(KCharacterSetIdentifierUtf8, fs);
#else
			iconvData = (size_t)iconv_open("UTF-8","UCS-2");
		
			if (iconvData == (size_t)-1)
				fprintf(stderr, "UTF-8 conversion error %d\n", errno);
#endif
			break;
		default:
			SHVASSERT(false);
		}
		break;
	case EncUtf8:
		switch (To)
		{
		case Enc8:
#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
			convTo = CCnvCharacterSetConverter::NewLC();
			convFrom = CCnvCharacterSetConverter::NewLC();
			((CCnvCharacterSetConverter*)convTo)->PrepareToConvertToOrFromL(KCharacterSetIdentifierUtf8, fs);
			((CCnvCharacterSetConverter*)convFrom)->PrepareToConvertToOrFromL(KCharacterSetIdentifierCodePage1252, fs);
#else
			iconvData = (size_t)iconv_open(SHVStringC_Get8BitCharSet(),"UTF-8");
		
			if (iconvData == (size_t)-1) // failure, try ascii as fallback
				iconvData = (size_t)iconv_open("ASCII","UTF-8");
		
			if (iconvData == (size_t)-1)
				fprintf(stderr, "UTF-8 conversion error %d\n", errno);
#endif
			break;
		case Enc16:
#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
			convTo = CCnvCharacterSetConverter::NewLC();
			((CCnvCharacterSetConverter*)convTo)->PrepareToConvertToOrFromL(KCharacterSetIdentifierUtf8, fs);
#else
			iconvData = (size_t)iconv_open("UCS-2","UTF-8");
		
			if (iconvData == (size_t)-1)
				fprintf(stderr, "UTF-8 conversion error %d\n", errno);
#endif
			break;
		default:
			SHVASSERT(false);
		}
		break;
	}
}

/*************************************
 * Destructor
 *************************************/
SHVStringConv::~SHVStringConv()
{
#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
	if (convTo)
		CleanupStack::PopAndDestroy(); // convTo
	if (convFrom)
		CleanupStack::PopAndDestroy(); // convTo
	fs.Close();
#else
	if (iconvData != (size_t)-1)
		iconv_close((iconv_t)iconvData);
#endif
}

/*************************************
 * IsValid
 *************************************/
/// Validates the conversion class
/**
 \return True if the from and to encodings matches a valid routine.
 *
 * This is really only useful in assertions.
 */
bool SHVStringConv::IsValid()
{
#ifdef __SHIVA_WIN32
	return true;
#elif defined(__SHIVA_EPOC)
	if (convTo)
		reteurn true;
#else
	if (iconvData != (size_t)-1)
		return true;
#endif
	return false;
}

/*************************************
 * Convert
 *************************************/
/// Converts between the 2 encodings
/**
 \param inBuffer Input buffer
 \param outBuffer Output buffer
 \param len Length of output buffer
 \param charsWritten Optional output var of chars written
 \return The next byte not converted
 *
 \note In Utf8 output mode the chars written are actual bytes written, not Utf8 chars.\n
 * 
 * Unknown chars are written as ?.\n
 * The input buffer must be null terminated the case of the output buffer being
 * longer than the input buffer, in characters.\n
 * The conversion stops when input buffer hits a null char or there is not room in
 * the output buffer for any more characters.\n
 * Please note that when writing utf8 characters it is not guaranteed that the
 * output buffer is completely filled, due to the fact that utf8 characters have
 * variable length.
 */
const SHVByte* SHVStringConv::Convert(const SHVByte* inBuffer, void* outBuffer, size_t len, size_t* charsWritten)
{
	switch (From)
	{
	case Enc8:
		switch (To)
		{
		case Enc16:
			return (const SHVByte*)Convert8To16((const SHVChar*)inBuffer,(SHVWChar*)outBuffer,len, charsWritten);
		case EncUtf8:
			return (const SHVByte*)Convert8ToUTF8((const SHVChar*)inBuffer,(SHVChar*)outBuffer,len, charsWritten);
		default:
			SHVASSERT(false);
		}
	case Enc16:
		switch (To)
		{
		case Enc8:
			return (const SHVByte*)Convert16To8((const SHVWChar*)inBuffer,(SHVChar*)outBuffer,len, charsWritten);
		case EncUtf8:
			return (const SHVByte*)Convert16ToUTF8((const SHVWChar*)inBuffer,(SHVChar*)outBuffer,len, charsWritten);
		default:
			SHVASSERT(false);
		}
	case EncUtf8:
		switch (To)
		{
		case Enc8:
			return (const SHVByte*)ConvertUTF8To8((const SHVChar*)inBuffer,(SHVChar*)outBuffer,len, charsWritten);
		case Enc16:
			return (const SHVByte*)ConvertUTF8To16((const SHVChar*)inBuffer,(SHVWChar*)outBuffer,len, charsWritten);
		default:
			SHVASSERT(false);
		}
	default:
		SHVASSERT(false); // unknown encoding variant
	}
	
	if (charsWritten)
		*charsWritten = 0;
	
	return 0;
}



//=========================================================================================================

/*************************************
 * ToStr16
 *************************************/
SHVStringBuffer16 SHVString8C::ToStr16() const
{
	if (IsNull()) return SHVStringBuffer16();
size_t len = GetLength();
SHVString16 retVal;

	ConvertBufferToWChar(retVal.SetBufferSize(len+1), len);
	
	return retVal.ReleaseBuffer();
}

/*************************************
 * ConvertBufferToWChar
 *************************************/
bool SHVString8C::ConvertBufferToWChar(SHVWChar* outBuffer, size_t len) const
{
SHVStringConv conv(SHVStringConv::Enc8,SHVStringConv::Enc16);
bool retVal = *conv.Convert((const SHVByte*)Buffer,outBuffer,len,&len) == 0;
	if (outBuffer)
		outBuffer[len] = 0;
	return retVal;
}
const SHVChar* SHVStringConv::Convert8To16(const SHVChar* inBuffer, SHVWChar* outBuffer, size_t len, size_t* charsWritten)
{
size_t written;
	if (!charsWritten) charsWritten = &written;
	*charsWritten = 0;
	if (!IsValid()) return inBuffer;
	if (!outBuffer) { *charsWritten = SHVString8C::StrLen(inBuffer); return inBuffer + *charsWritten; }
#ifdef __SHIVA_WIN32
	if (len)
	{
		*charsWritten = ::mbstowcs((wchar_t*)outBuffer,inBuffer,len);
		SHVASSERT(*charsWritten != (size_t)-1);
		if (*charsWritten == (size_t)-1) // invalid char encountered
			*charsWritten = 0;
	}
	inBuffer += *charsWritten;
#elif defined(__SHIVA_EPOC)
TPtr16 unicode( outBuffer, len );
TPtrC8  ascii( (TUint8*)inBuffer, SHVString8C::StrLen(inBuffer) );
TInt aState = CCnvCharacterSetConverter::KStateDefault;
TInt result;

	result = ((CCnvCharacterSetConverter*)convTo)->ConvertToUnicode( unicode, ascii, aState);
	SHVASSERT(result >= 0);
	if (result < 0) // error
		*charsWritten = 0;
	else
		*charsWritten = (size_t)(ascii.Length()-result);
	inBuffer += charsWritten;
#else
const char*  iBuf;
SHVWChar* oBuf;
size_t iLeft;
size_t oLeft;

	for (size_t i=0; i<len && *inBuffer; i++)
	{
		iBuf = inBuffer;
		oBuf = outBuffer;
		iLeft  = 1;
		oLeft  = 2;
		if ( iconv((iconv_t)iconvData,(ICONV_IBUFTYPE)&iBuf,&iLeft,(char**)&oBuf,&oLeft) == ICONV_ERR)
		{
			//fprintf(stderr, "UNKNOWN CHAR: %c %d\n",buf[i],(short)buf[i]);
			*outBuffer = (short)'?';
		}
		(*charsWritten)++;
		inBuffer++;
		outBuffer++;
	}
#endif
	return inBuffer;
}

/*************************************
 * ToStr8
 *************************************/
SHVStringBuffer8 SHVString16C::ToStr8() const
{
	if (IsNull()) return SHVStringBuffer8();
size_t len = GetLength();
SHVString8 retVal;

	ConvertBufferToChar(retVal.SetBufferSize(len+1), len);
	
	return retVal.ReleaseBuffer();
}

/*************************************
 * ConvertBufferToChar
 *************************************/
bool SHVString16C::ConvertBufferToChar(SHVChar* outBuffer, size_t len) const
{
SHVStringConv conv(SHVStringConv::Enc16,SHVStringConv::Enc8);
bool retVal = *conv.Convert((const SHVByte*)Buffer,outBuffer,len,&len) == 0;
	if (outBuffer)
		outBuffer[len] = 0;
	return retVal;
}
const SHVWChar* SHVStringConv::Convert16To8(const SHVWChar* inBuffer, SHVChar* outBuffer, size_t len, size_t* charsWritten)
{
size_t written;
	if (!charsWritten) charsWritten = &written;
	*charsWritten = 0;
	if (!IsValid()) return inBuffer;
	if (!outBuffer) { *charsWritten = SHVString16C::StrLen(inBuffer); return inBuffer + *charsWritten; }
#ifdef __SHIVA_WIN32
	if (len)
	{
		*charsWritten = ::wcstombs(outBuffer,(const wchar_t*)inBuffer,len);
		SHVASSERT(*charsWritten != (size_t)-1);
		if (*charsWritten == (size_t)-1) // invalid char encountered
			*charsWritten = 0;
	}
	outBuffer[*charsWritten] = 0;
	inBuffer += *charsWritten;
#elif defined(__SHIVA_EPOC)
TPtrC16 unicode( inBuffer, SHVString16C::StrLen(inBuffer) );
TPtr8   ascii( (TUint8*)outBuffer, len );
TInt aState = CCnvCharacterSetConverter::KStateDefault;
TInt result;

	result = ((CCnvCharacterSetConverter*)convTo)->ConvertFromUnicode( ascii, unicode, aState);
	SHVASSERT(result >= 0);
	if (result < 0) // error
		*charsWritten = 0;
	else
		*charsWritten = (size_t)(ascii.Length()-result);
	outBuffer[*charsWritten] = 0;
	inBuffer += charsWritten;
#else
const SHVWChar* iBuf;
char* oBuf;
size_t iLeft;
size_t oLeft;

	for (size_t i=0; i<len && *inBuffer; i++)
	{
		iBuf = inBuffer;
		oBuf = outBuffer;
		iLeft  = 2;
		oLeft  = 1;
		if ( iconv((iconv_t)iconvData,(ICONV_IBUFTYPE)&iBuf,&iLeft,&oBuf,&oLeft) == ICONV_ERR)
		{
			*outBuffer = '?';
		}
		(*charsWritten)++;
		inBuffer++;
		outBuffer++;
	}

#endif

	return inBuffer;
}

/*************************************
 * ToStr8
 *************************************/
SHVStringBuffer8 SHVStringUTF8C::ToStr8() const
{
SHVString8 retVal;

	if (!IsNull())
	{
	size_t len = GetLength();
		ConvertBufferToChar(retVal.SetBufferSize(len+1), len);
	}
	
	return retVal.ReleaseBuffer();
}

/*************************************
 * ConvertBufferToChar
 *************************************/
bool SHVStringUTF8C::ConvertBufferToChar(SHVChar* buffer, size_t len) const
{
SHVStringConv conv(SHVStringConv::EncUtf8,SHVStringConv::Enc8);
bool retVal = *conv.Convert((const SHVByte*)Buffer,buffer,len,&len) == 0;
	if (buffer)
		buffer[len] = 0;
	return retVal;
}
const SHVChar* SHVStringConv::ConvertUTF8To8(const SHVChar* inBuffer, SHVChar* outBuffer, size_t len, size_t* charsWritten)
{
size_t written;
	if (!charsWritten) charsWritten = &written;
	*charsWritten = 0;
	if (!IsValid()) return inBuffer;
	if (!outBuffer) { *charsWritten = SHVStringUTF8C::StrLen(inBuffer); return inBuffer + SHVString8C::StrLen(inBuffer); } // lazy solution
const char* endch;
size_t bytes;
size_t charLen;
#ifdef __SHIVA_WIN32
WCHAR ch;
#elif defined(__SHIVA_EPOC)
SHVWChar ch;
#else
const char* iBuf;
char* oBuf;
size_t iLeft;
size_t oLeft;
#endif

	while (*inBuffer && *charsWritten < len)
	{
		(*charsWritten)++;
		charLen = SHVStringUTF8C::GetUTF8CharLen(*inBuffer);
		if (charLen < 2)
		{
			*outBuffer = *inBuffer;
			outBuffer++;
			inBuffer++;
		}
		else
		{
			endch = inBuffer+1;
			for (bytes=1;charLen && ((*(inBuffer+bytes))&0xC0) == 0x80;bytes++, charLen--) endch++;

#ifdef __SHIVA_WIN32
			MultiByteToWideChar(CP_UTF8,0,inBuffer,(int)bytes,&ch,1);
			WideCharToMultiByte(CP_ACP,0,&ch,1,outBuffer,1,"?",NULL);
#elif defined(__SHIVA_EPOC)
			aState = CCnvCharacterSetConverter::KStateDefault;
			{
			TPtr16 unicodePtr((TUint16*)&ch,1);
			TPtr8 utf8Ptr((TUint8*)outBuffer,1);
				if (((CCnvCharacterSetConverter*)convTo)->ConvertToUnicode( unicodePtr, TPtrC8((const TUint8*)inBuffer,bytes), aState ) != 0 ||
					((CCnvCharacterSetConverter*)convFrom)->ConvertFromUnicode( utf8Ptr, TPtrC16((const TUint16*)&ch,1), aState ) != 0)
				{
					*buffer = '?';
				}
			}
#else
			iBuf = (char*)inBuffer;
			oBuf = (char*)outBuffer;
			iLeft  = bytes;
			oLeft  = 1;

			if ( iconv((iconv_t)iconvData,(ICONV_IBUFTYPE)&iBuf,&iLeft,&oBuf,&oLeft) == ICONV_ERR)
				*outBuffer = '?';
#endif

			outBuffer++;
			inBuffer=endch;
		}
	}

	return inBuffer;
}

/*************************************
 * ToStr16
 *************************************/
SHVStringBuffer16 SHVStringUTF8C::ToStr16() const
{
SHVString16 retVal;

	if (!IsNull())
	{
	size_t len = GetLength();
		ConvertBufferToWChar(retVal.SetBufferSize(len+1), len);
	}
	
	return retVal.ReleaseBuffer();
}

/*************************************
 * ConvertBufferToWChar
 *************************************/
bool SHVStringUTF8C::ConvertBufferToWChar(SHVWChar* buffer, size_t len) const
{
SHVStringConv conv(SHVStringConv::EncUtf8,SHVStringConv::Enc16);
bool retVal = *conv.Convert((const SHVByte*)Buffer,buffer,len,&len) == 0;
	if (buffer)
		buffer[len] = 0;
	return retVal;
}
const SHVChar* SHVStringConv::ConvertUTF8To16(const SHVChar* inBuffer, SHVWChar* outBuffer, size_t len, size_t* charsWritten)
{
size_t written;
	if (!charsWritten) charsWritten = &written;
	*charsWritten = 0;
	if (!IsValid()) return inBuffer;
	if (!outBuffer) { *charsWritten = SHVStringUTF8C::StrLen(inBuffer); return inBuffer + SHVString8C::StrLen(inBuffer); } // lazy solution
const char* endch;
size_t bytes;
size_t charLen;
#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
#else
const char* iBuf;
char* oBuf;
size_t iLeft;
size_t oLeft;
#endif

	while (*inBuffer && *charsWritten < len)
	{
		(*charsWritten)++;
		charLen = SHVStringUTF8C::GetUTF8CharLen(*inBuffer);
		if (charLen < 2)
		{
			*outBuffer = *inBuffer;
			outBuffer++;
			inBuffer++;
		}
		else
		{
			endch = inBuffer+1;
			for (bytes=1;charLen && ((*(inBuffer+bytes))&0xC0) == 0x80;bytes++, charLen--) endch++;

#ifdef __SHIVA_WIN32
			MultiByteToWideChar(CP_UTF8,0,inBuffer,(int)bytes,(WCHAR*)outBuffer,1);
#elif defined(__SHIVA_EPOC)
			aState = CCnvCharacterSetConverter::KStateDefault;
			{
			TPtr16 unicodePtr((TUint16*)outBuffer,1);
				if (cnvToUnicode->ConvertToUnicode( unicodePtr, TPtrC8((const TUint8*)inBuffer,bytes), aState ) != 0)
				{
					*buffer = '?';
				}
			}
#else
			iBuf = (char*)inBuffer;
			oBuf = (char*)outBuffer;
			iLeft  = bytes;
			oLeft  = 2;

			if ( iconv((iconv_t)iconvData,(ICONV_IBUFTYPE)&iBuf,&iLeft,&oBuf,&oLeft) == ICONV_ERR)
				*outBuffer = '?';
#endif

			outBuffer++;
			inBuffer=endch;
		}
	}

	return inBuffer;
}



/*************************************
 * ToStrUTF8
 *************************************/
SHVStringBufferUTF8 SHVString8C::ToStrUTF8() const
{
SHVStringUTF8 retVal;

	if (!IsNull())
	{
	size_t len;

		if (ConvertBufferToUTF8(NULL,len))
			ConvertBufferToUTF8(retVal.SetBufferSize(len+1), len);
	}
	
	return retVal.ReleaseBuffer();
}


/*************************************
 * CalculateUTF8Len8
 *************************************/
size_t SHVStringUTF8C::CalculateUTF8Len8(const SHVChar* str)
{
size_t retVal = 0;

	SHVString8C(str).ConvertBufferToUTF8(NULL,retVal);
	return retVal;
}

/*************************************
 * ConvertBufferToUTF8
 *************************************/
bool SHVString8C::ConvertBufferToUTF8(SHVChar* str, size_t& len) const
{
SHVStringConv conv(SHVStringConv::Enc8,SHVStringConv::EncUtf8);
bool retVal = *conv.Convert((const SHVByte*)Buffer,str,len,&len) == 0;
	if (str)
		str[len] = 0;
	return retVal;
}
const SHVChar* SHVStringConv::Convert8ToUTF8(const SHVChar* inBuffer, SHVChar* outBuffer, size_t len, size_t* charsWritten)
{
size_t written;
	if (!charsWritten) charsWritten = &written;
	*charsWritten = 0;
	if (!IsValid()) return inBuffer;
char utf8Str[5];
#ifdef __SHIVA_WIN32
WCHAR ch;
int bytes;
#elif defined(__SHIVA_EPOC)
SHVWChar ch;
#else
const char* iBuf;
char* oBuf;
size_t iLeft;
size_t oLeft;
#endif

	// hack to make sure we can ignore outBuffer in the while check
	if (!outBuffer)
		len = SIZE_T_MAX;

	while (*inBuffer && *charsWritten < len)
	{
		if (!((*inBuffer)&0x80))
		{
			if (outBuffer)
			{
				*outBuffer = *inBuffer;
				outBuffer++;
			}
			inBuffer++;
			(*charsWritten)++;
		}
		else
		{
#ifdef __SHIVA_WIN32
			MultiByteToWideChar(CP_ACP,0,inBuffer,1,&ch,1);
			bytes = WideCharToMultiByte(CP_UTF8,0,&ch,1,outBuffer ? outBuffer : utf8Str, outBuffer ? int(len-*charsWritten) : 4,NULL,NULL);
			if (bytes)
			{
				*charsWritten += bytes;
				if (outBuffer)
					outBuffer += bytes;
			}
			else
			{
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
				{
					break;
				}
				else if (outBuffer)
				{
					*outBuffer = '?';
					outBuffer++;
				}
				(*charsWritten)++;
			}
#elif defined(__SHIVA_EPOC)
			aState = CCnvCharacterSetConverter::KStateDefault;
			{
			TPtr16 unicodePtr((TUint16*)&ch,1);
			TPtr8 utf8Ptr((TUint8*)(outBuffer ? outBuffer : utf8Str),outBuffer ? len-*charsWritten : 4);
			TInt errNo;
				if (((CCnvCharacterSetConverter*)convTo)->ConvertToUnicode( unicodePtr, TPtrC8((const TUint8*)inBuffer,1), aState ) != 0)
				{
					(*charsWritten)++;
					if (outBuffer)
					{
						*outBuffer = '?';
						outBuffer++;
					}
				}
				else
				{
					errNo = ((CCnvCharacterSetConverter*)convFrom)->ConvertFromUnicode( utf8Ptr, TPtrC16((const TUint16*)&ch,1), aState );
					if (errNo < 0)
					{
						///\todo detect lack of output buffer and break
						SHVASSERT(false);
						(*charsWritten)++;
						if (outBuffer)
						{
							*outBuffer = '?';
							outBuffer++;
						}
					}
					else
					{
						*charsWritten += 4-errNo;
						if (outBuffer)
							outBuffer += 4-errNo;
					}
				}
			}
#else
			iBuf = (char*)inBuffer;
			oBuf = (char*)(outBuffer ? outBuffer : utf8Str);
			iLeft  = 1;
			oLeft  = (outBuffer ? len-*charsWritten : 4);

			if ( iconv((iconv_t)iconvData,(ICONV_IBUFTYPE)&iBuf,&iLeft,&oBuf,&oLeft) == ICONV_ERR)
			{
				if (errno == E2BIG)
				{
					break;
				}
				(*charsWritten)++;
				if (outBuffer)
				{
					*outBuffer = '?';
					outBuffer++;
				}
			}
			else
			{
				if (outBuffer)
					outBuffer += len-*charsWritten - oLeft;
				*charsWritten += (outBuffer ? len-*charsWritten : 4) - oLeft;
			}
#endif

			inBuffer++;
		}
	}

	return inBuffer;
}

/*************************************
 * ToStrUTF8
 *************************************/
SHVStringBufferUTF8 SHVString16C::ToStrUTF8() const
{
SHVStringUTF8 retVal;

	if (!IsNull())
	{
	size_t len;

		if (ConvertBufferToUTF8(NULL,len))
			ConvertBufferToUTF8(retVal.SetBufferSize(len+1), len);
	}
	
	return retVal.ReleaseBuffer();
}

/*************************************
 * CalculateUTF8Len16
 *************************************/
size_t SHVStringUTF8C::CalculateUTF8Len16(const SHVWChar* str)
{
size_t retVal = 0;

	SHVString16C(str).ConvertBufferToUTF8(NULL,retVal);
	return retVal;
}








/*************************************
 * ConvertBufferToUTF8
 *************************************/
bool SHVString16C::ConvertBufferToUTF8(SHVChar* str, size_t& len) const
{
SHVStringConv conv(SHVStringConv::Enc16,SHVStringConv::EncUtf8);
bool retVal = *conv.Convert((const SHVByte*)Buffer,str,len,&len) == 0;
	if (str)
		str[len] = 0;
	return retVal;
}
const SHVWChar* SHVStringConv::Convert16ToUTF8(const SHVWChar* inBuffer, SHVChar* outBuffer, size_t len, size_t* charsWritten)
{
size_t written;
	if (!charsWritten) charsWritten = &written;
	*charsWritten = 0;
	if (!IsValid()) return inBuffer;
char utf8Str[5];
#ifdef __SHIVA_WIN32
int bytes;
#elif defined(__SHIVA_EPOC)
#else
const char* iBuf;
char* oBuf;
size_t iLeft;
size_t oLeft;
#endif

	// hack to make sure we can ignore outBuffer in the while check
	if (!outBuffer)
		len = SIZE_T_MAX;
	
	while (*inBuffer && *charsWritten < len)
	{
		if ( !((*inBuffer)&0xFF80) )
		{
			if (outBuffer)
			{
				*outBuffer = (const char)*inBuffer;
				outBuffer++;
			}
			inBuffer++;
			(*charsWritten)++;
		}
		else
		{
#ifdef __SHIVA_WIN32
			bytes = WideCharToMultiByte(CP_UTF8,0,(const WCHAR*)inBuffer,1,outBuffer ? outBuffer : utf8Str, outBuffer ? int(len-*charsWritten) : 4,NULL,NULL);
			if (bytes)
			{
				*charsWritten += bytes;
				if (outBuffer)
					outBuffer += bytes;
			}
			else
			{
				if (GetLastError() == ERROR_INSUFFICIENT_BUFFER)
				{
					break;
				}
				else if (outBuffer)
				{
					*outBuffer = '?';
					outBuffer++;
				}
				(*charsWritten)++;
			}
#elif defined(__SHIVA_EPOC)
			aState = CCnvCharacterSetConverter::KStateDefault;
			{
			TPtr16 unicodePtr((TUint16*)inBuffer,1);
			TPtr8 utf8Ptr((TUint8*)(outBuffer ? outBuffer : utf8Str),outBuffer ? len-*charsWritten : 4);
			TInt errNo = ((CCnvCharacterSetConverter*)convTo)->ConvertFromUnicode( utf8Ptr, TPtrC16((const TUint16*)&ch,1), aState );
				if (errNo < 0)
				{
					///\todo detect lack of output buffer and break
					SHVASSERT(false);
					(*charsWritten)++;
					if (outBuffer)
					{
						*outBuffer = '?';
						outBuffer++;
					}
				}
				else
				{
					*charsWritten += 4-errNo;
					if (outBuffer)
						outBuffer += 4-errNo;
				}
			}
#else
			iBuf = (char*)inBuffer;
			oBuf = (char*)(outBuffer ? outBuffer : utf8Str);
			iLeft  = 2;
			oLeft  = outBuffer ? len-*charsWritten : 4;

			if ( iconv((iconv_t)iconvData,(ICONV_IBUFTYPE)&iBuf,&iLeft,&oBuf,&oLeft) == ICONV_ERR)
			{
				if (errno == E2BIG)
				{
					break;
				}
				(*charsWritten)++;
				if (outBuffer)
				{
					*outBuffer = '?';
					outBuffer++;
				}
			}
			else
			{
				if (outBuffer)
					outBuffer += len-*charsWritten - oLeft;
				*charsWritten += (outBuffer ? len-*charsWritten : 4) - oLeft;
			}
#endif
			inBuffer++;
		}
	}

	return inBuffer;
}

#endif
