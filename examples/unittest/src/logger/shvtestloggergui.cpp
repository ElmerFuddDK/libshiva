#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvtestloggergui.h"


//=========================================================================================================
// SHVTestLoggerGUI class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVTestLoggerGUI::SHVTestLoggerGUI(SHVControlEdit* logWindow) : SHVTestLogger(), LogWindow(logWindow)
{
}

/*************************************
 * AddTitle
 *************************************/
void SHVTestLoggerGUI::AddTitle(const SHVStringC str)
{
	AddLog(SHVStringC::Format(_S("\n\n%s:\n------------------------------------------\n"), str.GetSafeBuffer()));
}

/*************************************
 * AddHeader
 *************************************/
void SHVTestLoggerGUI::AddHeader(const SHVStringC str)
{
	AddLog(SHVStringC::Format(_S("\n%s\n"), str.GetSafeBuffer()));
}

/*************************************
 * AddHeader
 *************************************/
void SHVTestLoggerGUI::AddHeader(const SHVTChar* s, ...)
{
SHVString str;
	SHVVA_LIST args;
	SHVVA_START(args, s);
	str.FormatList(s,args);
	AddHeader(str);
	SHVVA_END(args);
}

/*************************************
 * AddLine
 *************************************/
void SHVTestLoggerGUI::AddLine(const SHVStringC str)
{
	AddLog(SHVStringC::Format(_S("  %s\n"), str.GetSafeBuffer()));
}

/*************************************
 * AddLine
 *************************************/
void SHVTestLoggerGUI::AddLine(const SHVTChar* s, ...)
{
SHVString str;
	SHVVA_LIST args;
	SHVVA_START(args, s);
	str.FormatList(s,args);
	AddLine(str);
	SHVVA_END(args);
}

/*************************************
 * True
 *************************************/
const SHVStringC SHVTestLoggerGUI::True()
{
	return _S("true");
}

/*************************************
 * False
 *************************************/
const SHVStringC SHVTestLoggerGUI::False()
{
	return _S("false");
}

/*************************************
 * Success
 *************************************/
const SHVStringC SHVTestLoggerGUI::Success(bool success)
{
	return (success ? _S("Succeeded") : _S("FAILED"));
}

/*************************************
 * AddLog
 *************************************/
void SHVTestLoggerGUI::AddLog(const SHVStringC str)
{
SHVString newLog(LogWindow->GetText());
int limit = LogWindow->GetLimit();
size_t newLogLen;

	newLog += str;
	newLogLen = newLog.GetLength();
	
	if (limit > 0 && newLogLen > (size_t)limit)
	{
		newLogLen = (size_t)limit;
		LogWindow->SetText(newLog.GetBufferConst()+(newLogLen-(size_t)limit));
	}
	else
	{
		LogWindow->SetText(newLog);
	}
	
	if (newLogLen > 0)
		LogWindow->SetSelection((int)newLogLen,SHVInt(),SHVControlEdit::ScrollBottom);
}
