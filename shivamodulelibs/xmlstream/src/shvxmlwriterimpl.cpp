#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../include/xmlstreamimpl/shvxmlwriter_impl.h"

/*************************************
 * Constructor
 *************************************/
SHVXmlWriter_Impl::SHVXmlWriter_Impl(SHVXmlWriter::WriterEncoding enc)
{
	switch (enc)
	{
#ifdef UNICODE
	case SHVXmlWriter::WriterEncodingNative:
#endif
	case SHVXmlWriter::WriterEncodingUTF16:
		InternalEncoding = Internal16;
		break;
#ifndef UNICODE
	case SHVXmlWriter::WriterEncodingNative:
		InternalEncoding = Internal8;
		break;
#endif
	case SHVXmlWriter::WriterEncodingUTF8:
		InternalEncoding  = InternalUTF8;
		break;
	default:
#ifdef UNICODE
		InternalEncoding = Internal16;
#else
		InternalEncoding = Internal8;
#endif
		break;
	}
}

/*************************************
 * Destructor
 *************************************/
SHVXmlWriter_Impl::~SHVXmlWriter_Impl()
{
SHVListPos p = NULL;
	while (TagStack.MoveNext(p))
	{
		switch (InternalEncoding)
		{
		case Internal8:
			delete TagStack.GetAt(p)->Tag.Tag8;
			break;
		case Internal16:
			delete TagStack.GetAt(p)->Tag.Tag16;
			break;
		case InternalUTF8:
			delete TagStack.GetAt(p)->Tag.TagUTF8;
			break;
		}
	}
}

/*************************************
 * WriteStartElement16
 *************************************/
void SHVXmlWriter_Impl::WriteStartElement16(SHVStreamOut& Streamout, const SHVString16C& elementName)
{
	switch (InternalEncoding)
	{
	case Internal8:
		WriteStartElement8(Streamout, elementName.ToStr8());
		break;
	case Internal16:
		if (CurrentShortClose())
		{
			Streamout.WriteChar16(L'>');
			ClearCurrentShortClose();
		}
		Push16(elementName);
		Streamout.WriteChar16(L'<');
		Streamout.WriteString16(elementName.GetBufferConst());
		break;
	case InternalUTF8:
		WriteStartElementUTF8(Streamout, elementName.ToStrUTF8());
		break;
	}
}


/*************************************
 * WriteAttribute16
 *************************************/
void SHVXmlWriter_Impl::WriteAttribute16(SHVStreamOut& Streamout, const SHVString16C& attrName, const SHVString16C& value)
{
	switch (InternalEncoding)
	{
	case Internal8:
		WriteAttribute8(Streamout, attrName.ToStr8(), value.ToStr8());
		break;
	case Internal16:
		Streamout.WriteChar16(L' ');
		Streamout.WriteString16(attrName.GetBufferConst());
		Streamout.WriteString16(L"=\"");
		InternalWriteText16(Streamout, value);
		Streamout.WriteChar16(L'"');
		break;
	case InternalUTF8:
		WriteAttributeUTF8(Streamout, attrName.ToStrUTF8(), value.ToStrUTF8());
		break;
	}	
}

/*************************************
 * WriteText16
 *************************************/
void SHVXmlWriter_Impl::WriteText16(SHVStreamOut& Streamout, const SHVStringC& text)
{
	if (CurrentShortClose())
	{
		switch (InternalEncoding)
		{
		case Internal16:
			Streamout.WriteChar16(L'>');
			break;
		case Internal8:
		case InternalUTF8:
			Streamout.WriteChar8('>');
			break;
		}
		ClearCurrentShortClose();
	}
	InternalWriteText16(Streamout, text);
}

/*************************************
 * WriteStartElement8
 *************************************/
