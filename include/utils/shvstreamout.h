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
#ifdef UNICODE
	return WriteString16(buffer, maxlen);
#else
	return WriteString8(buffer, maxlen);
#endif
}

/*************************************
 * WriteChar
 *************************************/
SHVBool SHVStreamOut::WriteChar(SHVTChar ch)
{
#ifdef UNICODE
	return WriteChar16(ch);
#else
	return WriteChar8(ch);
#endif
}

#endif
