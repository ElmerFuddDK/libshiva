#ifndef __SHIVA_UTILS_STREAMOUT_H
#define __SHIVA_UTILS_STREAMOUT_H

#include "../shvtypes.h"
#include "../shvstring.h"
//-=========================================================================================================
/// SHVStreamOut class - Stream out interface
/**
 */
class SHVStreamOut
{
public:
	virtual ~SHVStreamOut() {}
	virtual SHVBool IsOk() const =  0;
	virtual SHVBool WriteBuffer(void* buffer, int len) = 0;

	virtual SHVBool WriteString16(SHVWChar* buffer, int maxlen = -1) = 0;
	virtual SHVBool WriteChar16(SHVWChar ch) = 0;

	virtual SHVBool WriteString8(SHVChar* buffer, int maxlen = -1) = 0;
	virtual SHVBool WriteChar8(SHVChar ch) = 0;

	inline SHVBool WriteString(SHVTChar* buffer, int maxlen = -1);
	inline SHVBool WriteChar(SHVTChar);
};

// ============================================= implementation ============================================= //
/*************************************
 * ReadString
 *************************************/
SHVBool SHVStreamOut::WriteString(SHVTChar* buffer, int maxlen)
{
#ifdef UNICODE
	return WriteString16(buffer, maxlen);
#else
	return WriteString8(buffer, maxlen);
#endif
}

/*************************************
 * ReadChar
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
