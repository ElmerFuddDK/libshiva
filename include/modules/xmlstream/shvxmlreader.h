#ifndef __SHIVA_XMLSTREAM_READER_H
#define __SHIVA_XMLSTREAM_READER_H

#include "../../utils/shvstring.h"
#include "../../utils/shvstringutf8.h"
#include "../../utils/shvstreamin.h"
#include "../../utils/shvptrcontainer.h"

// forward declare
class SHVXmlReaderCallbackBase;

//-=========================================================================================================
/// SHVXmlReader class - This class is the interface for the class that parses an xml.
/**
 * This is a very simple, cut to the bone xml parser class.
 */

class SHVXmlReader
{
public:
	enum ParserEncoding {
		ParserEncodingUTF8,
		ParserEncodingUTF16,
		ParserEncodingNative
	};
	virtual ~SHVXmlReader() {}
	virtual size_t CalculateAttributeCount() const = 0;
	virtual SHVStringBuffer16 GetElementName16() const = 0;
	virtual SHVStringBuffer16 GetAttributeName16(size_t idx) const = 0;
	virtual SHVStringBuffer16 GetAttributeValue16(size_t idx) const = 0;
	virtual SHVStringBuffer16 GetAttributeValueByName16(const SHVString16C& name) const = 0;
	virtual SHVStringBuffer16 GetValue16() const = 0;

	virtual SHVStringBuffer8 GetElementName8() const = 0;
	virtual SHVStringBuffer8 GetAttributeName8(size_t idx) const = 0;
	virtual SHVStringBuffer8 GetAttributeValue8(size_t idx) const = 0;
	virtual SHVStringBuffer8 GetAttributeValueByName8(const SHVString8C& name) const = 0;
	virtual SHVStringBuffer8 GetValue8() const = 0;

	virtual SHVStringBufferUTF8 GetElementNameUTF8() const = 0;
	virtual SHVStringBufferUTF8 GetAttributeNameUTF8(size_t idx) const = 0;
	virtual SHVStringBufferUTF8 GetAttributeValueUTF8(size_t idx) const = 0;
	virtual SHVStringBufferUTF8 GetAttributeValueByNameUTF8(const SHVStringUTF8C& name) const = 0;
	virtual SHVStringBufferUTF8 GetValueUTF8() const = 0;

	inline SHVStringBuffer GetElementName() const;
	inline SHVStringBuffer GetAttributeName(size_t idx) const;
	inline SHVStringBuffer GetAttributeValue(size_t idx) const;
	inline SHVStringBuffer GetAttributeValueByName(const SHVStringC& name) const;
	inline SHVStringBuffer GetValue() const;

	virtual SHVBool Parse(SHVStreamIn& stream) = 0;
	virtual SHVBool ParseDirect(const char* buffer, int actualLen, bool isFinal) = 0;
	virtual int GetErrorCode() const = 0;
	virtual int GetParsedBytes() const = 0;
	virtual void SetStartElementCallBack(SHVXmlReaderCallbackBase* callback) = 0;
	virtual void SetEndElementCallBack(SHVXmlReaderCallbackBase* callback) = 0;
	virtual void SetValueCallback(SHVXmlReaderCallbackBase* callback) = 0;
	virtual void SetMultidocument(bool val) = 0;
	virtual bool GetMultidocument() const = 0;

};
typedef SHVPtrContainer<SHVXmlReader> SHVXmlReaderPtr;

//-=========================================================================================================
/// SHVXmlReaderCallbackBase class - Base class for the callback template class
/**
 * 
 */
class SHVXmlReaderCallbackBase
{
public:
	virtual void PerformCallback(SHVXmlReader& reader) = 0;
	virtual ~SHVXmlReaderCallbackBase() {}
};

//-=========================================================================================================
/// SHVXmlReaderCallback template class - callback template class
/**
 * 
 */
template<class T>
class SHVXmlReaderCallback: public SHVXmlReaderCallbackBase
{
public:
	typedef void (T::*Callback)(SHVXmlReader& reader);
	inline SHVXmlReaderCallback(T* target, Callback function);
	virtual void PerformCallback(SHVXmlReader& reader);	
private:
	Callback CallbackFunction;
	T* Target;
};

// ===================================== implementation - SHVXmlReader ===================================== //

/*************************************
 * GetElementName
 *************************************/
SHVStringBuffer SHVXmlReader::GetElementName() const
{
#ifdef UNICODE
	return GetElementName16();
#else
	return GetElementName8();
#endif
}

/*************************************
 * GetAttributeName
 *************************************/
SHVStringBuffer SHVXmlReader::GetAttributeName(size_t idx) const
{
#ifdef UNICODE
	return GetAttributeName16(idx);
#else
	return GetAttributeName8(idx);
#endif
}

/*************************************
 * GetAttributeValue
 *************************************/
SHVStringBuffer SHVXmlReader::GetAttributeValue(size_t idx) const
{
#ifdef UNICODE
	return GetAttributeValue16(idx);
#else
	return GetAttributeValue8(idx);
#endif
}

/*************************************
 * GetAttributeValueByName
 *************************************/
SHVStringBuffer SHVXmlReader::GetAttributeValueByName(const SHVStringC& name) const
{
#ifdef UNICODE
	return GetAttributeValueByName16(name);
#else
	return GetAttributeValueByName8(name);
#endif
}

/*************************************
 * GetCharacterData
 *************************************/
SHVStringBuffer SHVXmlReader::GetValue() const
{
#ifdef UNICODE
	return GetValue16();
#else
	return GetValue8();
#endif
}

// ================================= implementation - SHVXmlReaderCallback ================================= //
/*************************************
 * Constructor
 *************************************/
template<class T>
SHVXmlReaderCallback<T>::SHVXmlReaderCallback(T* target, Callback function)
{
	Target = target;
	CallbackFunction = function;
}
	
/*************************************
 * PerformCallback
 *************************************/
template<class T>
void SHVXmlReaderCallback<T>::PerformCallback(SHVXmlReader& reader)
{
	(Target->*CallbackFunction)(reader);
}

#endif
