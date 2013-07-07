#include "stdafx.h"
#include "shiva/include/platformspc.h"
#include "shiva/include/framework/shvconsole.h"
#include "shiva/include/framework/shveventstdin.h"

#include "shvluaenginetester.h"

typedef SHVRefObjectTemplate<int> SHVRefInt;
SHVLUAREFTYPE(SHVLuaRefInt,SHVRefInt);


//=========================================================================================================
// SHVLuaEngineTester class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVLuaEngineTester::SHVLuaEngineTester(SHVModuleList& modules) : SHVModule(modules,"LuaEngineTester")
{
}

/*************************************
 * Register
 *************************************/
SHVBool SHVLuaEngineTester::Register()
{
	if (!SHVModuleResolver<SHVLuaEngine>(Modules,LuaEngine,"LuaEngine"))
		return false;

	LuaScript = LuaEngine->CreateScript();
	LuaScript->RegisterFunc("TestFunc",new SHVLuaFunc<SHVLuaEngineTester>(this,&SHVLuaEngineTester::LuaTestFunc));
	LuaScript->SetHandlingErrors(true,new SHVEventSubscriber(this));
	
	// Console input events
	Modules.EventSubscribe(__EVENT_GLOBAL_STDIN, new SHVEventSubscriber(this));

	return SHVModule::Register();
}

/*************************************
 * PostRegister
 *************************************/
void SHVLuaEngineTester::PostRegister()
{
	SHVModule::PostRegister();
	
	if (Modules.GetConfig().Contains(_S("file")))
	{
	SHVLuaValuesRef retVal;
		LuaScript->ExecuteFromFile(Modules.GetConfig().Find(_S("file"))->ToString());
		retVal = LuaScript->ExecuteFunction("TestFunc",LuaScript->NewArgs()->AddDouble(1.234)->AddString(_S("This is a test"))->AddRef(SHVLuaRefInt().Obj(new SHVRefInt(512))));
		for (int i=0; i<retVal->GetCount(); i++)
		{
			SHVConsole::Printf(_S("Type(%d): %s\n"), retVal->Get(i)->GetType(), retVal->Get(i)->AsString().GetSafeBuffer());
			if (SHVLuaRefInt().Is(retVal,i))
			{
				SHVConsole::Printf8("  -> %s %d\n", SHVLuaRefInt().Type(), SHVLuaRefInt().Get(retVal,i)->Object());
			}
		}
	}
	if (Modules.GetConfig().Contains(_S("quit")))
	{
		Modules.CloseApp();
	}
}

/*************************************
 * Unregister
 *************************************/
void SHVLuaEngineTester::Unregister()
{
	SHVModule::PreUnregister();
	LuaScript->StopScript();
	LuaScript = NULL;
}

/*************************************
 * OnEvent
 *************************************/
void SHVLuaEngineTester::OnEvent(SHVEvent* event)
{
	if (SHVEventString::Equals(event,__EVENT_GLOBAL_STDIN))
	{
	SHVString8 str(SHVEventStdin::StdinFromEvent(event));
	
		if (str.Left(5) == SHVString8C("/run "))
		{
			LuaScript->Execute(str.GetSafeBuffer()+5);
		}
		else if (str == SHVString8C("/quit"))
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
	else if (event->GetCaller() == LuaEngine)
	{
		SHVConsole::ErrPrintf(_S("LUA Error(%d):\n%s\n"), event->GetSubID().IfNull(0), LuaScript->GetAndClearErrors().GetSafeBuffer());
	}
}

/*************************************
 * LuaTestFunc
 *************************************/
void SHVLuaEngineTester::LuaTestFunc(SHVLuaScript* script, SHVLuaFuncArgs &args)
{
	SHVUNUSED_PARAM(script);
	SHVConsole::Printf8("In lua test func\n  Arguments: %d\n", args.ArgCount());
	if (args.ArgCount())
		SHVConsole::Printf8("  Param 1 as int %d\n", args.ArgAsInt(0).IfNull(-1));
	args.PushDouble(1.245);
	args.PushString(_S("NOGET"));
	args.PushRef(new SHVRefInt(512)); // just a regular refobject, not validatable by SHVLuaRefInt
	args.PushRef(SHVLuaRefInt().Obj(new SHVRefInt(512)));
}

