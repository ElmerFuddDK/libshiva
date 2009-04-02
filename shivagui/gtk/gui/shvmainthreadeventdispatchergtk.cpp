/*
 *   Copyright (C) 2008 by Lars Eriksen
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of the
 *   License, or (at your option) any later version with the following
 *   exeptions:
 *
 *   1) Static linking to the library does not constitute derivative work
 *      and does not require the author to provide source code for the
 *      application.
 *      Compiling applications with the source code directly linked in is
 *      Considered static linking as well.
 *
 *   2) You do not have to provide a copy of the license with programs
 *      that are linked against this code.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "stdafx.h"
#include "../../../include/platformspc.h"
#include "../../../include/utils/shvdir.h"
#include "shvmainthreadeventdispatchergtk.h"
#include "shvgtk.h"
#include "shvcontrolimplementermainwindowgtk.h"

#include <unistd.h>
#include <fcntl.h>


//-=========================================================================================================
// SHVMainThreadEventDispatcherGtk - Main window implementation
//-=========================================================================================================


/*************************************
 * Constructor
 *************************************/
SHVMainThreadEventDispatcherGtk::SHVMainThreadEventDispatcherGtk()
{
	GUIManager = NULL;
	ClosePending = false;

	pipe(PipeSignal);

	// disable blocking mode on the pipes - close on failure
	if (fcntl(PipeSignal[0], F_SETFL, O_NONBLOCK) < 0)
	{
		close(PipeSignal[0]);
		PipeSignal[0] = 0;
	}
	if (fcntl(PipeSignal[1], F_SETFL, O_NONBLOCK) < 0)
	{
		close(PipeSignal[1]);
		PipeSignal[1] = 0;
	}
	
	InputEventHandle = 0;
	
	QueueSignalled = false;
}

/*************************************
 * Destructor
 *************************************/
SHVMainThreadEventDispatcherGtk::~SHVMainThreadEventDispatcherGtk()
{
	if (PipeSignal[0])
		close(PipeSignal[0]);
	if (PipeSignal[1])
		close(PipeSignal[1]);
	
	if (InputEventHandle)
		gtk_input_remove(InputEventHandle);
}

/*************************************
 * SignalDispatcher
 *************************************/
void SHVMainThreadEventDispatcherGtk::SetupDefaults(SHVModuleList& modules)
{
	///\todo Implement a proper way to get AppPath and AppName
	modules.GetConfig().Set(SHVModuleList::DefaultCfgAppPath,SHVStringC(_T(".")) + SHVDir::Delimiter());
	modules.GetConfig().Set(SHVModuleList::DefaultCfgAppName,SHVStringC(_T("")));
}

/*************************************
 * SignalDispatcher
 *************************************/
void SHVMainThreadEventDispatcherGtk::SignalDispatcher()
{
	SignalLock.Lock();
	if (!QueueSignalled)
	{
		QueueSignalled = true;
		write(PipeSignal[1],"1",1);
	}
	SignalLock.Unlock();
}

/*************************************
 * InitializeEventLoop
 *************************************/
SHVBool SHVMainThreadEventDispatcherGtk::InitializeEventLoop()
{
SHVBool retVal;

	gtk_init (0, NULL);

	GUIManager = new SHVGUIManagerGtk(Queue->GetModuleList());
	MainWindow = new SHVControlContainer(GUIManager,new SHVControlImplementerMainWindowGtk(this));
	
	GUIManager->SetMainWindow(MainWindow);
	
	retVal = Queue->GetModuleList().AddModule(GUIManager);
	
	if (retVal)
	{
		retVal = MainWindow->Create();
		GUIManager->SetStandardFonts(MainWindow->GetFont());
		MainWindow->SetFont(GUIManager->GetFont(SHVGUIManager::CfgFontNormal),false);
		
		if (retVal)
		{
		GtkWidget* mainWnd = Gtk::GetMainWndHandle(MainWindow);

			g_signal_connect (G_OBJECT (mainWnd), "delete-event",
							G_CALLBACK (SHVMainThreadEventDispatcherGtk::on_delete_event), this);
			g_signal_connect (G_OBJECT (mainWnd), "destroy",
							G_CALLBACK (SHVMainThreadEventDispatcherGtk::on_destroy), this);
			
			if (!PipeSignal[0] || !PipeSignal[1])
				retVal = SHVBool::False;
			else
			{
				InputEventHandle = gtk_input_add_full( PipeSignal[0], GDK_INPUT_READ, &SHVMainThreadEventDispatcherGtk::on_eventsignal, NULL, this, NULL );
			}
		}
	}
	
	return retVal;
}

