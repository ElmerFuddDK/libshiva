#ifndef __SHIVA_TESTLOGGER_H
#define __SHIVA_TESTLOGGER_H

#include "shiva/include/utils/shvstring.h"
#include "shiva/include/utils/shvrefobject.h"


class SHVTestLogger : public SHVRefObject
{
public:
	virtual void AddTitle(const SHVStringC str) = 0;
	virtual void AddHeader(const SHVStringC str) = 0;
	virtual void AddHeader(const SHVTChar* s, ...) = 0;
	virtual void AddLine(const SHVStringC str) = 0;
	virtual void AddLine(const SHVTChar* s, ...) = 0;

	virtual const SHVStringC True() = 0;
	virtual const SHVStringC False() = 0;

	virtual const SHVStringC Success(bool success) = 0;
protected:
	inline SHVTestLogger() {}
};
typedef SHVRefObjectContainer<SHVTestLogger> SHVTestLoggerRef;

#endif
