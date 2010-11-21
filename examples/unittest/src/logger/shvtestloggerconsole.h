#ifndef __SHIVA_TESTLOGGERCONSOLE_H
#define __SHIVA_TESTLOGGERCONSOLE_H

#include "shvtestlogger.h"


//-=========================================================================================================
/// SHVTestLoggerConsole class
/**
 * Adds log to the console.
 */

class SHVTestLoggerConsole : public SHVTestLogger
{
public:

	SHVTestLoggerConsole();

	virtual void AddTitle(const SHVStringC str);
	virtual void AddHeader(const SHVStringC str);
	virtual void AddHeader(const SHVTChar* s, ...);
	virtual void AddLine(const SHVStringC str);
	virtual void AddLine(const SHVTChar* s, ...);

	virtual const SHVStringC True();
	virtual const SHVStringC False();

	virtual const SHVStringC Success(bool success);
};

#endif
