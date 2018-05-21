#ifndef __SHIVA_UTILS_STREAMOUT_H
#define __SHIVA_UTILS_STREAMOUT_H

#include "../shvtypes.h"
#include "shvstring.h"
#include "shvptrcontainer.h"
//-=========================================================================================================
/// SHVStreamOut class - Stream out interface
/**
 */
class SHVAPI SHVStreamOut
{
public:
	virtual ~SHVStreamOut() {}
	virtual SHVBool IsOk() const =  0;
	virtual SHVBool WriteBuffer(const void* buffer, size_t len) = 0;

	virtual SHVBool WriteString16(const SHVWChar* buffer, size_t maxlen = SIZE_T_MAX) = 0;
	virtual SHVBool WriteChar16(SHVWChar ch) = 0;

	virtual SHVBool WriteString8(const SHVChar* buffer, size_t maxlen = SIZE_T_MAX) = 0;
	virtual SHVBool WriteChar8(const SHVChar ch) = 0;

	virtual SHVBool WriteStringUTF8(const SHVChar* buffer, size_t maxsize = SIZE_T_MAX) = 0;
	virtual SHVBool WriteCharUTF8(const SHVChar ch) = 0;

	virtual void Close() = 0;

	inline SHVBool WriteString(const SHVTChar* buffer, size_t maxlen = SIZE_T_MAX);
	inline SHVBool WriteChar(SHVTChar);
};
typedef SHVPtrContainer<SHVStreamOut> SHVStreamOutPtr;

// ============================================= implementation ============================================= //
/*************************************
 * WriteString
 *************************************/
SHVBool SHVStreamOut::WriteString(const SHVTChar* buffer, size_t maxlen)
{
#if __SHVSTRINGDEFAULT == 8
	return WriteString8(buffer, maxlen);
#elif __SHVSTRINGDEFAULT == 16
	return WriteString16(buffer, maxlen);
#elif __SHVSTRINGDEFAULT == utf8
	return WriteStringUTF8(buffer, maxlen);
#endif
}

/*************************************
 * WriteChar
 *************************************/
SHVBool SHVStreamOut::WriteChar(SHVTChar ch)
{
#if __SHVSTRINGDEFAULT == 8
	return WriteChar8(ch);
#elif __SHVSTRINGDEFAULT == 16
	return WriteChar16(ch);
#elif __SHVSTRINGDEFAULT == utf8
	return WriteCharUTF8(ch);
#endif
}


// ============================================== documentation ============================================= //

/// \class SHVStreamOut shvstreamout.h "shiva/include/utils/shvstreamout.h"

#endif
