#include "../../../../include/platformspc.h"
#include "../../../../include/shvversion.h"
#include "../../../../include/utils/shvdll.h"

#include "../../../../include/frameworkimpl/shvmainthreadeventdispatcherconsole.h"
#include "../../../../include/framework/shvmodulefactory.h"
#include "../../../../include/modules/dataengine/shvdataengine.h"
#include "../../../../include/modules/dataengine/shvdatastruct.h"
#include "../../../../include/modules/dataengine/shvdatarowlist.h"
#include "../../../../include/frameworkimpl/shvmoduleloaderimpl.h"
#include "../../../../include/modules/shvmodulefactories.h"


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
		SHVString8 str(SHVEventStdin::StdinFromEvent8(event));
	
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
				table->Add("unicode", SHVDataVariant::TypeString);
				table->SetTableName("testtable");
				key->AddKey("key", false);
				table->SetPrimaryIndex(key);
				table->SetIsMultiInstance(false);
				DataEngine->RegisterTable(table);
				printf("Done\n");
			}
			else
			if (str == SHVString8C("/insert"))
			{			
			SHVDataRowListRef rowList = DataSession->GetRows("testtable", _S(""), 0);
			SHVDataRowRef row;
				if (rowList.IsNull())
				{
					printf("Damn did not work\n");
				}
				else
				{
					rowList->EnableReplaceIfDuplicate(true);
					rowList->StartEdit();
					for (int i = 0; i < 1000000; i++)
					{
						row = rowList->AddRow();
						row->SetInt(0,i);
						row->SetString(1,SHVStringC::Format(_S("Number %d"), i));
						row->SetInt(2,i);
						row->SetString(3, _S("Blabber head"));
						row->AcceptChanges();
					}
					rowList->EndEdit();
					printf("Done\n");
/*
					rowList->StartEdit();
					row = rowList->AddRow();
					row->SetInt(0, 1);
					row->SetString(1, "1-1");
					row->SetString(2, "1-2");
					row->SetString(3, "æøå");
					row->AcceptChanges();
					row = rowList->AddRow();
					row->SetInt(0, 2);
					row->SetString(1, "2-1");
					row->SetString(2, "2-2");
					row->SetString(3, SHVStringC("æøå").ToStr16().ToStrUTF8().GetBufferConst());
					row->AcceptChanges();
					row = rowList->AddRow();
					row->SetInt(0, 3);
					row->SetString(1, "3-1");
					row->SetString(2, "3-2");
					row->SetString(3, "æøå");
					row->AcceptChanges();
					rowList->EndEdit();
*/
				}
			}
			else
			if (str == SHVString8C("/select"))
			{
			SHVDataRowListCRef rows = DataSession->GetRows("testtable", _S(""), 0);
				if (rows.IsNull())
				{
					printf("Verdamnt\n");
				}
				else
				{
					while (rows->NextRow())
					{
						printf("%d %s %s %s\r\n", 	(int) rows->GetCurrentRow()->AsInt(0), 
												rows->GetCurrentRow()->AsString(1).GetSafeBuffer(),
												rows->GetCurrentRow()->AsString(2).GetSafeBuffer(),
							  					rows->GetCurrentRow()->AsString(3).GetSafeBuffer());
					}
				}
			}
			else
			if (str == SHVString8C("/seek"))
			{
			SHVDataRowKeyRef key = DataEngine->CreateKey();
			SHVDataVariant* v = DataEngine->CreateVariant();
			SHVDataRowListCRef l = DataSession->GetRows("testtable", _S(""), 0);

				v->SetInt(500);
				key->AddKeyWithValue("key", v, false);
				l->SetPosition(key);
				printf("Found %s\n", l->GetCurrentRow()->AsString("col1").GetSafeBuffer());
				printf("Done\n");

			}
			else
			if (str == SHVString8C("/statement"))
			{
			SHVBool ok;
			SHVDataStatementRef statement = DataSession->PrepareStatement(
				            "SELECT key AS number, col1 AS data FROM testtable ORDER BY col2 limit 1;"
				            "SELECT key AS differentnumber, col1 AS moredata FROM testtable WHERE col1 = @param AND @param2 = 'enabled';"
				            "SELECT key AS differentnumber, col1 AS moredata FROM testtable WHERE col1 = 'Number 50';"
				            );
				printf("Statement ok : %d\n", statement->IsOk().GetError());
				printf("Reset %d\n", statement->Reset().GetError());
				statement->ClearAllParameters();
				printf("SetParameter %d\n", statement->SetParameterStringUTF8("@dummy","dummy").GetError());
				printf("SetParameter %d\n", statement->SetParameterStringUTF8("@param","Number 50").GetError());
				printf("SetParameter %d\n", statement->SetParameterStringUTF8("@param2","enabled").GetError());
				while (statement->NextResult())
				{
					printf("Result set start\n");
					while (statement->NextRow())
					{
					SHVString val;
					int len;
						statement->GetString(val,len,1);
						printf("  Data : %s\n", val.GetSafeBuffer());
					}
				}
				printf("Reset %d\n", statement->Reset().GetError());
				statement->ClearAllParameters();
				printf("SetParameter %d\n", statement->SetParameterStringUTF8("@param","Number 1234").GetError());
				printf("SetParameter %d\n", statement->SetParameterStringUTF8("@param2","enabled").GetError());
				while (statement->NextResult())
				{
					printf("Result set start\n");
					while (statement->NextRow())
					{
					SHVString val;
					int len;
						statement->GetString(val,len,1);
						printf("  Data : %s\n", val.GetSafeBuffer());
					}
				}
				printf("Reset %d\n", statement->Reset().GetError());
				statement->ClearAllParameters();
				while (statement->NextResult())
				{
					printf("Result set start\n");
					while (statement->NextRow())
					{
					SHVString val;
					int len;
						statement->GetString(val,len,1);
						printf("  Data : %s\n", val.GetSafeBuffer());
					}
				}
			}
			else
			if (str == SHVString8C("/test"))
			{
			SHVDataRowKeyRef key = DataEngine->CreateKey();
			SHVDataRowListRef l = DataSession->GetRows("testtable", _S(""), 0);
			SHVDataVariant* v = DataEngine->CreateVariant();
			SHVDataRow* row;
				l->StartEdit();
				printf("Deleting\n");
				v->SetInt(500);
				key->AddKeyWithValue("key", v, false);
				if (l->DeleteRow(key))
					printf("Now 500 is gone\n");
				if (!l->DeleteRow(key))
					printf("For sure\n");

				printf("Updateing\n");
				key->SetKeyValue(0, SHVInt(501));
				l->SetPosition(key);
				row = l->EditCurrentRow();
				row->SetString(3, _S("Fister"));
				row->AcceptChanges();
				l->Reset();
				l->EndEdit();
				l->SetPosition(key);
				printf("Is corrected to = %s\n", l->GetCurrentRow()->AsString(3).GetSafeBuffer());
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
	else
	{
	SHVMainThreadEventQueue mainqueue(new SHVMainThreadEventDispatcherConsole());
	SHVModuleLoaderImpl loader(mainqueue.GetModuleList());
#ifdef SHIVASTATICMODULELIB
		loader.AddModuleFactory(SHVModuleFactory_DataEngineNew(&mainqueue.GetModuleList()));
#else
		loader.AddModuleLib(dataenginelib.CreateLibFileName(_S("dataengine")));
#endif

		mainqueue.GetModuleList().AddModule(new SHVTest(mainqueue.GetModuleList()));
		loader.AddSymbol(__MODULESYMBOL_DEFAULTS);
		loader.LoadModules();

		return mainqueue.Run().GetError();
	}
	
	return -1;
}
