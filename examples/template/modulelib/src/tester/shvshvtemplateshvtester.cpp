#include "stdafx.h"
#include "shiva/include/platformspc.h"
#include "shiva/include/framework/shvconsole.h"
#include "shiva/include/framework/shveventstdin.h"

#include "shvshvtemplateshvtester.h"


//=========================================================================================================
// SHVSHVTemplateSHVTester class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVSHVTemplateSHVTester::SHVSHVTemplateSHVTester(SHVModuleList& modules) : SHVModule(modules,"SHVTemplateSHVTester")
{
}

/*************************************
 * Register
 *************************************/
SHVBool SHVSHVTemplateSHVTester::Register()
{
	if (!SHVModuleResolver<SHVSHVTemplateSHV>(Modules,SHVTemplateSHV,"SHVTemplateSHV"))
		return false;

	// Console input events
	Modules.EventSubscribe(__EVENT_GLOBAL_STDIN, new SHVEventSubscriber(this));

	return SHVModule::Register();
}

/*************************************
 * OnEvent
 *************************************/
void SHVSHVTemplateSHVTester::OnEvent(SHVEvent* event)
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
								" /quit         Will quit ...\n"
								" /help         Displays this info\n"
								"\n");
		}
		else
		{
			SHVConsole::Printf8("Unknown command - try /help\n");
		}
	}
}

