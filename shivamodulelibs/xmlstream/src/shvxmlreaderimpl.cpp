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
	return ValueCol.ToStr16();
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
	return ValueCol.ToStr8();
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
	return ValueCol.ToStrUTF8();
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
	if (self->ValueCol.GetLength())
	{
		if (self->ValueCallback)
			self->ValueCallback->PerformCallback(*self);
		self->ValueCol = NULL;
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
	if (self->ValueCol.GetLength())
	{
		if (self->ValueCallback)
			self->ValueCallback->PerformCallback(*self);
		self->ValueCol = NULL;
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
SHVString Str;
	Str.SetBufferSize(len + 1);
	Str[0] = 0;
	Str.AddChars(s, len);
	self->ValueCol += Str;
}

/*************************************
 * XML_Char2String16
 *************************************/
SHVStringBuffer16 SHVXmlReaderImpl::XML_Char2String16(const XML_Char *str) const
{
#ifdef UNICODE
	return SHVString16(str).ReleaseBuffer();
#else
	return SHVString8(str).ToStr16();
#endif
}

/*************************************
 * XML_Char2String8
 *************************************/
SHVStringBuffer8 SHVXmlReaderImpl::XML_Char2String8(const XML_Char *str) const
{
#ifdef UNICODE
	return SHVString16(str).ToStr8();
#else
	return SHVString8(str).ReleaseBuffer();
#endif
}

/*************************************
 * XML_Char2StringUTF8
 *************************************/
SHVStringBufferUTF8 SHVXmlReaderImpl::XML_Char2StringUTF8(const XML_Char *str) const
{
#ifdef UNICODE
	return SHVString16(str).ToStrUTF8();
#else
	return SHVString8(str).ToStrUTF8();
#endif
}

