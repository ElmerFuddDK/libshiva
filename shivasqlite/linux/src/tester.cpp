#include "Shiva/include/platformspc.h"
#include "Shiva/include/shvversion.h"
#include "Shiva/include/utils/shvdll.h"

#include "Shiva/include/frameworkimpl/shvmainthreadeventdispatcherconsole.h"
#include "Shiva/include/framework/shvmodulefactory.h"


class SHVTest : public SHVModule
{
public:

	SHVTest(SHVModuleList& modules) : SHVModule(modules,"Test")
	{
	}

	SHVBool Register()
	{
		printf("In register\n");
	
		Modules.EventSubscribe(SHVModuleList::EventEndRegister, new SHVEventSubscriber(this));
		Modules.EventSubscribe(SHVModuleList::EventClosing, new SHVEventSubscriber(this));
		Modules.EventSubscribe(__EVENT_GLOBAL_STDIN, new SHVEventSubscriber(this));
	
		return SHVModule::Register();
	}

	void PostRegister()
	{
	}

	void OnEvent(SHVEvent* event)
	{
		if (SHVEvent::Equals(event,SHVModuleList::EventEndRegister))
		{
			// Do something here
		}
		else if (SHVEventString::Equals(event,__EVENT_GLOBAL_STDIN))
		{
			SHVString8 str(SHVEventStdin::StdinFromEvent(event));
	
			if (str == SHVString8C("/quit"))
			{
				Modules.CloseApp();
			}
			else
			{
				printf("Unknown command\n");
			}
		}
	}

	void Unregister()
	{
		printf("In unregister\n");
		SHVModule::Unregister();
	}
};


int main(int argc, char *argv[])
{
SHVDll sedtemplatesedlib;
	
	SHVUNUSED_PARAM(argc);
	SHVUNUSED_PARAM(argv);

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE))
	{
		fprintf(stderr,"WRONG SHIVA VERSION\n");
	}
	else if (!sedtemplatesedlib.Load(sedtemplatesedlib.CreateLibFileName(_T("SEDTemplateSED"))))
	{
		fprintf(stderr,"Could not load SEDTemplateSED\n");
	}
	else
	{
	SHVMainThreadEventQueue mainqueue(new SHVMainThreadEventDispatcherConsole());
	SHVModuleFactory* factory = (SHVModuleFactory*)sedtemplatesedlib.CreateObjectInt(&mainqueue.GetModuleList(),SHVDll::ClassTypeModuleFactory);

		mainqueue.GetModuleList().AddModule(new SHVTest(mainqueue.GetModuleList()));
		factory->ResolveModules(__MODULESYMBOL_DEFAULTS);

		return mainqueue.Run().GetError();
	}
	
	return -1;
}