void SHVXmlWriter_Impl::WriteStartElement8(SHVStreamOut& Streamout, const SHVString8C& elementName)
{
	switch (InternalEncoding)
	{
	case Internal8:
		if (CurrentShortClose())
		{
			Streamout.WriteChar8('>');
			ClearCurrentShortClose();
		}
		Push8(elementName);
		Streamout.WriteChar8('<');
		Streamout.WriteString8(elementName.GetBufferConst());
		break;
	case Internal16:
		WriteStartElement(Streamout, elementName.ToStr16());
		break;
	case InternalUTF8:
		WriteStartElementUTF8(Streamout, elementName.ToStrUTF8());
		break;
	}
}

/*************************************
 * WriteAttribute8
 *************************************/
void SHVXmlWriter_Impl::WriteAttribute8(SHVStreamOut& Streamout, const SHVString8C& attrName, const SHVString8C& value)
{
	switch (InternalEncoding)
	{
	case Internal8:
		Streamout.WriteChar8(' ');
		Streamout.WriteString8(attrName.GetBufferConst());
		Streamout.WriteString8("=\"");
		InternalWriteText8(Streamout, value);
		Streamout.WriteChar8('"');
		break;
	case Internal16:
		WriteAttribute16(Streamout, attrName.ToStr16(), value.ToStr16());
		break;
	case InternalUTF8:
		WriteAttributeUTF8(Streamout, attrName.ToStrUTF8(), value.ToStrUTF8());
		break;
	}
}

/*************************************
 * WriteText8
 *************************************/
void SHVXmlWriter_Impl::WriteText8(SHVStreamOut& Streamout, const SHVString8C& text)
{
	if (CurrentShortClose())
	{
		switch (InternalEncoding)
		{
		case Internal16:
			Streamout.WriteChar16(L'>');
			break;
		case Internal8:
		case InternalUTF8:
			Streamout.WriteChar8('>');
			break;
		}
		ClearCurrentShortClose();
	}
	InternalWriteText8(Streamout, text);
}

/*************************************
 * WriteStartElementUTF8
 *************************************/
void SHVXmlWriter_Impl::WriteStartElementUTF8(SHVStreamOut& Streamout, const SHVStringUTF8C& elementName)
{
	switch (InternalEncoding)
	{
	case Internal8:
		WriteStartElement8(Streamout, elementName.ToStr8());
		break;
	case Internal16:
		WriteStartElement16(Streamout, elementName.ToStr16());
		break;
	case InternalUTF8:
		if (CurrentShortClose())
		{
			Streamout.WriteChar8('>');
			ClearCurrentShortClose();
		}
		PushUTF8(elementName);
		Streamout.WriteChar8('<');
		Streamout.WriteString8(elementName.GetBufferConst());
		break;
	}
}

/*************************************
 * WriteAttributeUTF8
 *************************************/
void SHVXmlWriter_Impl::WriteAttributeUTF8(SHVStreamOut& Streamout, const SHVStringUTF8C& attrName, const SHVStringUTF8C& value)
{
	switch (InternalEncoding)
	{
	case Internal8:
		WriteAttribute8(Streamout, attrName.ToStr8(), value.ToStr8());
		break;
	case Internal16:
		WriteAttribute16(Streamout, attrName.ToStr16(), value.ToStr16());
		break;
	case InternalUTF8:
		Streamout.WriteChar8(' ');
		Streamout.WriteString8(attrName.GetBufferConst());
		Streamout.WriteString8("=\"");
		InternalWriteTextUTF8(Streamout, value);
		Streamout.WriteChar8('"');
		break;
	}
}

/*************************************
 * WriteTextUTF8
 *************************************/
void SHVXmlWriter_Impl::WriteTextUTF8(SHVStreamOut& Streamout, const SHVStringUTF8C& text)
{
	if (CurrentShortClose())
	{
		switch (InternalEncoding)
		{
		case Internal16:
			Streamout.WriteChar16(L'>');
			break;
		case Internal8:
		case InternalUTF8:
			Streamout.WriteChar8('>');
			break;
		}
		ClearCurrentShortClose();
	}
	InternalWriteTextUTF8(Streamout, text);
}

/*************************************
 * WriteEndElement
 *************************************/
