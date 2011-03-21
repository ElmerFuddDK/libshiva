#ifndef __SHIVA_WIN32GUI_MAINTHREADEVENTDISPATCHER_H
#define __SHIVA_WIN32GUI_MAINTHREADEVENTDISPATCHER_H


#include "../../../include/framework/shvmainthreadeventdispatcher.h"
#include "../../../include/gui/shvcontrolcontainer.h"
#include "shvguimanagergtk.h"

#ifndef __SHIVA_GTK
# error This code only works for gtk
#endif

#include <gtk/gtk.h>


// forward declares
class SHVControlImplementerMainWindowGtk;

//-=========================================================================================================
/// SHVMainThreadEventDispatcherGtk - Main thread event dispatcher for gtk
/**
 * Stuff ...
 */

class SHVMainThreadEventDispatcherGtk : public SHVMainThreadEventDispatcher
{
public:


	// constructor
	SHVMainThreadEventDispatcherGtk();
	virtual ~SHVMainThreadEventDispatcherGtk();


	virtual void SetupDefaults(SHVModuleList& modules);

	virtual void SignalDispatcher();

	virtual SHVBool InitializeEventLoop();
	virtual void RunEventLoop();
	virtual void StopEventLoop(SHVBool errors);
	
	
protected:

	virtual void OnEvent(SHVEvent* event);


private:
	///\cond INTERNAL
	friend class SHVControlImplementerMainWindowGtk;
	
	void DispatchEvents();
	void CloseApp();

	SHVGUIManagerGtk* GUIManager;
	SHVControlContainerRef MainWindow;
	
	bool ClosePending;

	
	static gboolean on_delete_event(GtkWidget *widget, GdkEvent  *event, gpointer data);
	static void on_destroy(GtkWidget * widget, gpointer data);
	static void on_eventsignal( gpointer data,
				 gint source,
				 GdkInputCondition condition );
	
	guint InputEventHandle;
	int PipeSignal[2];
	SHVMutexBase SignalLock;
	bool QueueSignalled;
	///\endcond
};

#endif
