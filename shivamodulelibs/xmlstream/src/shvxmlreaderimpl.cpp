/*
 *   Copyright (C) 2008 by Mogens Bak Nielsen, Lars Eriksen
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
#include "../include/shvxmlreaderimpl.h"


/*************************************
 * Constructor
 *************************************/
SHVXmlReaderImpl::SHVXmlReaderImpl(ParserEncoding enc, size_t bufferSize)
{
	StartElementCallback = EndElementCallback = ValueCallback = NULL;
	BufferSize = bufferSize;
	Encoding = enc;
	switch (Encoding)
	{
	case ParserEncodingUTF8:
		EncodingExpat = _T("UTF-8");
		break;
#ifndef UNICODE
	case ParserEncodingNative:
		EncodingExpat = _T("ISO-8859-1");
		break;
#else
	// native encoding is UTF16 when unicode is defined
	case ParserEncodingNative:
#endif
	case ParserEncodingUTF16:
		EncodingExpat = _T("UTF-16");
		break;
	}
	Parser = XML_ParserCreate(EncodingExpat);
	InitializeExpat();
	MultiDocument = false;
	ValuePos = NULL;
}

/*************************************
 * Destructor
 *************************************/
SHVXmlReaderImpl::~SHVXmlReaderImpl()
{
	if (StartElementCallback)
		delete StartElementCallback;
	if (EndElementCallback)
		delete EndElementCallback;
	if (ValueCallback)
		delete ValueCallback;
	XML_ParserFree(Parser);
}

/*************************************
 * CalculateAttributeCount
 *************************************/
size_t SHVXmlReaderImpl::CalculateAttributeCount() const
{
size_t retVal = 0;
size_t i = 0;
	if (Attributes)
	{
		for (;Attributes[i]; i+=2, retVal++);
	}
	return retVal;
}

/*************************************
 * GetElementName16
 *************************************/
SHVStringBuffer16 SHVXmlReaderImpl::GetElementName16() const
{
	return XML_Char2String16(Value);
}

/*************************************
 * GetAttributeName16
 *************************************/
SHVStringBuffer16 SHVXmlReaderImpl::GetAttributeName16(size_t idx) const
{
	return XML_Char2String16(Attributes[idx*2]);
}

/*************************************
 * GetAttributeValue16
 *************************************/
SHVStringBuffer16 SHVXmlReaderImpl::GetAttributeValue16(size_t idx) const
{
	return XML_Char2String16(Attributes[idx*2+1]);
}

/*************************************
 * GetAttributeValueByName16
 *************************************/
SHVStringBuffer16 SHVXmlReaderImpl::GetAttributeValueByName16(const SHVString16C& name) const
{
size_t idx = 0;
#ifdef UNICODE
	for (; Attributes && Attributes[idx] && SHVString16C::StrCmp(name.GetBufferConst(), Attributes[idx]); idx+=2);
#else
SHVStringUTF8 nameUTF8(name.ToStrUTF8());
	for (; Attributes && Attributes[idx] && SHVStringUTF8C::StrCmp(nameUTF8.GetBufferConst(), Attributes[idx]); idx+=2);	
#endif 
	if (Attributes && Attributes[idx])
		return XML_Char2String16(Attributes[idx+1]);
	else
		return SHVString16().ReleaseBuffer();
}

/*************************************
 * GetCharacterData16
 *************************************/
SHVStringBuffer16 SHVXmlReaderImpl::GetValue16() const
{
#ifdef UNICODE
SHVString16 str;
size_t p = 0;
	if (!ValueCol.IsNull())
		ValueCol->ReadString16(str, ValueCol->GetSize() / sizeof(SHVTChar), p);
	return str.ReleaseBuffer();
#else
SHVString8 str;
size_t p = 0;
	if (!ValueCol.IsNull())
		ValueCol->ReadString8(str, ValueCol->GetSize() /sizeof(SHVTChar), p);
	return str.ToStr16();
#endif
}

/*************************************
 * GetElementName8
 *************************************/
SHVStringBuffer8 SHVXmlReaderImpl::GetElementName8() const
{
	return XML_Char2String8(Value);
}

/*************************************
 * GetAttributeName8
 *************************************/
SHVStringBuffer8 SHVXmlReaderImpl::GetAttributeName8(size_t idx) const
{
	return XML_Char2String8(Attributes[idx*2]);
}

/*************************************
 * GetAttributeValue8
 *************************************/
SHVStringBuffer8 SHVXmlReaderImpl::GetAttributeValue8(size_t idx) const
{
	return XML_Char2String8(Attributes[idx*2+1]);
}

