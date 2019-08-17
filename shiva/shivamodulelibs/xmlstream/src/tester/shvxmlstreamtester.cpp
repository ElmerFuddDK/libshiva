#include "stdafx.h"
#include "shiva/include/platformspc.h"
#include "shiva/include/framework/shvconsole.h"
#include "shiva/include/framework/shveventstdin.h"

#include "shvxmlstreamtester.h"


//=========================================================================================================
// SHVXmlStreamTester class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVXmlStreamTester::SHVXmlStreamTester(SHVModuleList& modules) : SHVModule(modules,"XmlStreamTester")
{
}

/*************************************
 * Register
 *************************************/
SHVBool SHVXmlStreamTester::Register()
{
	if (!SHVModuleResolver<SHVXmlStream>(Modules,XmlStream,"XmlStream"))
		return false;

	// Console input events
	Modules.EventSubscribe(__EVENT_GLOBAL_STDIN, new SHVEventSubscriber(this));

	return SHVModule::Register();
}

/*************************************
 * PostRegister
 *************************************/
void SHVXmlStreamTester::PostRegister()
{
	SHVModule::PostRegister();
}

/*************************************
 * Unregister
 *************************************/
void SHVXmlStreamTester::Unregister()
{
	SHVModule::PreUnregister();
}

/*************************************
 * OnEvent
 *************************************/
void SHVXmlStreamTester::OnEvent(SHVEvent* event)
{
	if (SHVEventString::Equals(event,__EVENT_GLOBAL_STDIN))
	{
	SHVStringUTF8 str(SHVEventStdin::StdinFromEventUTF8(event));
	
		if (str == SHVStringUTF8C("/quit"))
		{
			Modules.CloseApp();
		}
		else if (str == SHVStringUTF8C("/help"))
		{
			SHVConsole::PrintfUTF8("Commands available:\n"
								" /quit         Will quit ...\n"
								" /help         Displays this info\n"
								"\n");
		}
		else
		{
			SHVConsole::PrintfUTF8("Unknown command - try /help\n");
		}
	}
}