void SHVXmlWriter_Impl::WriteEndElement(SHVStreamOut& Streamout)
{
bool ShortClose;
	switch (InternalEncoding)
	{
	case Internal8:
		{
		SHVString8 elementName = Pop8(ShortClose);
			if (!ShortClose)
			{
				Streamout.WriteString8("</");
				Streamout.WriteString8(elementName.GetBufferConst());
				Streamout.WriteChar8('>');
			}
			else
				Streamout.WriteString8("/>");
		}
		break;
	case Internal16:
		{
		SHVString16 elementName = Pop16(ShortClose);
			if (!ShortClose)
			{
				Streamout.WriteString16(L"</");
				Streamout.WriteString16(elementName.GetBufferConst());
				Streamout.WriteChar16('>');
			}
			else
				Streamout.WriteString16(L"/>");
		}
		break;
	case InternalUTF8:
		{
		SHVStringUTF8 elementName = PopUTF8(ShortClose);
			if (!ShortClose)
			{
				Streamout.WriteString8("</");
				Streamout.WriteString8(elementName.GetBufferConst());
				Streamout.WriteChar8('>');
			}
			else
				Streamout.WriteString8("/>");
		}
		break;
	}
}

/*************************************
 * Push16
 *************************************/
void SHVXmlWriter_Impl::Push16(const SHVString16C& tag)
{
	TagStackElem* elm = new TagStackElem();
	elm->ShortClose = true;
	elm->Tag.Tag16 = new SHVString16(tag.GetBufferConst());
	TagStack.AddTail(elm);
}

/*************************************
 * Push8
 *************************************/
void SHVXmlWriter_Impl::Push8(const SHVString8C& tag)
{
	TagStackElem* elm = new TagStackElem();
	elm->ShortClose = true;
	elm->Tag.Tag8 = new SHVString8(tag.GetBufferConst());
	TagStack.AddTail(elm);
}

/*************************************
 * PushUTF8
 *************************************/
void SHVXmlWriter_Impl::PushUTF8(const SHVStringUTF8C& tag)
{
	TagStackElem* elm = new TagStackElem();
	elm->ShortClose = true;
	elm->Tag.TagUTF8 = new SHVStringUTF8(tag.GetBufferConst());
	TagStack.AddTail(elm);
}

/*************************************
 * Pop16
 *************************************/
SHVStringBuffer16 SHVXmlWriter_Impl::Pop16(bool& shortClose)
{
TagStackElem* elm = TagStack.PopTail();
SHVStringBuffer16 retVal = elm->Tag.Tag16->ReleaseBuffer();
	shortClose = elm->ShortClose;
	delete elm->Tag.Tag16;
	delete elm;
	return retVal;
}

/*************************************
 * Pop8
 *************************************/
SHVStringBuffer8 SHVXmlWriter_Impl::Pop8(bool& shortClose)
{
TagStackElem* elm = TagStack.PopTail();
SHVStringBuffer8 retVal = elm->Tag.Tag8->ReleaseBuffer();
	shortClose = elm->ShortClose;
	delete elm->Tag.Tag8;
	delete elm;
	return retVal;
}

/*************************************
 * PopUTF8
 *************************************/
SHVStringBufferUTF8 SHVXmlWriter_Impl::PopUTF8(bool& shortClose)
{
TagStackElem* elm = TagStack.PopTail();
SHVStringBufferUTF8 retVal = elm->Tag.TagUTF8->ReleaseBuffer();
	shortClose = elm->ShortClose;
	delete elm->Tag.TagUTF8;
	delete elm;
	return retVal;
}

/*************************************
 * ClearCurrentShortClose
 *************************************/
void SHVXmlWriter_Impl::ClearCurrentShortClose()
{
SHVListPos tail = TagStack.GetTailPosition();
	if (tail)
	{
		TagStack.GetAt(tail)->ShortClose = false;
	}
}

/*************************************
 * CurrentShortClose
 *************************************/
const bool SHVXmlWriter_Impl::CurrentShortClose() const
{
SHVListPos tail = ((SHVList<TagStackElem>*) &TagStack)->GetTailPosition();
	if (tail)
	{
		return TagStack.GetAt(tail)->ShortClose;
	}
	else
		return false;
}

