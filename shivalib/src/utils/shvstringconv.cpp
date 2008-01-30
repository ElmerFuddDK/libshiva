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
#include "../../../include/utils/shvstring.h"

#ifdef __SHVSTRING_INCLUDE_UNICODE

#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
# include <f32file.h>
# include <charconv.h>
#else
# include <unicode.h>
const char* SHVStringC_Get8BitCharSet()
{
const char* retVal = getenv("CHARSET");
	if (!retVal || !SHVString8C::StrCmp(retVal,"UTF-8")) //run away, it is UTF8
		retVal = "ISO-8859-1";
	return retVal;
}
#endif

SHVStringBuffer16 SHVString8C::ToStr16() const
{
	if (IsNull()) return SHVStringBuffer16();
size_t len = GetLength();
SHVString16 retVal;

	ConvertBufferToWChar(retVal.SetBufferSize(len+1), len);
	
	return retVal.ReleaseBuffer();
}

bool SHVString8C::ConvertBufferToWChar(SHVWChar* outBuffer, size_t len) const
{
bool retVal = true;
#ifdef __SHIVA_WIN32

	if (len) ::mbstowcs((wchar_t*)outBuffer,Buffer,len);
	outBuffer[len] = 0;
#elif defined(__SHIVA_EPOC)
CCnvCharacterSetConverter* cnv = CCnvCharacterSetConverter::NewLC();
RFs fs;
TPtr16 unicode( outBuffer, len );
TPtrC8  ascii( (TUint8*)Buffer, len );
TInt aState = CCnvCharacterSetConverter::KStateDefault;
TInt result;

	fs.Connect();
	cnv->PrepareToConvertToOrFromL(KCharacterSetIdentifierCodePage1252, fs);
	result = cnv->ConvertToUnicode( unicode, ascii, aState);
	outBuffer[len] = 0;

	fs.Close();

	CleanupStack::PopAndDestroy(); // cnv

#else
const char*  iBuf;
SHVWChar* oBuf;
size_t iLeft;
size_t oLeft;
unicode_iconv_t conv = unicode_iconv_open("UCS-2",SHVStringC_Get8BitCharSet());

	if ((size_t)conv == -1) // failure, try ascii as fallback
		conv = unicode_iconv_open("UCS-2","ASCII");

	if ((size_t)conv == -1)
	{
		fprintf(stderr, "local->UNICODE conversion error %d\n", errno);
		outBuffer[0] = 0;
		return retVal;
	}

	for (size_t i=0; i<len; i++)
	{
		iBuf = Buffer+i;
		oBuf = outBuffer+i;
		outBuffer[i]=0;
		iLeft  = 1;
		oLeft  = 2;
		if ( unicode_iconv(conv,&iBuf,&iLeft,(char**)&oBuf,&oLeft) == -1)
		{
			switch (Buffer[i])
			{
			default:
				//fprintf(stderr, "UNKNOWN CHAR: %c %d\n",buf[i],(short)buf[i]);
				outBuffer[i] = (short)'?';
				break;
			}
		}
	}

	outBuffer[len] = 0;

	unicode_iconv_close(conv);
#endif
	return retVal;
}

SHVStringBuffer8 SHVString16C::ToStr8() const
{
	if (IsNull()) return SHVStringBuffer8();
size_t len = GetLength();
SHVString8 retVal;

	ConvertBufferToChar(retVal.SetBufferSize(len+1), len);
	
	return retVal.ReleaseBuffer();
}

bool SHVString16C::ConvertBufferToChar(SHVChar* outBuffer, size_t len) const
{
	bool retVal = true;
#ifdef __SHIVA_WIN32

	::wcstombs(outBuffer,(const wchar_t*)Buffer,len);
	outBuffer[len] = '\0';

#elif defined(__SHIVA_EPOC)

CCnvCharacterSetConverter* cnv = CCnvCharacterSetConverter::NewLC();
RFs fs;
TPtrC16 unicode( Buffer, len );
TPtr8   ascii( (TUint8*)outBuffer, len );
TInt aState = CCnvCharacterSetConverter::KStateDefault;

	fs.Connect();
	cnv->PrepareToConvertToOrFromL(KCharacterSetIdentifierCodePage1252, fs);
	cnv->ConvertFromUnicode( ascii, unicode, aState );
	outBuffer[len] = '\0';

	fs.Close();

	CleanupStack::PopAndDestroy(); // cnv

#else
const SHVWChar* iBuf;
char* oBuf;
size_t iLeft;
size_t oLeft;
unicode_iconv_t conv = unicode_iconv_open(SHVStringC_Get8BitCharSet(),"UCS-2");

	if ((size_t)conv == -1) // failure, try ascii as fallback
		conv = unicode_iconv_open("ASCII","UCS-2");

	if ((size_t)conv == -1)
	{
		fprintf(stderr, "UNICODE->local conversion error %d\n", errno);
		outBuffer[0] = '\0';
		return retVal;
	}

	for (size_t i=0; i<len; i++)
	{
		iBuf = Buffer+i;
		oBuf = outBuffer+i;
		iLeft  = 2;
		oLeft  = 1;
		if ( unicode_iconv(conv,(const char**)&iBuf,&iLeft,&oBuf,&oLeft) == -1)
		{
			switch (Buffer[i])
			{
			default:
				outBuffer[i] = '?';
				break;
			}
		}
	}

	outBuffer[len] = '\0';

	unicode_iconv_close(conv);
#endif

	return retVal;
}

