#ifndef __SHIVA_UTILS_STREAMIN_H
#define __SHIVA_UTILS_STREAMIN_H

#include "../shvtypes.h"
#include "shvstring.h"
#include "shvptrcontainer.h"
//-=========================================================================================================
/// SHVStreamIn class - Stream input interface
/**
 */
class SHVAPI SHVStreamIn
{
public:
	virtual ~SHVStreamIn() {}
	virtual bool Eof() const = 0;
	virtual SHVBool IsOk() const =  0;
	virtual SHVBool ReadBuffer(void* buffer, size_t bufferSize, size_t& actualLength) = 0;

	virtual SHVBool ReadString16(SHVWChar* buffer, size_t maxlen) = 0;
	virtual SHVWChar ReadChar16() = 0;

	virtual SHVBool ReadString8(SHVChar* buffer, size_t maxlen) = 0;
	virtual SHVChar ReadChar8() = 0;
	virtual void Close() = 0;

	inline SHVBool ReadString(SHVTChar* buffer, size_t maxlen);
	inline SHVTChar ReadChar();
};
typedef SHVPtrContainer<SHVStreamIn> SHVStreamInPtr;

// ============================================= implementation ============================================= //
/*************************************
 * ReadString
 *************************************/
SHVBool SHVStreamIn::ReadString(SHVTChar* buffer, size_t maxlen)
{
#ifdef UNICODE
	return ReadString16(buffer, maxlen);
#else
	return ReadString8(buffer, maxlen);
#endif
}

/*************************************
 * ReadChar
 *************************************/
SHVTChar SHVStreamIn::ReadChar()
{
#ifdef UNICODE
	return ReadChar16();
#else
	return ReadChar8();
#endif
}

#endif
