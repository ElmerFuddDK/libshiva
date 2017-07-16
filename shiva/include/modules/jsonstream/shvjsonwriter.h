#ifndef __SHIVA_JSONWRITER_H
#define __SHIVA_JSONWRITER_H

#include "../../../include/utils/shvstreamout.h"
#include "../../../include/utils/shvstring.h"
#include "../../../include/utils/shvstring16.h"
#include "../../../include/utils/shvtime.h"

class SHVJsonWriter
{
public:
	virtual ~SHVJsonWriter() {}
	inline void WriteStartObject(SHVStreamOut& w, const SHVStringC key = NULL);
	virtual void WriteEndObject(SHVStreamOut& w) = 0;
	inline void WriteStartArray(SHVStreamOut& w, const SHVStringC key = NULL);
	virtual void WriteEndArray(SHVStreamOut& w) = 0;

	inline void WriteString(SHVStreamOut& w,const SHVStringC val, const SHVStringC key = NULL);
	inline void WriteInt(SHVStreamOut& w,SHVInt val, const SHVStringC key = NULL);
	inline void WriteDouble(SHVStreamOut& w,SHVDouble val, const SHVStringC key = NULL);
	inline void WriteBool(SHVStreamOut& w,SHVBool val, const SHVStringC key = NULL);
	inline void WriteDateTime(SHVStreamOut& w,const SHVTime& time, const SHVStringC key = NULL);


	virtual void WriteStartObjectUTF8(SHVStreamOut& w, const SHVStringUTF8C key = NULL) =  0;
	virtual void WriteStartArrayUTF8(SHVStreamOut& w, const SHVStringUTF8C key = NULL) = 0;
	virtual void WriteStringUTF8(SHVStreamOut& w,const SHVStringUTF8C val, const SHVStringUTF8C key = NULL) = 0;
	virtual void WriteIntUTF8(SHVStreamOut& w,SHVInt val, const SHVStringUTF8C key = NULL) = 0;
	virtual void WriteDoubleUTF8(SHVStreamOut& w,SHVDouble val, const SHVStringUTF8C key = NULL) = 0;
	virtual void WriteBoolUTF8(SHVStreamOut& w,SHVBool val, const SHVStringUTF8C key = NULL) = 0;
	virtual void WriteDateTimeUTF8(SHVStreamOut& w,const SHVTime& time, const SHVStringUTF8C key = NULL) = 0;
};
typedef SHVPtrContainer<SHVJsonWriter> SHVJsonWriterPtr;

// ================================== implementation - SHVJsonKeyWriter =================================== //
/*************************************
 * WriteStartObject
 *************************************/
void SHVJsonWriter::WriteStartObject(SHVStreamOut &w, const SHVStringC key)
{
	WriteStartObjectUTF8(w, key.ToStrUTF8());
}

/*************************************
 * WriteStartArray
 *************************************/
void SHVJsonWriter::WriteStartArray(SHVStreamOut &w, const SHVStringC key)
{
	WriteStartArrayUTF8(w, key.ToStrUTF8());
}

/*************************************
 * WriteString
 *************************************/
void SHVJsonWriter::WriteString(SHVStreamOut &w, const SHVStringC val, const SHVStringC key)
{
	WriteStringUTF8(w, val.ToStrUTF8(), key.ToStrUTF8());
}

/*************************************
 * WriteInt
 *************************************/
void SHVJsonWriter::WriteInt(SHVStreamOut &w, SHVInt val, const SHVStringC key)
{
	WriteIntUTF8(w, val, key.ToStrUTF8());
}
/*************************************
 * WriteDouble
 *************************************/
void SHVJsonWriter::WriteDouble(SHVStreamOut &w, SHVDouble val, const SHVStringC key)
{
	WriteDoubleUTF8(w, val, key.ToStrUTF8());
}
/*************************************
 * WriteBool
 *************************************/
void SHVJsonWriter::WriteBool(SHVStreamOut &w, SHVBool val, const SHVStringC key)
{
	WriteBoolUTF8(w, val, key.ToStrUTF8());
}
/*************************************
 * WriteString
 *************************************/
void SHVJsonWriter::WriteDateTime(SHVStreamOut &w, const SHVTime &time, const SHVStringC key)
{
	WriteDateTimeUTF8(w, time, key.ToStrUTF8());
}

#endif // __SHIVA_JSONWRITER_H
