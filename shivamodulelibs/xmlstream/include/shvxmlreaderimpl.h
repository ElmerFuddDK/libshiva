#ifndef __SHIVA_XMLSTREAM_READER_IMPL_H
#define __SHIVA_XMLSTREAM_READER_IMPL_H

#include "../../include/shvxmlreader.h"
#include "../../src/expat/expat.h"

//-=========================================================================================================
/// SHVXmlReader_Impl class - Implementation of an simple xml parser
/**
 * This is a very simple, cut to the bone xml parser class.
 */

class SHVXmlReader_Impl: public SHVXmlReader
{
public:
	SHVXmlReader_Impl(ParserEncoding enc, size_t bufferSize = 1024);
	virtual ~SHVXmlReader_Impl();

	virtual size_t CalculateAttributeCount() const;

	virtual SHVStringBuffer16 GetElementName16() const;
	virtual SHVStringBuffer16 GetAttributeName16(size_t idx) const;
	virtual SHVStringBuffer16 GetAttributeValue16(size_t idx) const;
	virtual SHVStringBuffer16 GetAttributeValueByName16(const SHVString16C& name) const;
	virtual SHVStringBuffer16 GetValue16() const;

	virtual SHVStringBuffer8 GetElementName8() const;
	virtual SHVStringBuffer8 GetAttributeName8(size_t idx) const;
	virtual SHVStringBuffer8 GetAttributeValue8(size_t idx) const;
	virtual SHVStringBuffer8 GetAttributeValueByName8(const SHVString8C& name) const;
	virtual SHVStringBuffer8 GetValue8() const;

	virtual SHVStringBufferUTF8 GetElementNameUTF8() const;
	virtual SHVStringBufferUTF8 GetAttributeNameUTF8(size_t idx) const;
	virtual SHVStringBufferUTF8 GetAttributeValueUTF8(size_t idx) const;
	virtual SHVStringBufferUTF8 GetAttributeValueByNameUTF8(const SHVStringUTF8C& name) const;
	virtual SHVStringBufferUTF8 GetValueUTF8() const;

	virtual SHVBool Parse(SHVStreamIn& stream);
	virtual int GetErrorCode() const;
	virtual void SetStartElementCallBack(SHVXmlReaderCallbackBase* callback);
	virtual void SetEndElementCallBack(SHVXmlReaderCallbackBase* callback);
	virtual void SetValueCallback(SHVXmlReaderCallbackBase* callback);
private:
	static void StartElementHandler(void *userData, const XML_Char *name, const XML_Char **atts);
	static void EndElementHandler(void *userData, const XML_Char *name);
	static void DefaultHandler(void *userData, const XML_Char *s, int len);

	SHVStringBuffer16 XML_Char2String16(const XML_Char *str) const;
	SHVStringBuffer8 XML_Char2String8(const XML_Char *str) const;
	SHVStringBufferUTF8 XML_Char2StringUTF8(const XML_Char *str) const;

	SHVXmlReaderCallbackBase* StartElementCallback;
	SHVXmlReaderCallbackBase* EndElementCallback;
	SHVXmlReaderCallbackBase* ValueCallback;
	SHVXmlReader::ParserEncoding Encoding;
	size_t BufferSize;
	SHVString ValueCol;
//  Expat specific data
	const XML_Char **Attributes;
	const XML_Char *Value;
	int Len;
	XML_Parser Parser;
};


#endif