/*************************************
 * InternalWriteText16
 *************************************/
void SHVXmlWriter_Impl::InternalWriteText16(SHVStreamOut& Streamout, const SHVStringC& text)
{
size_t len;
	switch (InternalEncoding)
	{
	case InternalUTF8:
		WriteTextUTF8(Streamout, text.ToStrUTF8());
		break;
	case Internal8:
		WriteText8(Streamout, text.ToStr8());
	case Internal16:
		len = text.GetLength();
		for (size_t strpos = 0; strpos < len; strpos++)
		{
			switch(text.GetBufferConst()[strpos])
			{
			case '&':
				Streamout.WriteString16(L"&amp;");
				break;
			case '<':
				Streamout.WriteString16(L"&lt;");
				break;
			case '>':
				Streamout.WriteString16(L"&gt;");
				break;
			case '"':
				Streamout.WriteString16(L"&quot;");
				break;
			case 9:
			case 10:
			case 13:
				Streamout.WriteString16(L"&#");
				Streamout.WriteString16(SHVString16::LongToString(text.GetBufferConst()[strpos]).GetBufferConst());
				Streamout.WriteChar16(L';');
				break;
			default:
				Streamout.WriteChar16(text.GetBufferConst()[strpos]);
			}
		}
		break;
	}	
}

/*************************************
 * InternalWriteText8
 *************************************/
void SHVXmlWriter_Impl::InternalWriteText8(SHVStreamOut& Streamout, const SHVString8C& text)
{
size_t len;
	switch (InternalEncoding)
	{		
	case Internal8:
		len = text.GetLength();
		for (size_t strpos = 0; strpos < len; strpos++)
		{
			switch(text.GetBufferConst()[strpos])
			{
			case '&':
				Streamout.WriteString8("&amp;");
				break;
			case '<':
			    Streamout.WriteString8("&lt;");
			    break;
			case '>':
			    Streamout.WriteString8("&gt;");
			    break;
			case '"':
			    Streamout.WriteString8("&quot;");
			    break;
			case 9:
			case 10:
			case 13:
				Streamout.WriteString8("&#");
				Streamout.WriteString8(SHVStringUTF8::LongToString(text.GetBufferConst()[strpos]).GetBufferConst());
				Streamout.WriteChar8(';');
				break;
			default:
				Streamout.WriteChar8(text.GetBufferConst()[strpos]);
			}
		}
		break;
	case Internal16:
		WriteText16(Streamout, text.ToStr16());
		break;
	case InternalUTF8:
		WriteTextUTF8(Streamout, text.ToStrUTF8());
		break;
	}
}

/*************************************
 * InternalWriteTextUTF8
 *************************************/
void SHVXmlWriter_Impl::InternalWriteTextUTF8(SHVStreamOut& Streamout, const SHVStringUTF8C& text)
{
size_t len;
	switch (InternalEncoding)
	{
	case Internal8:
		WriteText8(Streamout, text.ToStr8());
		break;
	case Internal16:
		WriteText16(Streamout, text.ToStr16());
		break;
	case InternalUTF8:
		len = SHVString8::StrLen(text.GetBufferConst());
		for (size_t strpos = 0; strpos < len; strpos++)
		{
			switch(text.GetBufferConst()[strpos])
			{
			case '&':
				Streamout.WriteString8("&amp;");
				break;
			case '<':
			    Streamout.WriteString8("&lt;");
			    break;
			case '>':
			    Streamout.WriteString8("&gt;");
			    break;
			case '"':
			    Streamout.WriteString8("&quot;");
			    break;
			case 9:
			case 10:
			case 13:
				Streamout.WriteString8("&#");
				Streamout.WriteString8(SHVStringUTF8::LongToString(text.GetBufferConst()[strpos]).GetBufferConst());
				Streamout.WriteChar8(';');
				break;
			default:
				Streamout.WriteChar8(text.GetBufferConst()[strpos]);
			}
		}
		break;
	}
}
