#ifndef __SHIVA_WIN32GUI_MAINTHREADEVENTDISPATCHER_H
#define __SHIVA_WIN32GUI_MAINTHREADEVENTDISPATCHER_H


#include "../../../include/framework/shvmainthreadeventdispatcher.h"
#include "../../../include/gui/shvcontrolcontainer.h"
#include "shvguimanagerwin32.h"

#ifndef __SHIVA_WIN32
# error This code only works for win32
#elif defined(__SHIVA_MFC)
# error This code does not work in MFC mode
#endif


// forward declares
class SHVControlImplementerMainWindowWin32;

//-=========================================================================================================
/// SHVMainThreadEventDispatcherWin32 - Main thread event dispatcher for win32
/**
 * Stuff ...
 */

class SHVMainThreadEventDispatcherWin32 : public SHVMainThreadEventDispatcher
{
public:


	// constructor
	SHVMainThreadEventDispatcherWin32();
	virtual ~SHVMainThreadEventDispatcherWin32();


	virtual void SignalDispatcher();

	virtual SHVBool InitializeEventLoop();
	virtual void RunEventLoop();
	virtual void StopEventLoop(SHVBool errors);


protected:

	virtual void OnEvent(SHVEvent* event);


private:
	///\cond INTERNAL
	friend class SHVControlImplementerMainWindowWin32;
	void DispatchEvents();
	void CloseApp();

	SHVGUIManagerWin32* GUIManager;
	SHVControlContainerRef MainWindow;
	bool ClosePending;
	///\endcond
};

#endif
