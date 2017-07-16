#include "stdafx.h"
#include "shiva/include/platformspc.h"
#include "shiva/include/framework/shvconsole.h"
#include "shiva/include/framework/shveventstdin.h"

#include "shvjsonstreamtester.h"


//=========================================================================================================
// SHVJsonStreamTester class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVJsonStreamTester::SHVJsonStreamTester(SHVModuleList& modules) : SHVModule(modules,"SubProcessTester")
{
}

/*************************************
 * Register
 *************************************/
SHVBool SHVJsonStreamTester::Register()
{
	// Console input events
	Modules.EventSubscribe(__EVENT_GLOBAL_STDIN, new SHVEventSubscriber(this));

	return SHVModule::Register();
}

/*************************************
 * Unregister
 *************************************/
void SHVJsonStreamTester::Unregister()
{
	SHVModule::Unregister();	
}

/*************************************
 * OnEvent
 *************************************/
void SHVJsonStreamTester::OnEvent(SHVEvent* event)
{
	if (SHVEventString::Equals(event,__EVENT_GLOBAL_STDIN))
	{
	SHVString8 str(SHVEventStdin::StdinFromEvent8(event));
	
	}
}

