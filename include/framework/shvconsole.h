#ifndef __SHIVA_FRAMEWORK_SHVCONSOLE_H
#define __SHIVA_FRAMEWORK_SHVCONSOLE_H

#include "../utils/shvstringc.h"

// use this define on windows CE in order to enable the fake console window
#ifdef __SHIVA_WINCE
# define CONSOLEMAIN() int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
# define CONSOLEPARSEARGS(cfg) SHVModuleListImpl::ParseArgs(cfg,SHVStringC((const SHVTChar*)lpCmdLine))
#else
# define CONSOLEMAIN() int main(int argc, char *argv[])
# define CONSOLEPARSEARGS(cfg) SHVModuleListImpl::ParseArgs(cfg,argc,argv)
#endif


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
	
#ifdef __SHIVA_WINCE
private:
	///\cond INTERNAL
	static void vwprintf(const SHVWChar* str, SHVVA_LIST args);
	static void vprintf(const SHVChar* str, SHVVA_LIST args);
	static void vfwprintf(FILE* f, const SHVWChar* str, SHVVA_LIST args);
	static void vfprintf(FILE* f, const SHVChar* str, SHVVA_LIST args);
	///\endcond
#endif
};

#endif
