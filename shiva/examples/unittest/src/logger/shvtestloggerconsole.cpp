#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvtestloggerconsole.h"
#include "shiva/include/framework/shvconsole.h"

#ifdef __SHIVA_WINCE
# define COLOR_RED   ""
# define COLOR_GREEN ""
# define COLOR_NEUTRAL ""
#else
# define COLOR_RED   "\033[31m"
# define COLOR_GREEN "\033[32m"
# define COLOR_NEUTRAL "\033[0m"
#endif


//=========================================================================================================
// SHVTestLoggerConsole class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVTestLoggerConsole::SHVTestLoggerConsole() : SHVTestLogger()
{
}

/*************************************
 * AddTitle
 *************************************/
void SHVTestLoggerConsole::AddTitle(const SHVStringC str)
{
	SHVConsole::Printf(_S("\n\n%s:\n------------------------------------------\n"), str.GetSafeBuffer());
}

/*************************************
 * AddHeader
 *************************************/
void SHVTestLoggerConsole::AddHeader(const SHVStringC str)
{
	SHVConsole::Printf(_S("\n%s\n"), str.GetSafeBuffer());
}

/*************************************
 * AddHeader
 *************************************/
void SHVTestLoggerConsole::AddHeader(const SHVTChar* s, ...)
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
void SHVTestLoggerConsole::AddLine(const SHVStringC str)
{
	SHVConsole::Printf(_S("  %s\n"), str.GetSafeBuffer());
}

/*************************************
 * AddLine
 *************************************/
void SHVTestLoggerConsole::AddLine(const SHVTChar* s, ...)
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
const SHVStringC SHVTestLoggerConsole::True()
{
	return _S("true");
}

/*************************************
 * False
 *************************************/
const SHVStringC SHVTestLoggerConsole::False()
{
	return _S("false");
}

/*************************************
 * Success
 *************************************/
const SHVStringC SHVTestLoggerConsole::Success(bool success)
{
	return (success ? _SD(COLOR_GREEN) _SC("Succeeded") _SCD(COLOR_NEUTRAL) : _SD(COLOR_RED) _SC("FAILED") _SCD(COLOR_NEUTRAL));
}