SHVStringBuffer8 SHVString8C::FromUTF8(const char* utf8Str)
{
SHVString8 retVal;
size_t len = SHVString8C::StrLen(utf8Str);
SHVChar* buf;
const char* endch;
size_t bytes;
#ifdef __SHIVA_WIN32
WCHAR ch;
#elif defined(__SHIVA_EPOC)
CCnvCharacterSetConverter* cnvToUnicode = CCnvCharacterSetConverter::NewLC();
CCnvCharacterSetConverter* cnvFromUnicode = CCnvCharacterSetConverter::NewLC();
RFs fs;
TInt aState;
SHVWChar ch;

	fs.Connect();

	cnvToUnicode->PrepareToConvertToOrFromL(KCharacterSetIdentifierUtf8, fs);
	cnvFromUnicode->PrepareToConvertToOrFromL(KCharacterSetIdentifierCodePage1252, fs);
#else
const char* iBuf;
char* oBuf;
size_t iLeft;
size_t oLeft;
unicode_iconv_t conv = unicode_iconv_open(SHVStringC_Get8BitCharSet(),"UTF-8");

	if ((size_t)conv == -1) // failure, try ascii as fallback
		conv = unicode_iconv_open("ASCII","UTF-8");

	if ((size_t)conv == -1)
	{
		fprintf(stderr, "UTF-8 conversion error %d\n", errno);
		return retVal.ReleaseBuffer();
	}
#endif

	retVal.AllocBuffer(len+1);
	buf = retVal.Buffer;

	while (*utf8Str)
	{
		if (!((*utf8Str)&0x80))
		{
			*buf = *utf8Str;
			buf++;
			utf8Str++;
		}
		else
		{
			endch = utf8Str+1;
			for (bytes=1;((*(utf8Str+bytes))&0xC0) == 0x80;bytes++) endch++;

#ifdef __SHIVA_WIN32
			MultiByteToWideChar(CP_UTF8,0,utf8Str,(int)bytes,&ch,1);
			WideCharToMultiByte(CP_ACP,0,&ch,1,buf,1,"?",NULL);
#elif defined(__SHIVA_EPOC)
			aState = CCnvCharacterSetConverter::KStateDefault;
			{
			TPtr16 unicodePtr((TUint16*)&ch,1);
			TPtr8 utf8Ptr((TUint8*)buf,1);
				if (cnvToUnicode->ConvertToUnicode( unicodePtr, TPtrC8((const TUint8*)utf8Str,bytes), aState ) != 0 ||
					cnvFromUnicode->ConvertFromUnicode( utf8Ptr, TPtrC16((const TUint16*)&ch,1), aState ) != 0)
				{
					*buf = '?';
				}
			}
#else
			iBuf = (char*)utf8Str;
			oBuf = (char*)buf;
			iLeft  = bytes;
			oLeft  = 1;

			if ( unicode_iconv(conv,&iBuf,&iLeft,&oBuf,&oLeft) == -1)
				*buf = '?';
#endif

			buf++;
			utf8Str=endch;
		}
	}
	*buf = '\0';

#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
	fs.Close();
	CleanupStack::PopAndDestroy(2); // cnv's
#else
	unicode_iconv_close(conv);
#endif

	return retVal.ReleaseBuffer();
}

SHVStringBuffer16 SHVString16C::FromUTF8(const char* utf8Str)
{
SHVString16 retVal;
size_t len = SHVString8C::StrLen(utf8Str);
SHVWChar* buf;
const char* endch;
size_t bytes;
#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
CCnvCharacterSetConverter* cnvToUnicode = CCnvCharacterSetConverter::NewLC();
RFs fs;
TInt aState;

	fs.Connect();

	cnvToUnicode->PrepareToConvertToOrFromL(KCharacterSetIdentifierUtf8, fs);
#else
const char* iBuf;
char* oBuf;
size_t iLeft;
size_t oLeft;
unicode_iconv_t conv = unicode_iconv_open("UCS-2","UTF-8");

	if ((size_t)conv == -1)
	{
		fprintf(stderr, "UTF-8 conversion error %d\n", errno);
		return retVal.ReleaseBuffer();
	}
#endif

	retVal.AllocBuffer(len+1);
	buf = retVal.Buffer;

	while (*utf8Str)
	{
		if (!((*utf8Str)&0x80))
		{
			*buf = *utf8Str;
			buf++;
			utf8Str++;
		}
		else
		{
			endch = utf8Str+1;
			for (bytes=1;((*(utf8Str+bytes))&0xC0) == 0x80;bytes++) endch++;

#ifdef __SHIVA_WIN32
			MultiByteToWideChar(CP_UTF8,0,utf8Str,(int)bytes,(WCHAR*)buf,1);
#elif defined(__SHIVA_EPOC)
			aState = CCnvCharacterSetConverter::KStateDefault;
			{
			TPtr16 unicodePtr((TUint16*)buf,1);
				if (cnvToUnicode->ConvertToUnicode( unicodePtr, TPtrC8((const TUint8*)utf8Str,bytes), aState ) != 0)
				{
					*buf = '?';
				}
			}
#else
			iBuf = (char*)utf8Str;
			oBuf = (char*)buf;
			iLeft  = bytes;
			oLeft  = 2;

			if ( unicode_iconv(conv,&iBuf,&iLeft,&oBuf,&oLeft) == -1)
				*buf = '?';
#endif

			buf++;
			utf8Str=endch;
		}
	}
	*buf = '\0';

#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
	fs.Close();
	CleanupStack::PopAndDestroy(1); // cnv
#else
	unicode_iconv_close(conv);
#endif

	return retVal.ReleaseBuffer();
}

#endif
