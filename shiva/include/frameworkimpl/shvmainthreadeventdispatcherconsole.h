#ifndef __SHIVA_FRAMEWORK_SHVMAINTHREADEVENTDISPATCHERCONSOLE_H
#define __SHIVA_FRAMEWORK_SHVMAINTHREADEVENTDISPATCHERCONSOLE_H

#include "../framework/shvmainthreadeventdispatcher.h"
#include "../threadutils/shvmutexbase.h"
#include "../utils/shvbuffercstream.h"

#include "../framework/shveventstdin.h"
#ifdef __SHIVA_WIN32
# include "../threadutils/shvthread.h"
# ifdef __SHIVA_POCKETPC
#  include <aygshell.h>
# endif
#endif

//-=========================================================================================================
/// SHVMainThreadEventDispatcherConsole class - Dispatcher for console user IO
/**
 * This class implements a generic dispatcher that can be used for generic console
 * user IO.
 */

class SHVAPI SHVMainThreadEventDispatcherConsole : public SHVMainThreadEventDispatcher
{
public:


	// constructor
	SHVMainThreadEventDispatcherConsole();
	~SHVMainThreadEventDispatcherConsole();


	// from SHVMainThreadEventDispatcher
	virtual void SetupDefaults(SHVModuleList& modules);

	virtual void SignalDispatcher();

	virtual SHVBool InitializeEventLoop();
	virtual void RunEventLoop();
	virtual void StopEventLoop(SHVBool errors);


protected:

	// from SHVEventTarget
	virtual void OnEvent(SHVEvent* event);
	
	void HandleStdin();

private:
	///\cond INTERNAL
	
	enum { StdinBufSize = 256 };
	
#if defined(__SHIVA_WINCE)
friend class SHVConsole;

	static void Print(const SHVString16C str);
	void PrintInternal(const SHVString16C str);
	static int AlignDWord(int val) { return ((val + 3) & ~3); }
	SHVBool CreateDlg();
	void Resize();
	static INT_PTR CALLBACK WinceDlgProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK WinceEditProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

# ifdef __SHIVA_POCKETPC
	SHACTIVATEINFO s_sai;
# endif
	SHVModuleList* ModuleList;
	SHVMutex Lock;
	SHVList<SHVString16,SHVStringBuffer16> StringBuffers;
	int fontHeight, fontWidth;
	SIZE boundaries;
	HWND wndConsole;
	HWND edtConsole;
	HFONT Font;
	WNDPROC edtProc;
#elif defined(__SHIVA_WIN32)
	bool Initializing;
	bool QuirksMode;
	HANDLE StdinHandle;
	SHVModuleList* ModuleList;
	SHVMutexBase Signal;
	SHVEventSubscriberRef selfSubs;
	SHVThread<SHVMainThreadEventDispatcherConsole> StdinThread;
	void StdinFunc();
#else
	SHVMutex Mutex;
	bool QueueSignalled;
	int PipeSignal[2];
#endif
	
	SHVBufferCStream StdinStream;
	size_t StdinPos;
	///\endcond
};

#endif
