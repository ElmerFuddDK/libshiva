#ifndef __SHIVA_XMLSTREAM_WRITE_H
#define __SHIVA_XMLSTREAM_WRITE_H

#include "../../utils/shvstring.h"
#include "../../utils/shvstringutf8.h"
#include "../../utils/shvstreamout.h"
#include "../../utils/shvptrcontainer.h"

//-=========================================================================================================
/// SHVXmlWriter class - This class is the interface for the class that writes xml.
/**
 * This is a very simple, cut to the bone xml writer class.
 */
class SHVXmlWriter
{
public:
	enum WriterEncoding {
		WriterEncodingUTF8,
		WriterEncodingUTF16,
		WriterEncodingNative,
		WriterEncoding8859_1
	};
	virtual ~SHVXmlWriter() {}
	virtual void WriteStartElement16(SHVStreamOut& Streamout, const SHVString16C& elementName) = 0;
	virtual void WriteAttribute16(SHVStreamOut& Streamout, const SHVString16C& attrName, const SHVString16C& value) = 0;
	virtual void WriteText16(SHVStreamOut& Streamout, const SHVString16C& text) = 0;

	virtual void WriteStartElement8(SHVStreamOut& Streamout, const SHVString8C& elementName) = 0;
	virtual void WriteAttribute8(SHVStreamOut& Streamout, const SHVString8C& attrName, const SHVString8C& value) = 0;
	virtual void WriteText8(SHVStreamOut& Streamout, const SHVString8C& text) = 0;

	virtual void WriteStartElementUTF8(SHVStreamOut& Streamout, const SHVStringUTF8C& elementName) = 0;
	virtual void WriteAttributeUTF8(SHVStreamOut& Streamout, const SHVStringUTF8C& attrName, const SHVStringUTF8C& value) = 0;
	virtual void WriteTextUTF8(SHVStreamOut& Streamout, const SHVStringUTF8C& text) = 0;

	inline void WriteStartElement(SHVStreamOut& Streamout, const SHVStringC& elementName);
	virtual void WriteEndElement(SHVStreamOut& Streamout) = 0;
	inline void WriteAttribute(SHVStreamOut& Streamout, const SHVStringC& attrName, const SHVStringC& value);
	inline void WriteText(SHVStreamOut& Streamout, const SHVStringC& text);
};
typedef SHVPtrContainer<SHVXmlWriter> SHVXmlWriterPtr;


// ===================================== implementation - SHVXmlWriter ===================================== //

void SHVXmlWriter::WriteStartElement(SHVStreamOut& Streamout, const SHVStringC& elementName)
{
#ifdef UNICODE
	WriteStartElement16(Streamout, elementName);
#else
	WriteStartElement8(Streamout, elementName);
#endif
}

void SHVXmlWriter::WriteAttribute(SHVStreamOut& Streamout, const SHVStringC& attrName, const SHVStringC& value)
{
#ifdef UNICODE
	WriteAttribute16(Streamout, attrName, value);
#else
	WriteAttribute8(Streamout, attrName, value);
#endif
}

void SHVXmlWriter::WriteText(SHVStreamOut& Streamout, const SHVStringC& text)
{
#ifdef UNICODE
	WriteText16(Streamout, text);
#else
	WriteText8(Streamout, text);
#endif
}

#endif