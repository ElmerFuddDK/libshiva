#ifndef __SHIVA_LOGGER_H
#define __SHIVA_LOGGER_H

#include "utils/shvstring.h"
#include "utils/shvrefobject.h"

class SHVLogger : public SHVRefObject
{
public:
	virtual void AddTitle(const SHVStringC& str) = 0;
	virtual void AddHeader(const SHVStringC& str) = 0;
	virtual void AddHeader(const SHVTChar* s, ...) = 0;
	virtual void AddLine(const SHVStringC& str) = 0;
	virtual void AddLine(const SHVTChar* s, ...) = 0;

	virtual const SHVStringC& True() = 0;
	virtual const SHVStringC& False() = 0;

	virtual const SHVTChar* Success(bool success) = 0;
};

#endif
