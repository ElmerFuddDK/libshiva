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
#include "../../../include/utils/shvstringutf8.h"

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

	if (conv == (unicode_iconv_t)-1) // failure, try ascii as fallback
		conv = unicode_iconv_open("UCS-2","ASCII");

	if (conv == (unicode_iconv_t)-1)
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

	if (conv == (unicode_iconv_t)-1) // failure, try ascii as fallback
		conv = unicode_iconv_open("ASCII","UCS-2");

	if (conv == (unicode_iconv_t)-1)
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
const char* utf8Str = Buffer;
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

	if (conv == (unicode_iconv_t)-1) // failure, try ascii as fallback
		conv = unicode_iconv_open("ASCII","UTF-8");

	if (conv == (unicode_iconv_t)-1)
	{
		fprintf(stderr, "UTF-8 conversion error %d\n", errno);
		return false;
	}
#endif

	SHVUNUSED_PARAM(len);

	while (*utf8Str)
	{
		if (!((*utf8Str)&0x80))
		{
			*buffer = *utf8Str;
			buffer++;
			utf8Str++;
		}
		else
		{
			endch = utf8Str+1;
			for (bytes=1;((*(utf8Str+bytes))&0xC0) == 0x80;bytes++) endch++;

#ifdef __SHIVA_WIN32
			MultiByteToWideChar(CP_UTF8,0,utf8Str,(int)bytes,&ch,1);
			WideCharToMultiByte(CP_ACP,0,&ch,1,buffer,1,"?",NULL);
#elif defined(__SHIVA_EPOC)
			aState = CCnvCharacterSetConverter::KStateDefault;
			{
			TPtr16 unicodePtr((TUint16*)&ch,1);
			TPtr8 utf8Ptr((TUint8*)buffer,1);
				if (cnvToUnicode->ConvertToUnicode( unicodePtr, TPtrC8((const TUint8*)utf8Str,bytes), aState ) != 0 ||
					cnvFromUnicode->ConvertFromUnicode( utf8Ptr, TPtrC16((const TUint16*)&ch,1), aState ) != 0)
				{
					*buffer = '?';
				}
			}
#else
			iBuf = (char*)utf8Str;
			oBuf = (char*)buffer;
			iLeft  = bytes;
			oLeft  = 1;

			if ( unicode_iconv(conv,&iBuf,&iLeft,&oBuf,&oLeft) == -1)
				*buffer = '?';
#endif

			buffer++;
			utf8Str=endch;
		}
	}
	*buffer = '\0';

#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
	fs.Close();
	CleanupStack::PopAndDestroy(2); // cnv's
#else
	unicode_iconv_close(conv);
#endif

	return true;
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
const char* utf8Str = Buffer;
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

	if (conv == (unicode_iconv_t)-1)
	{
		fprintf(stderr, "UTF-8 conversion error %d\n", errno);
		return false;
	}
#endif

	SHVUNUSED_PARAM(len);
	
	while (*utf8Str)
	{
		if (!((*utf8Str)&0x80))
		{
			*buffer = *utf8Str;
			buffer++;
			utf8Str++;
		}
		else
		{
			endch = utf8Str+1;
			for (bytes=1;((*(utf8Str+bytes))&0xC0) == 0x80;bytes++) endch++;

#ifdef __SHIVA_WIN32
			MultiByteToWideChar(CP_UTF8,0,utf8Str,(int)bytes,(WCHAR*)buffer,1);
#elif defined(__SHIVA_EPOC)
			aState = CCnvCharacterSetConverter::KStateDefault;
			{
			TPtr16 unicodePtr((TUint16*)buffer,1);
				if (cnvToUnicode->ConvertToUnicode( unicodePtr, TPtrC8((const TUint8*)utf8Str,bytes), aState ) != 0)
				{
					*buffer = '?';
				}
			}
#else
			iBuf = (char*)utf8Str;
			oBuf = (char*)buffer;
			iLeft  = bytes;
			oLeft  = 2;

			if ( unicode_iconv(conv,&iBuf,&iLeft,&oBuf,&oLeft) == -1)
				*buffer = '?';
#endif

			buffer++;
			utf8Str=endch;
		}
	}
	*buffer = '\0';

#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
	fs.Close();
	CleanupStack::PopAndDestroy(1); // cnv
#else
	unicode_iconv_close(conv);
#endif

	return true;
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
const SHVChar* buffer = Buffer;
char utf8Str[5];
size_t retVal = 0;
#ifdef __SHIVA_WIN32
WCHAR ch;
int bytes;
#elif defined(__SHIVA_EPOC)
CCnvCharacterSetConverter* cnvToUnicode = CCnvCharacterSetConverter::NewLC();
CCnvCharacterSetConverter* cnvFromUnicode = CCnvCharacterSetConverter::NewLC();
RFs fs;
TInt aState;
SHVWChar ch;

	fs.Connect();

	cnvToUnicode->PrepareToConvertToOrFromL(KCharacterSetIdentifierCodePage1252, fs);
	cnvFromUnicode->PrepareToConvertToOrFromL(KCharacterSetIdentifierUtf8, fs);
#else
const char* iBuf;
char* oBuf;
size_t iLeft;
size_t oLeft;
unicode_iconv_t conv = unicode_iconv_open("UTF-8",SHVStringC_Get8BitCharSet());

	if (conv == (unicode_iconv_t)-1) // failure, try ascii as fallback
		conv = unicode_iconv_open("UTF-8","ASCII");

	if (conv == (unicode_iconv_t)-1)
	{
		fprintf(stderr, "UTF-8 conversion error %d\n", errno);
		return false;
	}