/*************************************
 * GetAttributeValueByName8
 *************************************/
SHVStringBuffer8 SHVXmlReaderImpl::GetAttributeValueByName8(const SHVString8C& name) const
{
#ifdef UNICODE
SHVString str = name.ToStrT();
#else
const SHVString8C& str = name;
#endif
size_t idx = 0;
	for (; Attributes && Attributes[idx] && SHVStringC::StrCmp(str.GetBufferConst(), Attributes[idx]); idx+=2);
	if (Attributes && Attributes[idx])
		return XML_Char2String8(Attributes[idx+1]);
	else
		return SHVString8().ReleaseBuffer();
}

/*************************************
 * GetCharacterData8
 *************************************/
SHVStringBuffer8 SHVXmlReaderImpl::GetValue8() const
{
#ifdef UNICODE
SHVString16 str;
size_t p = 0;
	if (!ValueCol.IsNull())
		ValueCol->ReadString16(str, ValueCol->GetSize() / sizeof(SHVTChar), p);
	return str.ToStr8();
#else
SHVString8 str;
size_t p = 0;
	if (!ValueCol.IsNull())
		ValueCol->ReadString8(str, ValueCol->GetSize() /sizeof(SHVTChar), p);
	return str.ReleaseBuffer();
#endif
}

/*************************************
 * GetElementNameUTF8
 *************************************/
SHVStringBufferUTF8 SHVXmlReaderImpl::GetElementNameUTF8() const
{
	return XML_Char2StringUTF8(Value);
}

/*************************************
 * GetAttributeNameUTF8
 *************************************/
SHVStringBufferUTF8 SHVXmlReaderImpl::GetAttributeNameUTF8(size_t idx) const
{
	return XML_Char2StringUTF8(Attributes[idx*2]);
}

/*************************************
 * GetAttributeValueUTF8
 *************************************/
SHVStringBufferUTF8 SHVXmlReaderImpl::GetAttributeValueUTF8(size_t idx) const
{
	return XML_Char2StringUTF8(Attributes[idx*2+1]);
}

/*************************************
 * GetAttributeValueByNameUTF8
 *************************************/
SHVStringBufferUTF8 SHVXmlReaderImpl::GetAttributeValueByNameUTF8(const SHVStringUTF8C& name) const
{
SHVString str = name.ToStrT();
size_t idx = 0;
	for (; Attributes && Attributes[idx] && SHVStringC::StrCmp(str.GetBufferConst(), Attributes[idx]); idx+=2);
	if (Attributes && Attributes[idx])
		return XML_Char2StringUTF8(Attributes[idx+1]);
	else
		return SHVStringUTF8().ReleaseBuffer();
}

/*************************************
 * GetCharacterDataUTF8
 *************************************/
SHVStringBufferUTF8 SHVXmlReaderImpl::GetValueUTF8() const
{
	return GetValue8().ToStrUTF8();
}

/*************************************
 * ParseDirect
 *************************************/
SHVBool SHVXmlReaderImpl::ParseDirect(const char* buffer, int actualLen, bool isFinal)
{
bool more = true;
SHVBool retVal = SHVBool::True;
bool status;

	CurrPos = 0;
	while (retVal && more)
	{
		more = false;
		status = XML_Parse(Parser, buffer + CurrPos, actualLen, (isFinal ? 1 : 0)) == XML_STATUS_OK;
		if (!status && MultiDocument && XML_GetErrorCode(Parser) == XML_ERROR_JUNK_AFTER_DOC_ELEMENT && DocLevel == 0)
		{
			more = true;
			actualLen -= CurrPos;
			XML_ParserReset(Parser, EncodingExpat);
			InitializeExpat();
		}
		else
			retVal.SetError(XML_GetErrorCode(Parser));
	}
	return retVal;
}

/*************************************
 * Parse
 *************************************/
SHVBool SHVXmlReaderImpl::Parse(SHVStreamIn& stream)
{
void* buffer = malloc(BufferSize);
size_t actualLen;
SHVBool retVal = SHVBool::True;
	Attributes = NULL;
	Value = NULL;
	while (stream.ReadBuffer(buffer, BufferSize, actualLen) && retVal)
	{
		retVal = ParseDirect((const char*) buffer, (int) actualLen, stream.Eof());
	}
	free(buffer);
	return retVal;
}

int SHVXmlReaderImpl::GetErrorCode() const
{
	return XML_GetErrorCode(Parser);
}

/*************************************
 * SetStartElementCallBack
 *************************************/
