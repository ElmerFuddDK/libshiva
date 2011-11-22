#include "stdafx.h"
#include "shiva/include/platformspc.h"
#include "shiva/include/framework/shvconsole.h"
#include "shiva/include/framework/shveventstdin.h"

#include "shvsubprocesstester.h"


//=========================================================================================================
// SHVSubProcessTester class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVSubProcessTester::SHVSubProcessTester(SHVModuleList& modules) : SHVModule(modules,"SubProcessTester")
{
}

/*************************************
 * Register
 *************************************/
SHVBool SHVSubProcessTester::Register()
{
	if (!SHVModuleResolver<SHVSubProcessServer>(Modules,SubProcessServer,"SubProcessServer"))
		return false;

	// Console input events
	Modules.EventSubscribe(__EVENT_GLOBAL_STDIN, new SHVEventSubscriber(this));

	return SHVModule::Register();
}

/*************************************
 * Unregister
 *************************************/
void SHVSubProcessTester::Unregister()
{
	SHVModule::Unregister();
	
	if (!SubProcess.IsNull())
	{
		SubProcess->Shutdown();
		SubProcess = NULL;
	}
	if (Thread.IsRunning())
	{
		ThreadSubProcess->Shutdown();
	}
	while (Thread.IsRunning())
		SHVThreadBase::Sleep(50);
	ThreadSubProcess = NULL;
}

/*************************************
 * OnEvent
 *************************************/
void SHVSubProcessTester::OnEvent(SHVEvent* event)
{
	if (SHVEventString::Equals(event,__EVENT_GLOBAL_STDIN))
	{
	SHVString8 str(SHVEventStdin::StdinFromEvent(event));
	
		if (str == SHVString8C("/quit"))
		{
			Modules.CloseApp();
		}
		else if (str == SHVString8C("/help"))
		{
			SHVConsole::Printf8("Commands available:\n"
								" /launch       Will launch a test application to interact with\n"
								" /thread       Will launch the test application in a thread\n"
								" /quit         Will quit ...\n"
								" /help         Displays this info\n"
								"\n");
		}
		else if (str == SHVString8C("/thread"))
		{
			if (!Thread.IsRunning())
			{
				ThreadSubProcess = SubProcessServer->New();
				Thread.Start(this,&SHVSubProcessTester::ThreadProcess);
			}
		}
		else if (str == SHVString8C("/launch"))
		{
		SHVFileList args;
			if (!SubProcess.IsNull())
			{
				SHVConsole::Printf8("Terminating old process\n");
				SubProcess->Shutdown();
			}
			SubProcess = SubProcessServer->New();
			
			args.AddTail("-c");
			args.AddTail("awk 'BEGIN {print \"starting ...\"; fflush()}; /exit/ {print \"Subprocess exiting\"; fflush(); exit}; /$/ { print \"Subprocess got \" $0; fflush() }'");
			if (!SubProcess->Start("sh",args, SHVSubProcess::StdIn))
			{
				SHVConsole::Printf8("Failed to start awk\n");
			}
		}
		else
		{
			if (str.Left(1) == SHVString8("/"))
			{
				SHVConsole::Printf8("Unknown command - try /help\n");
			}
			else
			{
			bool running = false;
				if (!SubProcess.IsNull())
				{
					SubProcess->WriteLine8(str);
					running = true;
				}
				if (!ThreadSubProcess.IsNull())
				{
					ThreadSubProcess->WriteLine8(str);
					running = true;
				}
				if (!running)
				{
					SHVConsole::Printf8("Subprocesses not running\n");
				}
			}
		}
	}
}

/*************************************
 * ThreadProcess
 *************************************/
void SHVSubProcessTester::ThreadProcess()
{
SHVFileList args;
	args.AddTail("BEGIN {print \"starting ...\"; fflush()}; /exit/ {print \"Subprocess exiting\"; fflush(); exit}; /$/ { print \"Subprocess got \" $0; fflush() }");
	if (!ThreadSubProcess->Start("awk",args))
	{
		SHVConsole::Printf8("Failed to start subprocess\n");
	}
	else
	{
	SHVString8 str;
		while (ThreadSubProcess->ReadLine8(str))
		{
			SHVConsole::Printf8("Thread got : %s\n", str.GetSafeBuffer());
		}
		SHVConsole::Printf8("ThreadProcess done\n");
		ThreadSubProcess->Shutdown();
	}
}
