
#include "../../../include/platformspc.h"
#include "../../../include/framework/shvmodule.h"
#include "../../../include/frameworkimpl/shvmainthreadeventdispatchergeneric.h"
#include "../../../include/shvversion.h"


class SHVTest : public SHVModule
{
public:

	SHVTest(SHVModuleList& modules) : SHVModule(modules,"Test")
	{
	}
	
	SHVBool Register()
	{
		printf("In register\n");
		
		SHVASSERT(Modules.ResolveModule("Test"));
		SHVASSERT(Modules.ResolveModule("Timer"));
		
		Modules.EventSubscribe(SHVModuleList::EventClosing, new SHVEventSubscriber(this));
		
		return SHVModule::Register();
	}
	
	void PostRegister()
	{
		printf("Application running...\n");
		printf("Shutting it down\n");
		Modules.CloseApp();
	}
	
	void OnEvent(SHVEvent* event)
	{
		SHVUNUSED_PARAM(event);
		printf("Delaying shutdown by 1000 ms\n");
		Modules.EmitEvent(new SHVEventString(this,__EVENT_GLOBAL_DELAYSHUTDOWN,1000));
	}

	void Unregister()
	{
		printf("In unregister\n");
		SHVModule::Unregister();
	}
};


int main(int argc, char *argv[])
{
	SHVUNUSED_PARAM(argc);
	SHVUNUSED_PARAM(argv);

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE))
	{
		fprintf(stderr,"WRONG SHIVA VERSION\n");
	}
	else
	{
	SHVMainThreadEventQueue mainqueue(new SHVMainThreadEventDispatcherGeneric());
	SHVString testStr;

		mainqueue.GetModuleList().AddModule(new SHVTest(mainqueue.GetModuleList()));

		printf("Testing assertions - should fail in debug mode\n");
		SHVASSERT(false);
	
		testStr.Format(_T("This is a test %s %d.%d.%d\n"), _T("of SHIVA version"), __SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE);
	
		printf("%s", testStr.GetSafeBuffer());
		
		return mainqueue.Run().GetError();
	}
	
	return -1;
}
