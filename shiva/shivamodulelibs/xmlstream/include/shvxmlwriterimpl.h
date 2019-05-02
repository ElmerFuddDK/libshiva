#ifndef __SHIVA_XMLSTREAM_WRITE_IMPL_H
#define __SHIVA_XMLSTREAM_WRITE_IMPL_H

#include "../../../include/modules/xmlstream/shvxmlwriter.h"
#include "../../../include/utils/shvlist.h"
#include "../../../include/utils/shvptrcontainer.h"
#include "../../../include/utils/shvstreamout.h"


//-=========================================================================================================
/// SHVXmlWriter class - This class is the interface for the class that writes xml.
/**
 * This is a very simple, cut to the bone xml writer class.
 */
class SHVXmlWriterImpl: public SHVXmlWriter
{
public:
	SHVXmlWriterImpl(SHVXmlWriter::WriterEncoding enc);
	virtual ~SHVXmlWriterImpl();

	virtual void WriteStartElement16(SHVStreamOut& Streamout, const SHVString16C& elementName);
	virtual void WriteAttribute16(SHVStreamOut& Streamout, const SHVString16C& attrName, const SHVString16C& value);
	virtual void WriteText16(SHVStreamOut& Streamout, const SHVString16C& text);

	virtual void WriteStartElement8(SHVStreamOut& Streamout, const SHVString8C& elementName);
	virtual void WriteAttribute8(SHVStreamOut& Streamout, const SHVString8C& attrName, const SHVString8C& value);
	virtual void WriteText8(SHVStreamOut& Streamout, const SHVString8C& text);

	virtual void WriteStartElementUTF8(SHVStreamOut& Streamout, const SHVStringUTF8C& elementName);
	virtual void WriteAttributeUTF8(SHVStreamOut& Streamout, const SHVStringUTF8C& attrName, const SHVStringUTF8C& value);
	virtual void WriteTextUTF8(SHVStreamOut& Streamout, const SHVStringUTF8C& text);

	virtual void WriteEndElement(SHVStreamOut& Streamout);
private:
	void Push16(const SHVString16C& tag);
	void Push8(const SHVString8C& tag);
	void PushUTF8(const SHVStringUTF8C& tag);
	SHVStringBuffer16 Pop16(bool& shortClose);
	SHVStringBuffer8 Pop8(bool& shortClose);
	SHVStringBufferUTF8 PopUTF8(bool& shortClose);
	void ClearCurrentShortClose();
	bool CurrentShortClose() const;
	void InternalWriteText16(SHVStreamOut& Streamout, const SHVString16C& text);
	void InternalWriteText8(SHVStreamOut& Streamout, const SHVString8C& text);
	void InternalWriteTextUTF8(SHVStreamOut& Streamout, const SHVStringUTF8C& text);
private:
	enum {
		Internal8,
		Internal16,
		InternalUTF8
	} InternalEncoding;
	struct TagStackElem
	{
		bool ShortClose;
		union {
			SHVString16* Tag16;
			SHVString8* Tag8;
			SHVStringUTF8* TagUTF8;
		} Tag;
	};
	SHVList<TagStackElem*> TagStack;
};

#endif
