#include "stdafx.h"
#include "shiva/include/platformspc.h"
#include "shiva/include/framework/shvconsole.h"
#include "shiva/include/framework/shveventstdin.h"
#include "shiva/include/utils/shvstringstream.h"
#include "shiva/include/utils/shvvector.h"
#include "shiva/include/utils/shvfile.h"
#include "shiva/include/utils/shvbufferc.h"

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
	Transaction = NULL;
}

/*************************************
 * OnEvent
 *************************************/
void SHVFreeTDSTester::OnEvent(SHVEvent* event)
{
	if (SHVEventString::Equals(event,__EVENT_GLOBAL_STDIN))
	{
	SHVString8 str(SHVEventStdin::StdinFromEvent8(event));
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
								" /quit            Will quit ...\n"
								" /help            Displays this info\n"
								" /set             Sets a connection property\n"
								" /show            Shows connection properties\n"
								" /connect         Connects to the server\n"
								" /disconnect      Disconnects\n"
								" begintrans       Starts a transaction that collects queries\n"
								" committrans      Commits the transaction\n"
								" execfile <name>  Opens a file and runs it"
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
			
			FreeTDS->SetProperty(prop.ToStrT(),value.ToStrT());
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
			Connection = FreeTDS->CreateConnection();
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
		else if (cmd == "execfile")
		{
		SHVFile f;
		SHVStringUTF8 sql;
			if (f.Open(val.ToStrT(),SHVFile::FlagOpen|SHVFile::FlagRead))
			{
			SHVString8 str;
			SHVBufferCRef buf;

				str.SetBufferSize(f.GetSize()+1);
				f.ReadString8(str);

				buf = new SHVBufferCPtr(str.GetBufferConst(),f.GetSize());

				f.Close();

				sql = str.ToStrUTF8();
				if (sql.IsEmpty())
				{
					SHVConsole::Printf8("Sql file empty\n");
				}
				else
				{
					ExecQuery(sql);
				}
			}
			else
			{
				SHVConsole::Printf8("No such file\n");
			}
		}
		else if (str == "begintrans")
		{
			if (!Transaction.IsNull())
				SHVConsole::Printf8("Already in transaction\n");
			else
				Transaction = FreeTDS->CreateTransaction(Connection,SHVFreeTDSWrapper::IsolationLevelRepeatableRead,3);
		}
		else if (str == "committrans")
		{
			if (Transaction.IsNull())
				SHVConsole::Printf8("Not in a transaction\n");
			else
			{
				// Uncomment next line for testing interruptability
				// StartInterrupt();
				if (Transaction->PerformTransaction())
					ParseResult(Transaction);
				if (!Transaction->IsOK())
				{
					SHVConsole::Printf8("Transaction failed with code %d\n", Transaction->GetError());
					for (size_t i=0; i<Transaction->GetQueryCount();i++)
						SHVConsole::Printf8("  Query %02d: %s\n", int(i), Transaction->GetQuery(i).GetSafeBuffer());
				}
				ParseMessages(Transaction);
				Transaction = NULL;
			}
		}
		else
		{
			ExecQuery(str.ToStrUTF8());
		}
	}
}

/*************************************
 * ExecQuery
 *************************************/
void SHVFreeTDSTester::ExecQuery(SHVStringUTF8C sql)
{
	if (!Transaction.IsNull())
	{
		Transaction->AddQuery(sql);
	}
	else if (Connection.IsNull() || !Connection->IsConnected())
	{
		SHVConsole::Printf8("Not connected\n");
	}
	else
	{
	bool clear = true;
		// Uncomment next line for testing interruptability
		// StartInterrupt();
		while (!sql.IsEmpty() && Connection->ExecQueryPartial(sql,&sql,clear))
		{
			clear = false;
			ParseResult(Connection);
		}
		if (!Connection->IsOK())
		{
			SHVConsole::Printf8("Query failed with %d\n", Connection->GetError());
			Connection->Reset();
		}
		ParseMessages(Connection);
	}
}

/*************************************
 * ParseResult
 *************************************/
void SHVFreeTDSTester::ParseResult(SHVFreeTDSResultset* result)
{
SHVStringStream8 stream;
SHVString value;
SHVString8 colType;
SHVVector<int> colLengths;
int i, len, valLen;

	while (result->NextResult())
	{
		stream.Reset();
		for (i=0; i<result->GetColumnCount(); i++)
		{
			result->GetColumnType8(colType,i);
			result->GetColumnName(value,i);

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
				stream.AddChars(value.GetSafeBuffer(),len-3);
				stream.AddChars8("... ",4);
			}
			else
			{
				stream.AddChars(value.GetSafeBuffer(),valLen);
				for (int rest=len-valLen+1;rest>0;rest--)
					stream.AddChars8(" ",1);
			}
		}
		stream.Finalize();
		SHVConsole::Printf8("%s\n", stream.ToStr8().GetSafeBuffer());

		while (result->NextRow())
		{
			stream.Reset();
			for (i=0; i<result->GetColumnCount(); i++)
			{
				if (!result->GetString(value,i))
					value = _S("<NULL>");
				len = *colLengths[i];
				valLen = (int)value.GetLength();
				if (valLen > len)
				{
					stream.AddChars(value.GetSafeBuffer(),len-3);
					stream.AddChars8("... ",4);
				}
				else
				{
					stream.AddChars(value.GetSafeBuffer(),valLen);
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

/*************************************
 * ParseMessages
 *************************************/
void SHVFreeTDSTester::ParseMessages(SHVFreeTDSResultset* result)
{
SHVFreeTDSResultset::TDSMessage msg;

	while (result->PopTDSMessage(msg))
	{
		SHVConsole::Printf(_S("TDS Message(%d): Severity %d, State %d\n"), msg.Msgno, msg.Severity, msg.State);
		SHVConsole::Printf(_S("    %s\n"), msg.Message.GetSafeBuffer());
	}
}
