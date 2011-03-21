#ifndef __SHIVA_JSONWRITERIMPL_H
#define __SHIVA_JSONWRITERIMPL_H

#include "shiva/include/modules/jsonstream/shvjsonwriter.h"
#include "shiva/include/utils/shvlist.h"

class SHVJsonWriterImpl: public SHVJsonWriter
{
public:
	SHVJsonWriterImpl();
	virtual ~SHVJsonWriterImpl();

	virtual void WriteEndObject(SHVStreamOut& w);
	virtual void WriteEndArray(SHVStreamOut& w);

	virtual void WriteStartObjectUTF8(SHVStreamOut& w, const SHVStringUTF8C key);
	virtual void WriteStartArrayUTF8(SHVStreamOut& w, const SHVStringUTF8C key);
	virtual void WriteStringUTF8(SHVStreamOut& w,const SHVStringUTF8C val, const SHVStringUTF8C key);
	virtual void WriteIntUTF8(SHVStreamOut& w,SHVInt val, const SHVStringUTF8C key);
	virtual void WriteDoubleUTF8(SHVStreamOut& w,SHVDouble val, const SHVStringUTF8C key);
	virtual void WriteBoolUTF8(SHVStreamOut& w,SHVBool val, const SHVStringUTF8C key);
	virtual void WriteDateTimeUTF8(SHVStreamOut& w,const SHVTime& time, const SHVStringUTF8C key);
private:
	SHVList<bool> FirstElementStack;
};

#endif // __SHIVA_JSONWRITERIMPL_H