#endif

	while (*buffer)
	{
		if (!((*buffer)&0x80))
		{
			if (str)
			{
				*str = *buffer;
				str++;
			}
			buffer++;
			retVal++;
		}
		else
		{
#ifdef __SHIVA_WIN32
			MultiByteToWideChar(CP_ACP,0,buffer,1,&ch,1);
			bytes = WideCharToMultiByte(CP_UTF8,0,&ch,1,str ? str : utf8Str,5,NULL,NULL);
			if (bytes)
			{
				retVal += bytes;
				if (str)
					str += bytes;
			}
			else
			{
				if (str)
				{
					*str = '?';
					str++;
				}
				retVal++;
			}
#elif defined(__SHIVA_EPOC)
			aState = CCnvCharacterSetConverter::KStateDefault;
			{
			TPtr16 unicodePtr((TUint16*)&ch,1);
			TPtr8 utf8Ptr((TUint8*)(str ? str : utf8Str),4);
				if (cnvToUnicode->ConvertToUnicode( unicodePtr, TPtrC8((const TUint8*)utf8Str,bytes), aState ) != 0 ||
					cnvFromUnicode->ConvertFromUnicode( utf8Ptr, TPtrC16((const TUint16*)&ch,1), aState ) != 0)
				{
					retVal++;
					if (str)
					{
						*str = '?';
						str++;
					}
				}
				else
				{
					retVal += SHVString8C::StrLen(utf8Str);
					if (str)
						str += SHVString8C::StrLen(utf8Str);
				}
			}
#else
			iBuf = (char*)str;
			oBuf = (char*)(str ? str : utf8Str);
			iLeft  = 1;
			oLeft  = 4;

			if ( unicode_iconv(conv,&iBuf,&iLeft,&oBuf,&oLeft) == -1)
			{
				retVal++;
				if (str)
				{
					*str = '?';
					str++;
				}
			}
			else
			{
				retVal += 4 - oLeft;
				if (str)
				{
					str += 4 - oLeft;
				}
			}
#endif

			buffer++;
		}
	}

	if (str)
		*str = '\0';

#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
	fs.Close();
	CleanupStack::PopAndDestroy(2); // cnv's
#else
	unicode_iconv_close(conv);
#endif

	len = retVal;

	return true;
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
const SHVWChar* buffer = Buffer;
char utf8Str[5];
size_t retVal = 0;
#ifdef __SHIVA_WIN32
int bytes;
#elif defined(__SHIVA_EPOC)
CCnvCharacterSetConverter* cnvFromUnicode = CCnvCharacterSetConverter::NewLC();
RFs fs;
TInt aState;

	fs.Connect();

	cnvFromUnicode->PrepareToConvertToOrFromL(KCharacterSetIdentifierUtf8, fs);
#else
const char* iBuf;
char* oBuf;
size_t iLeft;
size_t oLeft;
unicode_iconv_t conv = unicode_iconv_open("UTF-8","UCS-2");

	if (conv == (unicode_iconv_t)-1)
	{
		fprintf(stderr, "UTF-8 conversion error %d\n", errno);
		return false;
	}
#endif

	while (*buffer)
	{
		if ( !((*buffer)&0xFF80) )
		{
			if (str)
			{
				*str = (const char)*buffer;
				str++;
			}
			buffer++;
			retVal++;
		}
		else
		{
#ifdef __SHIVA_WIN32
			bytes = WideCharToMultiByte(CP_UTF8,0,buffer,1,str ? str : utf8Str,5,NULL,NULL);
			if (bytes)
			{
				retVal += bytes;
				if (str)
					str += bytes;
			}
			else
			{
				if (str)
				{
					*str = '?';
					str++;
				}
				retVal++;
			}
#elif defined(__SHIVA_EPOC)
			aState = CCnvCharacterSetConverter::KStateDefault;
			{
			TPtr16 unicodePtr((TUint16*)str,1);
			TPtr8 utf8Ptr((TUint8*)(str ? str : utf8Str),4);
				if (cnvFromUnicode->ConvertFromUnicode( utf8Ptr, TPtrC16((const TUint16*)&ch,1), aState ) != 0)
				{
					retVal++;
					if (str)
					{
						*str = '?';
						str++;
					}
				}
				else
				{
					retVal += SHVString8C::StrLen(utf8Str);
					if (str)
						str += SHVString8C::StrLen(utf8Str);
				}
			}
#else
			iBuf = (char*)buffer;
			oBuf = (char*)(str ? str : utf8Str);
			iLeft  = 2;
			oLeft  = 4;

			if ( unicode_iconv(conv,&iBuf,&iLeft,&oBuf,&oLeft) == -1)
			{
				retVal++;
				if (str)
				{
					*str = '?';
					str++;
				}
			}
			else
			{
				retVal += 4 - oLeft;
				if (str)
				{
					str += 4 - oLeft;
				}
			}
#endif
			buffer++;
		}
	}

	if (str)
		*str = '\0';

#ifdef __SHIVA_WIN32
#elif defined(__SHIVA_EPOC)
	fs.Close();
	CleanupStack::PopAndDestroy(2); // cnv's
#else
	unicode_iconv_close(conv);
#endif

	len = retVal;

	return true;
}

#endif
