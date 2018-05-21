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

	virtual SHVBool ReadStringUTF8(SHVChar* buffer, size_t size) = 0;
	virtual SHVChar ReadCharUTF8() = 0;

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
#if __SHVSTRINGDEFAULT == 8
	return ReadString8(buffer, maxlen);
#elif __SHVSTRINGDEFAULT == 16
	return ReadString16(buffer, maxlen);
#elif __SHVSTRINGDEFAULT == utf8
	return ReadStringUTF8(buffer, maxlen);
#endif
}

/*************************************
 * ReadChar
 *************************************/
SHVTChar SHVStreamIn::ReadChar()
{
#if __SHVSTRINGDEFAULT == 8
	return ReadChar8();
#elif __SHVSTRINGDEFAULT == 16
	return ReadChar16();
#elif __SHVSTRINGDEFAULT == utf8
	return ReadCharUTF8();
#endif
}


// ============================================== documentation ============================================= //

/// \class SHVStreamIn shvstreamin.h "shiva/include/utils/shvstreamin.h"

#endif
