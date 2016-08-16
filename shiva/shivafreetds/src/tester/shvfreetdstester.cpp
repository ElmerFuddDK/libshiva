#include "stdafx.h"
#include "shiva/include/platformspc.h"
#include "shiva/include/framework/shvconsole.h"
#include "shiva/include/framework/shveventstdin.h"
#include "shiva/include/utils/shvstringstream.h"
#include "shiva/include/utils/shvvector.h"

#include "shvfreetdstester.h"


//=========================================================================================================
// SHVshivafreetdsTester class
//=========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVFreeTDSTester::SHVFreeTDSTester(SHVModuleList& modules, SHVFreeTDSWrapper* wrapper) : SHVModule(modules,"FreeTDSTester"), FreeTDS(wrapper)
{
}

/*************************************
 * Register
 *************************************/
SHVBool SHVFreeTDSTester::Register()
{
	if (!FreeTDS.IsNull())
	{
	SHVString servername, port, username, password, database;
	
		servername = Modules.GetConfig().Find(_S("hostname"))->ToString();
		port = Modules.GetConfig().Find(_S("hostport"))->ToString();
		username = Modules.GetConfig().Find(_S("username"))->ToString();
		password = Modules.GetConfig().Find(_S("password"))->ToString();
		database = Modules.GetConfig().Find(_S("database"))->ToString();
		
		if (!servername.IsEmpty())
			FreeTDS->SetProperty(SHVFreeTDSWrapper::PropHostname,servername);
		if (!port.IsEmpty())
			FreeTDS->SetProperty(SHVFreeTDSWrapper::PropHostport,port);
		if (!username.IsEmpty())
			FreeTDS->SetProperty(SHVFreeTDSWrapper::PropUsername,username).SetProperty(SHVFreeTDSWrapper::PropPassword,password);
		if (!database.IsEmpty())
			FreeTDS->SetProperty(SHVFreeTDSWrapper::PropDatabase,database);
	}
	
	// Console input events
	Modules.EventSubscribe(__EVENT_GLOBAL_STDIN, new SHVEventSubscriber(this));

	return SHVModule::Register();
}

/*************************************
 * Unregister
 *************************************/
void SHVFreeTDSTester::Unregister()
{
	SHVModule::Unregister();
//	FreeTDS->DisconnectAll();
	FreeTDS = NULL;
	Connection = NULL;
}

/*************************************
 * OnEvent
 *************************************/
void SHVFreeTDSTester::OnEvent(SHVEvent* event)
{
	if (SHVEventString::Equals(event,__EVENT_GLOBAL_STDIN))
	{
	SHVString8 str(SHVEventStdin::StdinFromEvent(event));
	SHVString8 cmd, val;
	long space;
	
		space = str.Find(" ");
		if (space > 0)
		{
			cmd = str.Left(space);
			val = str.Mid(size_t(space)+1);
		}
	
		if (str == "/quit")
		{
			Modules.CloseApp();
		}
		else if (str == "/help")
		{
			SHVConsole::Printf8("Commands available:\n"
								" /quit         Will quit ...\n"
								" /help         Displays this info\n"
								" /set          Sets a connection property\n"
								" /show         Shows connection properties\n"
								" /connect      Connects to the server\n"
								" /disconnect   Disconnects\n"
								"\n");
		}
		else if (cmd == "/set")
		{
		SHVString8 prop, value;
		
			space = val.Find(" ");
			
			if (space > 0)
			{
				prop = val.Left(space);
				value = val.Mid(size_t(space)+1);
			}
			else
			{
				prop = val;
			}
			prop.MakeLower();
			
			FreeTDS->SetProperty(prop,value);
		}
		else if (str == "/show")
		{
			SHVConsole::Printf( _S("Settings OK: %s\n")
								_T(" Hostname : %s\n")
								_T(" Hostport : %s\n")
								_T(" Username : %s\n")
								_T(" Password : %s\n")
								_T(" Database : %s\n")
								_T("\n")
								, FreeTDS->PropertiesValid() ? _S("Yes") : _S("No")
								, FreeTDS->GetProperty(SHVFreeTDSWrapper::PropHostname).GetSafeBuffer()
								, FreeTDS->GetProperty(SHVFreeTDSWrapper::PropHostport).GetSafeBuffer()
								, FreeTDS->GetProperty(SHVFreeTDSWrapper::PropUsername).GetSafeBuffer()
								, FreeTDS->GetProperty(SHVFreeTDSWrapper::PropPassword).GetSafeBuffer()
								, FreeTDS->GetProperty(SHVFreeTDSWrapper::PropDatabase).GetSafeBuffer()
								);
		}
		else if (str == "/connect")
		{
			if (!Connection.IsNull())
				Connection->Disconnect();
			Connection = FreeTDS->CreateConnection(NULL);
			if (!Connection->Connect())
				SHVConsole::Printf8("Failed to connect with error %d\n", Connection->GetError());
			else
				SHVConsole::Printf8("Connected\n");
		}
		else if (str == "/disconnect")
		{
			if (!Connection.IsNull() && Connection->IsConnected())
			{
				SHVConsole::Printf8("Disconnecting\n");
				Connection->Disconnect();
			}
			Connection = NULL;
		}
		else if (str.Left(1) == "/")
		{
			SHVConsole::Printf8("Unknown command - try /help\n");
		}
		else if (Connection.IsNull() || !Connection->IsConnected())
		{
			SHVConsole::Printf8("Not connected\n");
		}
		else
		{
			if (Connection->ExecQuery(str.ToStrUTF8()))
			{
			SHVStringStream8 stream;
			SHVString8 value, colType;
			SHVVector<int> colLengths;
			int i, len, valLen;
			bool isNull;
			
				while (Connection->NextResult())
				{
					stream.Reset();
					for (i=0; i<Connection->GetColumnCount(); i++)
					{
						Connection->GetColumnType8(colType,i);
						Connection->GetColumnName8(value,i);

						if (colType == "int") len = 8;
						else if (colType == "int64") len = 10;
						else if (colType == "double") len = 10;
						else len = 50;
						
						if (len < 8)
							len = 8;
						else if (len > 50)
							len = 50;
						colLengths.Add(new int(len));
						valLen = (int)value.GetLength();
						if (valLen > len)
						{
							stream.AddChars8(value.GetSafeBuffer(),len-3);
							stream.AddChars8("... ",4);
						}
						else
						{
							stream.AddChars8(value.GetSafeBuffer(),valLen);
							for (int rest=len-valLen+1;rest>0;rest--)
								stream.AddChars8(" ",1);
						}
					}
					stream.Finalize();
					SHVConsole::Printf8("%s\n", stream.ToStr8().GetSafeBuffer());

					while (Connection->NextRow())
					{
						stream.Reset();
						for (i=0; i<Connection->GetColumnCount(); i++)
						{
							if (!Connection->GetString(value,i))
								value = "<NULL>";
							len = *colLengths[i];
							valLen = (int)value.GetLength();
							if (valLen > len)
							{
								stream.AddChars8(value.GetSafeBuffer(),len-3);
								stream.AddChars8("... ",4);
							}
							else
							{
								stream.AddChars8(value.GetSafeBuffer(),valLen);
								for (int rest=len-valLen+1;rest>0;rest--)
									stream.AddChars8(" ",1);
							}
						}
						stream.Finalize();
#ifdef __SHIVA_WIN32
						SHVConsole::Printf16(_S("%s\n"), stream.ToStr16().GetSafeBuffer());
#else
						SHVConsole::Printf8("%s\n", stream.ToStrUTF8().GetSafeBuffer());
#endif
					}
				}
			}
		}
	}
}
