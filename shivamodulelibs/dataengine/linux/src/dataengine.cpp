#include "../../../../include/platformspc.h"
#include "../../../../include/shvversion.h"
#include "../../../../include/utils/shvdll.h"

#include "../../../../include/frameworkimpl/shvmainthreadeventdispatcherconsole.h"
#include "../../../../include/framework/shvmodulefactory.h"
#include "../../../../include/modules/dataengine/shvdataengine.h"
#include "../../../../include/modules/dataengine/shvdatastruct.h"
#include "../../../../include/modules/dataengine/shvdatarowlist.h"


class SHVTest : public SHVModule
{
private:
	SHVDataEngine* DataEngine;
	SHVDataSessionRef DataSession;
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
		
		if (!SHVModuleResolver<SHVDataEngine>(Modules,DataEngine,"DataEngine"))
			return false;
		DataSession = DataEngine->CreateSession();
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
			if (str == SHVString8C("/create"))
			{
			SHVDataStructRef table = DataEngine->CreateStruct();
			SHVDataRowKeyRef key = DataEngine->CreateKey();
				table->Add("key", SHVDataVariant::TypeInt);
				table->Add("col1", SHVDataVariant::TypeString);
				table->Add("col2", SHVDataVariant::TypeString);
				table->SetTableName("testtable");
				key->AddKey("key", false);
				table->SetPrimaryIndex(key);
				table->SetIsMultiInstance(false);
				DataEngine->RegisterTable(table);
			}
			else
			if (str == SHVString8C("/insert"))
			{			
			SHVDataRowListRef rowList = DataSession->GetRows("testtable", _T(""), 0);
			SHVDataRowRef row;
				if (rowList.IsNull())
				{
					printf("Damn did not work\n");
				}
				else
				{
					rowList->EnableReplaceIfDuplicate(true);
					rowList->StartEdit();
					row = rowList->AddRow();
					row->SetInt(0, 1);
					row->SetString(1, "1-1");
					row->SetString(2, "1-2");
					row->AcceptChanges();
					row = rowList->AddRow();
					row->SetInt(0, 2);
					row->SetString(1, "2-1");
					row->SetString(2, "2-2");
					row->AcceptChanges();
					row = rowList->AddRow();
					row->SetInt(0, 3);
					row->SetString(1, "3-1");
					row->SetString(2, "3-2");
					row->AcceptChanges();
					rowList->EndEdit();
				}
			}
			else
			if (str == SHVString8C("/select"))
			{
			SHVDataRowListCRef rows = DataSession->GetRows("testtable", _T(""), 0);
				if (rows.IsNull())
				{
					printf("Verdamnt\n");
				}
				else
				{
					while (rows->NextRow())
					{
						printf("%d %s %s\r\n", 	(int) rows->GetCurrentRow()->AsInt(0), 
												rows->GetCurrentRow()->AsString(1).ToStr8().GetSafeBuffer(),
												rows->GetCurrentRow()->AsString(2).ToStr8().GetSafeBuffer());						
					}
				}
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
		DataSession = NULL;
		SHVModule::Unregister();
	}
};


int main(int argc, char *argv[])
{
SHVDll dataenginelib;
	
	SHVUNUSED_PARAM(argc);
	SHVUNUSED_PARAM(argv);

	if (!SHVModuleList::CheckVersion(__SHIVA_VERSION_MAJOR, __SHIVA_VERSION_MINOR, __SHIVA_VERSION_RELEASE))
	{
		fprintf(stderr,"WRONG SHIVA VERSION\n");
	}
	else if (!dataenginelib.Load(dataenginelib.CreateLibFileName(_T("dataengine"))))
	{
		fprintf(stderr,"Could not load dataengine\n");
	}
	else
	{
	SHVMainThreadEventQueue mainqueue(new SHVMainThreadEventDispatcherConsole());
	SHVModuleFactory* factory = (SHVModuleFactory*)dataenginelib.CreateObjectInt(&mainqueue.GetModuleList(),SHVDll::ClassTypeModuleFactory);

		mainqueue.GetModuleList().AddModule(new SHVTest(mainqueue.GetModuleList()));
		factory->ResolveModules(__MODULESYMBOL_DEFAULTS);

		return mainqueue.Run().GetError();
	}
	
	return -1;
}