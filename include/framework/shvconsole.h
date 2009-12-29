#ifndef __SHIVA_FRAMEWORK_SHVCONSOLE_H
#define __SHIVA_FRAMEWORK_SHVCONSOLE_H

#include "../utils/shvstringc.h"

//-=========================================================================================================
/// SHVConsole class - console output static class
/**
 * Implementation of code for outputting text to the console
 */

class SHVAPI SHVConsole
{
public:
	
	static void Printf(const SHVTChar* str, ...);
	static void PrintfList(const SHVTChar* str, SHVVA_LIST args);
	static void ErrPrintf(const SHVTChar* str, ...);
	static void ErrPrintfList(const SHVTChar* str, SHVVA_LIST args);
	
	static void Printf8(const SHVChar* str, ...);
	static void PrintfList8(const SHVChar* str, SHVVA_LIST args);
	static void ErrPrintf8(const SHVChar* str, ...);
	static void ErrPrintfList8(const SHVChar* str, SHVVA_LIST args);
	
	static void Printf16(const SHVWChar* str, ...);
	static void PrintfList16(const SHVWChar* str, SHVVA_LIST args);
	static void ErrPrintf16(const SHVWChar* str, ...);
	static void ErrPrintfList16(const SHVWChar* str, SHVVA_LIST args);
	
};

#endif