void SHVXmlReaderImpl::SetStartElementCallBack(SHVXmlReaderCallbackBase* callback)
{
	if (StartElementCallback)
		delete StartElementCallback;
	StartElementCallback = callback;
}

/*************************************
 * SetEndElementCallBack
 *************************************/
void SHVXmlReaderImpl::SetEndElementCallBack(SHVXmlReaderCallbackBase* callback)
{
	if (EndElementCallback)
		delete EndElementCallback;
	EndElementCallback = callback;
}

/*************************************
 * SetValueCallback
 *************************************/
void SHVXmlReaderImpl::SetValueCallback(SHVXmlReaderCallbackBase* callback)
{
	if (ValueCallback)
		delete ValueCallback;
	ValueCallback = callback;
}

/*************************************
 * SetMultidocument
 *************************************/
void SHVXmlReaderImpl::SetMultidocument(bool val)
{
	MultiDocument = val;
}

/*************************************
 * GetMultidocument
 *************************************/
bool SHVXmlReaderImpl::GetMultidocument() const
{
	return MultiDocument;
}

/*************************************
 * InitializeExpat
 *************************************/
void SHVXmlReaderImpl::InitializeExpat()
{
	XML_SetElementHandler(Parser,
		&SHVXmlReaderImpl::StartElementHandler,
		&SHVXmlReaderImpl::EndElementHandler);
	XML_SetCharacterDataHandler(Parser, &SHVXmlReaderImpl::DefaultHandler);
	XML_SetUserData(Parser, this);
	DocLevel = 0;
	CurrPos = 0;
}

/*************************************
 * StartElementHandler
 *************************************/
void SHVXmlReaderImpl::StartElementHandler(void *userData, const XML_Char *name, const XML_Char **atts)
{
SHVXmlReaderImpl* self = (SHVXmlReaderImpl*) userData;
	if (!self->ValueCol.IsNull())
	{
		if (self->ValueCallback)
			self->ValueCallback->PerformCallback(*self);
		self->ValueCol = NULL;
		self->ValuePos = 0;
	}
	self->Value = name;
	self->Attributes = atts;
	if (self->StartElementCallback)
		self->StartElementCallback->PerformCallback(*self);
	self->Value = NULL;
	self->Attributes = NULL;
	self->DocLevel++;
}

/*************************************
 * EndElementHandler
 *************************************/
void SHVXmlReaderImpl::EndElementHandler(void *userData, const XML_Char *name)
{
SHVXmlReaderImpl* self = (SHVXmlReaderImpl*) userData;
	if (!self->ValueCol.IsNull())
	{
		if (self->ValueCallback)
			self->ValueCallback->PerformCallback(*self);
		self->ValueCol = NULL;
		self->ValuePos = 0;
	}
	self->Value = name;
	if (self->EndElementCallback)
		self->EndElementCallback->PerformCallback(*self);
	self->Value = NULL;
	self->DocLevel--;
	self->CurrPos = XML_GetCurrentByteIndex(self->Parser) + XML_GetCurrentByteCount(self->Parser);
}

/*************************************
 * CharacterDataHandler
 *************************************/
void SHVXmlReaderImpl::DefaultHandler(void *userData, const XML_Char *s, int len)
{
SHVXmlReaderImpl* self = (SHVXmlReaderImpl*) userData;
	if (self->ValueCol.IsNull())
		self->ValueCol = new SHVBufferStream();
	self->ValueCol->WriteBytes((const SHVByte*)s, len * sizeof(SHVTChar), self->ValuePos);
}

/*************************************
 * XML_Char2String16
 *************************************/
SHVStringBuffer16 SHVXmlReaderImpl::XML_Char2String16(const XML_Char *str) const
{
#ifdef UNICODE
	return SHVString(str).ReleaseBuffer();
#else
	return SHVStringUTF8(str).ToStr16();
#endif
}

/*************************************
 * XML_Char2String8
 *************************************/
SHVStringBuffer8 SHVXmlReaderImpl::XML_Char2String8(const XML_Char *str) const
{
#ifdef UNICODE
	return SHVString(str).ToStr8();
#else
	return SHVStringUTF8(str).ToStr8();
#endif
}

/*************************************
 * XML_Char2StringUTF8
 *************************************/
SHVStringBufferUTF8 SHVXmlReaderImpl::XML_Char2StringUTF8(const XML_Char *str) const
{
#ifdef UNICODE
	return SHVString(str).ToStrUTF8();
#else
	return SHVStringUTF8(str).ReleaseBuffer();
#endif
}

