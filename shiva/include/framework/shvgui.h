#ifndef __SHIVA_FRAMEWORK_SHVGUI_H
#define __SHIVA_FRAMEWORK_SHVGUI_H

#include "../utils/shvstringc.h"
#include "../utils/shvdll.h"

#ifdef __SHIVA_WINCE
# define GUIMAIN() int WINAPI WinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
# define GUIPARSEARGS(cfg) SHVModuleListImpl::ParseArgs(cfg,SHVStringC((const SHVTChar*)lpCmdLine))
#elif defined(__SHIVA_WIN32)
# define GUIMAIN() int APIENTRY _tWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPTSTR lpCmdLine, int nCmdShow)
# define GUIPARSEARGS(cfg) SHVModuleListImpl::ParseArgs(cfg,SHVStringC((const SHVTChar*)lpCmdLine))
#else
# define GUIMAIN() int main(int argc, char *argv[])
# define GUIPARSEARGS(cfg) SHVModuleListImpl::ParseArgs(cfg,argc,argv)
#endif

//-=========================================================================================================
/// SHVGUI class - GUI static helper class
/**
 * Implementation of helper GUI static functions
 */

class SHVAPI SHVGUI
{
public:

	// Returns the GUI lib names for each API
	static const SHVTChar* LibNameWin32();
	static const SHVTChar* LibNameGtk();

	// Attempts to load the GUI module library
	static SHVBool LoadLib(SHVDll& guilib, const SHVStringC defaultPath=NULL);
	static SHVBool LoadLibExtended(SHVDll& guilib, const SHVTChar* defaultPath, ...);
};

#endif
