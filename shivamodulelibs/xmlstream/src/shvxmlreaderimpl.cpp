#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../include/xmlstreamimpl/shvxmlreader_impl.h"


/*************************************
 * Constructor
 *************************************/
SHVXmlReader_Impl::SHVXmlReader_Impl(ParserEncoding enc, size_t bufferSize)
{
const TCHAR* encoding;
	StartElementCallback = EndElementCallback = ValueCallback = NULL;
	BufferSize = bufferSize;
	switch (enc)
	{
	case ParserEncodingUTF8:
		encoding = _T("UTF-8");
		break;
#ifndef UNICODE
	case ParserEncodingNative:
		encoding = _T("ISO-8859-1");
		break;
#else
	// native encoding is UTF16 when unicode is defined
	case ParserEncodingNative:
#endif
	case ParserEncodingUTF16:
		encoding = _T("UTF-16");
		break;
	}
	Encoding = enc;
	Parser = XML_ParserCreate(encoding);
	XML_SetElementHandler(Parser,
		&SHVXmlReader_Impl::StartElementHandler,
		&SHVXmlReader_Impl::EndElementHandler);
	XML_SetCharacterDataHandler(Parser, &SHVXmlReader_Impl::DefaultHandler);
	XML_SetUserData(Parser, this);
}

/*************************************
 * Destructor
 *************************************/
SHVXmlReader_Impl::~SHVXmlReader_Impl()
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
size_t SHVXmlReader_Impl::CalculateAttributeCount() const
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
SHVStringBuffer16 SHVXmlReader_Impl::GetElementName16() const
{
	return XML_Char2String16(Value);
}

/*************************************
 * GetAttributeName16
 *************************************/
SHVStringBuffer16 SHVXmlReader_Impl::GetAttributeName16(size_t idx) const
{
	return XML_Char2String16(Attributes[idx*2]);
}

/*************************************
 * GetAttributeValue16
 *************************************/
SHVStringBuffer16 SHVXmlReader_Impl::GetAttributeValue16(size_t idx) const
{
	return XML_Char2String16(Attributes[idx*2+1]);
}

/*************************************
 * GetAttributeValueByName16
 *************************************/
SHVStringBuffer16 SHVXmlReader_Impl::GetAttributeValueByName16(const SHVString16C& name) const
{
size_t idx = 0;
	for (; Attributes && Attributes[idx] && SHVStringC::StrCmp(name.GetBufferConst(), Attributes[idx]); idx+=2);
	if (Attributes && Attributes[idx])
		return XML_Char2String16(Attributes[idx+1]);
	else
		return SHVString16().ReleaseBuffer();
}

/*************************************
 * GetCharacterData16
 *************************************/
SHVStringBuffer16 SHVXmlReader_Impl::GetValue16() const
{
	return SHVString(ValueCol).ReleaseBuffer();
}

/*************************************
 * GetElementName8
 *************************************/
SHVStringBuffer8 SHVXmlReader_Impl::GetElementName8() const
{
	return XML_Char2String8(Value);
}

/*************************************
 * GetAttributeName8
 *************************************/
SHVStringBuffer8 SHVXmlReader_Impl::GetAttributeName8(size_t idx) const
{
	return XML_Char2String8(Attributes[idx*2]);
}

/*************************************
 * GetAttributeValue8
 *************************************/
SHVStringBuffer8 SHVXmlReader_Impl::GetAttributeValue8(size_t idx) const
{
	return XML_Char2String8(Attributes[idx*2+1]);
}

/*************************************
 * GetAttributeValueByName8
 *************************************/
SHVStringBuffer8 SHVXmlReader_Impl::GetAttributeValueByName8(const SHVString8C& name) const
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
SHVStringBuffer8 SHVXmlReader_Impl::GetValue8() const
{
#ifdef UNICODE
	return ValueCol.ToStr8();
#else
	return ValueCol.GetSafeBuffer();
#endif
}

/*************************************
 * GetElementNameUTF8
 *************************************/