/*************************************
 * RunEventLoop
 *************************************/
void SHVMainThreadEventDispatcherGtk::RunEventLoop()
{
	SHVMainThreadEventDispatcher::DispatchEvents();
	
	MainWindow->SetFlag(SHVControl::FlagVisible,true);
//	gtk_widget_show_all(Gtk::GetHandle(MainWindow));
	
	gtk_main();
	gtk_input_remove(InputEventHandle);
	InputEventHandle = 0;

	MainWindow->Clear();
	MainWindow = NULL;
}

/*************************************
 * StopEventLoop
 *************************************/
void SHVMainThreadEventDispatcherGtk::StopEventLoop(SHVBool errors)
{
	if (!errors) // errors during startup
	{
	SHVString errStr = Queue->GetModuleList().GetStartupErrors();
		fprintf(stderr, "\n\nRegistering failed:\n\"%s\"\n\n", errStr.GetSafeBuffer());
	}
	
	gtk_main_quit();
}

/*************************************
 * OnEvent
 *************************************/
void SHVMainThreadEventDispatcherGtk::OnEvent(SHVEvent* event)
{
	SHVUNUSED_PARAM(event);
}

/*************************************
 * OnEvent
 *************************************/
void SHVMainThreadEventDispatcherGtk::DispatchEvents()
{
	SHVMainThreadEventDispatcher::DispatchEvents();
}

/*************************************
 * OnEvent
 *************************************/
void SHVMainThreadEventDispatcherGtk::CloseApp()
{
	if (!ClosePending && !Queue->GetModuleList().IsUnregistering() && Queue->GetModuleList().IsRegistered())
	{
		ClosePending = true;
		Queue->GetModuleList().CloseApp();
	}
}


// =========================================== gtk event functions ========================================== //


///\cond INTERNAL
/*************************************
 * on_delete_event
 *************************************/
gboolean SHVMainThreadEventDispatcherGtk::on_delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
SHVMainThreadEventDispatcherGtk* self = (SHVMainThreadEventDispatcherGtk*)data;
	
	SHVUNUSED_PARAM(widget);
	SHVUNUSED_PARAM(event);

	if (self->Queue->GetModuleList().IsUnregistering() || self->ClosePending)
	{
		return TRUE;
	}
	else if (self->Queue->GetModuleList().IsRegistered())
	{
		self->ClosePending = true;
		self->Queue->GetModuleList().CloseApp();
		return TRUE;
	}
	return FALSE;
}

/*************************************
 * on_destroy
 *************************************/
void SHVMainThreadEventDispatcherGtk::on_destroy(GtkWidget * widget, gpointer data)
{
SHVMainThreadEventDispatcherGtk* self = (SHVMainThreadEventDispatcherGtk*)data;
	
	SHVUNUSED_PARAM(widget);

	self->Queue->GetModuleList().CloseApp();
}

/*************************************
 * on_eventsignal
 *************************************/
void SHVMainThreadEventDispatcherGtk::on_eventsignal( gpointer data,gint source,GdkInputCondition condition)
{
SHVMainThreadEventDispatcherGtk* self = (SHVMainThreadEventDispatcherGtk*)data;
char dummyBuffer[2];

	SHVUNUSED_PARAM(source);
	SHVUNUSED_PARAM(condition);

	self->SignalLock.Lock();
	self->QueueSignalled = false;
	read(self->PipeSignal[0], dummyBuffer, 50);
	self->SignalLock.Unlock();
	self->DispatchEvents();
}
///\endcond
