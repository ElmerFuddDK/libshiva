#ifndef __SHIVA_FRAMEWORK_SHVCONSOLE_H
#define __SHIVA_FRAMEWORK_SHVCONSOLE_H

#include "../utils/shvstringc.h"

#ifdef __SHIVA_WINCE
# define CONSOLEMAIN() int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
# ifdef UNICODE
#  define CONSOLEPARSEARGS(cfg) SHVModuleListImpl::ParseArgs(cfg,SHVString16C::FromWin32(lpCmdLine).ToStrT())
# else
#  define CONSOLEPARSEARGS(cfg) SHVModuleListImpl::ParseArgs(cfg,SHVString8C(lpCmdLine).ToStrT())
# endif
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
	
	static void PrintfUTF8(const SHVChar* str, ...);
	static void PrintfListUTF8(const SHVChar* str, SHVVA_LIST args);
	static void ErrPrintfUTF8(const SHVChar* str, ...);
	static void ErrPrintfListUTF8(const SHVChar* str, SHVVA_LIST args);
	
	static void Printf16(const SHVWChar* str, ...);
	static void PrintfList16(const SHVWChar* str, SHVVA_LIST args);
	static void ErrPrintf16(const SHVWChar* str, ...);
	static void ErrPrintfList16(const SHVWChar* str, SHVVA_LIST args);
	
	
	// Detect if native console encoding is utf8
	static bool NativeEncodingIsUTF8();
	
	
#ifdef __SHIVA_WIN32
private:
	///\cond INTERNAL
	static void vwprintf(const WCHAR* str, SHVVA_LIST args);
	static void vfwprintf(FILE* f, const WCHAR* str, SHVVA_LIST args);
	static void wprintf(const WCHAR* str, ...);
	static void fwprintf(FILE* f, const WCHAR* str, ...);
	///\endcond
#endif
};

#endif