SHVStringBufferUTF8 SHVXmlReader_Impl::GetElementNameUTF8() const
{
	return XML_Char2StringUTF8(Value);
}

/*************************************
 * GetAttributeNameUTF8
 *************************************/
SHVStringBufferUTF8 SHVXmlReader_Impl::GetAttributeNameUTF8(size_t idx) const
{
	return XML_Char2StringUTF8(Attributes[idx*2]);
}

/*************************************
 * GetAttributeValueUTF8
 *************************************/
SHVStringBufferUTF8 SHVXmlReader_Impl::GetAttributeValueUTF8(size_t idx) const
{
	return XML_Char2StringUTF8(Attributes[idx*2+1]);
}

/*************************************
 * GetAttributeValueByNameUTF8
 *************************************/
SHVStringBufferUTF8 SHVXmlReader_Impl::GetAttributeValueByNameUTF8(const SHVStringUTF8C& name) const
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
SHVStringBufferUTF8 SHVXmlReader_Impl::GetValueUTF8() const
{
	return ValueCol.ToStrUTF8();
}

/*************************************
 * Parse
 *************************************/
SHVBool SHVXmlReader_Impl::Parse(SHVStreamIn& stream)
{
void* buffer = malloc(BufferSize);
size_t actualLen;
SHVBool retVal = SHVBool::True;
	Attributes = NULL;
	Value = NULL;
	while (stream.ReadBuffer(buffer, BufferSize, actualLen) && retVal)
	{
		retVal = XML_Parse(Parser, (const char*) buffer, (int) actualLen, stream.Eof()) == XML_STATUS_OK;
	}
	free(buffer);
	return retVal;
}

int SHVXmlReader_Impl::GetErrorCode() const
{
	return XML_GetErrorCode(Parser);
}

/*************************************
 * SetStartElementCallBack
 *************************************/
void SHVXmlReader_Impl::SetStartElementCallBack(SHVXmlReaderCallbackBase* callback)
{
	if (StartElementCallback)
		delete StartElementCallback;
	StartElementCallback = callback;
}

/*************************************
 * SetEndElementCallBack
 *************************************/
void SHVXmlReader_Impl::SetEndElementCallBack(SHVXmlReaderCallbackBase* callback)
{
	if (EndElementCallback)
		delete EndElementCallback;
	EndElementCallback = callback;
}

/*************************************
 * SetValueCallback
 *************************************/
void SHVXmlReader_Impl::SetValueCallback(SHVXmlReaderCallbackBase* callback)
{
	if (ValueCallback)
		delete ValueCallback;
	ValueCallback = callback;
}

/*************************************
 * StartElementHandler
 *************************************/
void SHVXmlReader_Impl::StartElementHandler(void *userData, const XML_Char *name, const XML_Char **atts)
{
SHVXmlReader_Impl* self = (SHVXmlReader_Impl*) userData;
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
}

/*************************************
 * EndElementHandler
 *************************************/
void SHVXmlReader_Impl::EndElementHandler(void *userData, const XML_Char *name)
{
SHVXmlReader_Impl* self = (SHVXmlReader_Impl*) userData;
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
}

/*************************************
 * CharacterDataHandler
 *************************************/
void SHVXmlReader_Impl::DefaultHandler(void *userData, const XML_Char *s, int len)
{
SHVXmlReader_Impl* self = (SHVXmlReader_Impl*) userData;
SHVString Str;
	Str.SetBufferSize(len + 1);
	Str[0] = 0;
	Str.AddChars(s, len);
	self->ValueCol += Str;
}

/*************************************
 * XML_Char2String16
 *************************************/
SHVStringBuffer16 SHVXmlReader_Impl::XML_Char2String16(const XML_Char *str) const
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
SHVStringBuffer8 SHVXmlReader_Impl::XML_Char2String8(const XML_Char *str) const
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
SHVStringBufferUTF8 SHVXmlReader_Impl::XML_Char2StringUTF8(const XML_Char *str) const
{
#ifdef UNICODE
	return SHVString16(str).ToStrUTF8();
#else
	return SHVString8(str).ToStrUTF8();
#endif
}

