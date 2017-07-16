#ifndef __SHIVA_JSONREADER_H
#define __SHIVA_JSONREADER_H

#include "../../utils/shvtime.h"
#include "../../utils/shvstring.h"
#include "../../utils/shvstreamin.h"
#include "../../utils/shvptrcontainer.h"

// forward declare
class SHVJsonReaderCallbackBase;
class SHVJsonKeyValuePair;

//-=========================================================================================================
/// SHVJsonReader class - This class is the interface for the class that parses json.
/**
 * This is a very simple, cut to the bone json parser class.
 */
class SHVJsonReader
{
public:
	virtual ~SHVJsonReader() {}

	virtual SHVBool Parse(SHVStreamIn& stream) = 0;
	virtual SHVBool ParseDirect(const char* buffer, int actualLen) = 0;
	virtual SHVBool ParserDone() = 0;

	virtual const SHVJsonKeyValuePair& operator[](size_t idx) = 0;
	virtual int GetCount() = 0;

	virtual void SetStartObjectCallback(SHVJsonReaderCallbackBase* callback) = 0;
	virtual void SetEndObjectCallback(SHVJsonReaderCallbackBase* callback) = 0;
	virtual void SetStartArrayCallback(SHVJsonReaderCallbackBase* callback) = 0;
	virtual void SetEndArrayCallback(SHVJsonReaderCallbackBase* callback) = 0;
};
typedef SHVPtrContainer<SHVJsonReader> SHVJsonReaderPtr;

//-=========================================================================================================
/// SHVXmlReaderCallbackBase class - Base class for the callback template class
/**
 * Return true if parser should continue. (Perfect for error handling)
 */
class SHVJsonReaderCallbackBase
{
public:
	virtual bool PerformCallback(SHVJsonReader& reader) = 0;
	virtual ~SHVJsonReaderCallbackBase() {}
};

//-=========================================================================================================
/// SHVJsonReaderCallback template class - callback template class
/**
 *
 */
template<class T>
class SHVJsonReaderCallback: public SHVJsonReaderCallbackBase
{
public:
	typedef bool (T::*Callback)(SHVJsonReader& reader);
	inline SHVJsonReaderCallback(T* target, Callback function);
	virtual bool PerformCallback(SHVJsonReader& reader);
private:
	Callback CallbackFunction;
	T* Target;
};

//-=========================================================================================================
/// SHVJsonKeyValuePair - Json key-value pair interface class
/**
 *
 */
class SHVJsonKeyValuePair
{
public:
	enum JsonType
	{
		Type_Null = 0,
		Type_Int = 2,
		Type_Float = 3,
		Type_Bool = 4,
		Type_String = 5
	};

	inline SHVStringBuffer GetName() const;

	virtual SHVStringBuffer8 GetName8() const = 0;
	virtual SHVStringBufferUTF8 GetNameUTF8() const = 0;
	virtual SHVStringBuffer16 GetName16() const = 0;

	virtual SHVInt AsInt() const = 0;
	virtual SHVInt64 AsInt64() const = 0;
	virtual SHVDouble AsDouble() const = 0;
	virtual SHVBool AsBool() const = 0;

	inline SHVStringBuffer AsString() const;
	virtual SHVStringBuffer8 AsString8() const = 0;
	virtual SHVStringBufferUTF8 AsStringUTF8() const = 0;
	virtual SHVStringBuffer16 AsString16() const = 0;

	virtual SHVTime AsDateTime() const = 0;
	virtual JsonType GetType() const = 0;
};

// ================================= implementation - SHVJsonReaderCallback ================================= //
/*************************************
 * Constructor
 *************************************/
template<class T>
SHVJsonReaderCallback<T>::SHVJsonReaderCallback(T* target, Callback function)
{
	Target = target;
	CallbackFunction = function;
}

/*************************************
 * PerformCallback
 *************************************/
template<class T>
bool SHVJsonReaderCallback<T>::PerformCallback(SHVJsonReader& reader)
{
	return (Target->*CallbackFunction)(reader);
}

// ================================= implementation - SHVJsonKeyValuePair ================================= //
/*************************************
 * GetName
 *************************************/
SHVStringBuffer SHVJsonKeyValuePair::GetName() const
{
#ifdef UNICODE
	return GetName16();
#else
	return GetName8();
#endif
}
/*************************************
 * AsString
 *************************************/
SHVStringBuffer SHVJsonKeyValuePair::AsString() const
{
#ifdef UNICODE
	return AsString16();
#else
	return AsString8();
#endif
}

#endif // __SHIVA_JSONREADER_H
