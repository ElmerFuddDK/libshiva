#ifndef __SHIVA_TESTLOGGERCONSOLE_H
#define __SHIVA_TESTLOGGERCONSOLE_H

#include "shiva/include/unittest/shvtestlogger.h"
#include "shiva/include/gui/shvcontroledit.h"


//-=========================================================================================================
/// SHVTestLoggerGUI class
/**
 * Adds log to a GUI edit box.
 */

class SHVTestLoggerGUI : public SHVTestLogger
{
public:

	SHVTestLoggerGUI(SHVControlEdit* logWindow);

	virtual void AddTitle(const SHVStringC str);
	virtual void AddHeader(const SHVStringC str);
	virtual void AddHeader(const SHVTChar* s, ...);
	virtual void AddLine(const SHVStringC str);
	virtual void AddLine(const SHVTChar* s, ...);

	virtual const SHVStringC True();
	virtual const SHVStringC False();

	virtual const SHVStringC Success(bool success);

private:
	
	void AddLog(const SHVStringC str);
	
	SHVControlEditRef LogWindow;

};

#endif
