#ifndef __SHIVA_JSONREADERIMPL_H
#define __SHIVA_JSONREADERIMPL_H

#include "shiva/include/utils/shvvector.h"
#include "shiva/include/utils/shvlist.h"
#include "shiva/include/modules/jsonstream/shvjsonreader.h"
#include "../jsonparser/JSON_parser.h"


//-=========================================================================================================
/// SHVJsonReaderImpl class - This class is the implementation for the class that parses json
/**
 * This is a very simple, cut to the bone json parser class.
 */

struct SHVJsonStack
{
public:
	inline SHVJsonStack();
	SHVVector<SHVJsonKeyValuePair> KeyValuePairs;
};

class SHVJsonReaderImpl: public SHVJsonReader
{
public:
	SHVJsonReaderImpl(const SHVStringUTF8C rootName = NULL);

	virtual ~SHVJsonReaderImpl();
	virtual SHVBool Parse(SHVStreamIn& stream);
	virtual SHVBool ParseDirect(const char* buffer, int actualLen);
	virtual SHVBool ParserDone();

	virtual const SHVJsonKeyValuePair& operator[](size_t idx);
	virtual int GetCount();

	virtual void SetStartObjectCallback(SHVJsonReaderCallbackBase* callback);
	virtual void SetEndObjectCallback(SHVJsonReaderCallbackBase* callback);
	virtual void SetStartArrayCallback(SHVJsonReaderCallbackBase* callback);
	virtual void SetEndArrayCallback(SHVJsonReaderCallbackBase* callback);
private:
	static int Callback(void* ctx, int type, const struct JSON_value_struct* value);

	SHVJsonReaderCallbackBase* StartObjectCallback;
	SHVJsonReaderCallbackBase* EndObjectCallback;
	SHVJsonReaderCallbackBase* StartArrayCallback;
	SHVJsonReaderCallbackBase* EndArrayCallback;

	SHVList<SHVJsonStack> Stack;
	JSON_parser Parser;
	JSON_config Config;
	SHVStringUTF8 RootName;
	SHVStringUTF8 CurrentKey;
};

//-=========================================================================================================
/// SHVJsonKeyValuePairImpl - Json key-value pair implementation class
/**
 *
 */
class SHVJsonKeyValuePairImpl: public SHVJsonKeyValuePair
{
public:
	SHVJsonKeyValuePairImpl(const SHVStringUTF8C name, JSON_type type, const JSON_value_struct* val);
	virtual ~SHVJsonKeyValuePairImpl();

	virtual SHVStringBuffer8 GetName8() const;
	virtual SHVStringBufferUTF8 GetNameUTF8() const;
	virtual SHVStringBuffer16 GetName16() const;

	virtual SHVInt AsInt() const;
	virtual SHVInt64 AsInt64() const;
	virtual SHVDouble AsDouble() const;
	virtual SHVBool AsBool() const;
	virtual SHVStringBuffer8 AsString8() const;
	virtual SHVStringBufferUTF8 AsStringUTF8() const;
	virtual SHVStringBuffer16 AsString16() const;
	virtual SHVTime AsDateTime() const;
	virtual JsonType GetType() const;
private:
	JsonType Type;
	union {
		int intVal;
		SHVInt64Val intVal64;
		double doubleVal;
		bool boolVal;
		SHVStringUTF8* stringVal;
	} Data;
	SHVStringUTF8 Name;
};

// ===================================== implementation - SHVJsonStack ====================================== //

SHVJsonStack::SHVJsonStack()
{
}

#endif // __SHIVA_JSONREADERIMPL_H
