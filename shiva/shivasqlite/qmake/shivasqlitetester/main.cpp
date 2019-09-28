
#include "../../../include/platformspc.h"
#include "../../../include/shvversion.h"
#include "../../../include/utils/shvdll.h"

#include "../../../include/frameworkimpl/shvmainthreadeventdispatcherconsole.h"
#include "../../../include/framework/shvmodulefactory.h"
#include "../../../include/sqlite/sqlitestatement.h"
#include "../../../include/sqlite/sqlitewrapper.h"
#include "../../../include/sqlite/shvstringsqlite.h"

class SHVTest : public SHVModule
{
private:
	SHVSQLiteWrapperRef Connection;
	SHVDll SqliteDLL;
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
		if (!SqliteDLL.Load(SqliteDLL.CreateLibFileName(_S("shivasqlite"))))
		{
			Modules.AddStartupError(_S("Could not load shivasqlite"));
			return false;
		}
		Connection = (SHVSQLiteWrapper*) SqliteDLL.CreateObjectInt(&Modules, SHVDll::ClassTypeUser);
		Connection->Open(_S("test.sqlite"));
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
			SHVString8 str(SHVEventStdin::StdinFromEvent8(event));
	
			if (str == SHVString8C("/quit"))
			{
				Modules.CloseApp();
			}
			else
			{
			SHVBool ok;
			SHVStringSQLite rest("");
			SHVSQLiteStatementRef statement = Connection->PrepareUTF8(ok, str.GetSafeBuffer(), rest);
				if (statement.IsNull() || (ok.GetError() != SHVSQLiteWrapper::SQLite_OK && ok.GetError() != SHVSQLiteWrapper::SQLite_ROW))
					printf("Not good\n");
				else
				{
					printf("Good\n");
					while (statement->NextResult().GetError() == SHVSQLiteWrapper::SQLite_ROW)
					{
						printf("Row: ");
						for (int i = 0; i < statement->GetColumnCount(); i++)
						{
						SHVStringSQLite name("");
						SHVStringSQLite val("");
						int len;
							statement->GetColumnNameUTF8(name, i);
							statement->GetStringUTF8(val, len, i);
							printf("%s = %s ", name.GetSafeBuffer(), val.GetSafeBuffer());
						}
						printf("\n");
					}
				}
			}
		}
	}

	virtual void Unregister()
	{
		Connection = NULL;
		printf("In unregister\n");
		SHVModule::Unregister();
	}
};

CONSOLEMAIN()
{
	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE))
	{
		fprintf(stderr,"WRONG SHIVA VERSION\n");
	}
	else
	{
	SHVMainThreadEventQueue mainqueue(new SHVMainThreadEventDispatcherConsole());
		mainqueue.GetModuleList().AddModule(new SHVTest(mainqueue.GetModuleList()));
		return mainqueue.Run().GetError();
	}
	
	return -1;
}
