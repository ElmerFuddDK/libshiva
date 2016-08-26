#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvfreetdswrapperimpl.h"


// =========================================================================================================
//  SHVFreeTDSTransactionImpl class - Implements the connection via FreeTDS
// =========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVFreeTDSTransactionImpl::SHVFreeTDSTransactionImpl(SHVFreeTDSConnectionImpl* connection, bool ownerOfConnection, int isolationLvl, SHVInt maxRetries)
	: Connection(connection), OwnerOfConnection(ownerOfConnection), IsolationLevel(isolationLvl), MaxRetries(maxRetries)
{
	if (Connection.IsNull() || !Connection->Owner->Initialized)
	{
		OK.SetError(SHVFreeTDSWrapper::ErrInitFail);
		Owner = NULL;
	}
	else
	{
		OK.SetError(SHVFreeTDSWrapper::ErrNone);
		Owner = Connection->Owner;
	}
	CurrentResult = SIZE_T_MAX;
	CurrentRowPos = NULL;
	CurrentRow = NULL;
	CurrentColCount = 0;
	Performed = false;
	SqlErrorLevel = LevelOK;
	Retries = 0;
}

/*************************************
 * Destructor
 *************************************/
SHVFreeTDSTransactionImpl::~SHVFreeTDSTransactionImpl()
{
	if (!Connection.IsNull() & OwnerOfConnection)
		Connection->Disconnect();
	Connection = NULL;
	Resultset.Clear();
	Queries.Clear();
}

/*************************************
 * IsOK
 *************************************/
SHVBool SHVFreeTDSTransactionImpl::IsOK()
{
	return (!OK ? OK : Connection->OK);
}

/*************************************
 * Disconnect
 *************************************/
void SHVFreeTDSTransactionImpl::Disconnect()
{
	Connection->Disconnect();
	if (OK)
		OK.SetError(SHVFreeTDSWrapper::ErrNotConnected);
}

/*************************************
 * IsConnected
 *************************************/
bool SHVFreeTDSTransactionImpl::IsConnected()
{
	return (!Connection.IsNull() && Connection->IsConnected());
}

/*************************************
 * Interrupt
 *************************************/
void SHVFreeTDSTransactionImpl::Interrupt()
{
SHVFreeTDSConnectionImplRef conn;
	// This is a bit tricky to implement as we don't know
	// if the Connection object is valid (Might be in the
	// process of being destroyed).
	// The owner can find it for us if it still active though.
	if (Owner && Owner->ValidateConnectionIsActive(Connection,conn))
	{
		conn->Interrupt();
	}
}

/*************************************
 * AddQuery
 *************************************/
SHVBool SHVFreeTDSTransactionImpl::AddQuery(const SHVStringUTF8C query)
{
	if (Performed)
		OK.SetError(SHVFreeTDSWrapper::ErrGeneric);
	if (OK)
	{
	SHVString8C restStr(query.GetSafeBuffer());
	SHVString8 queryPart;
		
		while (OK && !restStr.IsEmpty())
		{
			OK = Connection->SplitQuery(restStr,queryPart,restStr);
			if (OK)
				Queries.Add(new SHVString8(queryPart.ReleaseBuffer()));
		}
	}
	return OK;
}

/*************************************
 * PerformTransaction
 *************************************/
SHVBool SHVFreeTDSTransactionImpl::PerformTransaction()
{
size_t queryCount = Queries.CalculateCount();

	if (Performed)
		OK.SetError(SHVFreeTDSWrapper::ErrGeneric);
	
	if (!Connection->IsConnected())
		OK = Connection->Connect();

	if (OK && queryCount)
	{
	bool rollback;
		Retries = 0;
		do
		{
			rollback = false;
			OK = Connection->ExecQuery(SHVStringUTF8C::Format("%sBEGIN TRANSACTION", GetIsolationLevelString()),true);
			while (Connection->IsOK() && Connection->NextResult()) ;
			for (size_t i=0; i<queryCount && OK; i++)
			{
			Result* curResult;
				OK = Connection->ExecQuery(Queries[i]->GetBufferConst(),false);
				while (OK && Connection->NextResult())
				{
					Resultset.Add(curResult = new Result(Connection->Columns,Connection->ColCount));
					while (OK && Connection->NextRow())
					{
						curResult->Rows.AddTail(Connection->ReleaseColumnsInternal());
					}
					if (OK && !Connection->IsOK())
						OK = Connection->IsOK();
				}
				if (OK && !Connection->IsOK())
					OK = Connection->IsOK();
			}
			if (OK)
			{
				OK = Connection->ExecQuery("COMMIT TRANSACTION",false);
				while (Connection->IsOK() && Connection->NextResult()) ;
			}
			else if (Connection->IsOK() == SHVFreeTDSWrapper::ErrQueryRetriable
					 && (MaxRetries.IsNull() || Retries < MaxRetries))
			{
			int retryDelay = Connection->CalculateRetryDelay(Retries);
				OK = Connection->IsOK();
				Connection->ExecQuery("ROLLBACK TRANSACTION",false);
				Resultset.Clear();
				Connection->ClearTDSMessages();
				
				SHVThreadBase::Sleep(retryDelay);
			}
			else
			{
				rollback = true;
			}
			Retries++;
		}
		while(OK.GetError() == SHVFreeTDSWrapper::ErrQueryRetriable);
		SqlMessages = Connection->SqlMessages.ReleaseBuffer();
		SqlErrorLevel = Connection->SqlErrorLevel;
		Connection->SqlErrorLevel = SHVFreeTDSConnectionImpl::LevelOK;
		if (rollback && Connection->IsOK())
		{
			Connection->ExecQuery("ROLLBACK TRANSACTION",false);
			Connection->ClearTDSMessages();
		}
	}
	
	Performed = true;
	if (OwnerOfConnection)
		Connection->Disconnect();
	
	return OK;
}

/*************************************
 * NextResult
 *************************************/
SHVBool SHVFreeTDSTransactionImpl::NextResult()
{
size_t resultCount = Resultset.CalculateCount();

	CurrentRowPos = NULL;
	CurrentRow = NULL;
	CurrentColCount = 0;

	if (!Performed)
		return SHVBool(SHVFreeTDSWrapper::ErrGeneric);

	if (!OK)
		return OK;
	
	if (CurrentResult >= resultCount)
		CurrentResult = 0;
	else
		CurrentResult++;
	
	if (CurrentResult < resultCount)
	{
		CurrentColCount = (int)Resultset[CurrentResult]->ColNames.CalculateCount();
		return true;
	}
	return false;
}

/*************************************
 * NextRow
 *************************************/
SHVBool SHVFreeTDSTransactionImpl::NextRow()
{
size_t resultCount = Resultset.CalculateCount();
Result* curResult;

	if (!Performed || CurrentResult >= resultCount)
		return SHVBool(SHVFreeTDSWrapper::ErrGeneric);

	if (!OK)
		return OK;
	
	curResult = Resultset[CurrentResult];
	
	if (curResult->Rows.MoveNext(CurrentRowPos))
		CurrentRow = curResult->Rows.GetAt(CurrentRowPos);
	else
		CurrentRow = NULL;
	return CurrentRow ? true : false;
}

/*************************************
 * HasTDSMessage
 *************************************/
bool SHVFreeTDSTransactionImpl::HasTDSMessage()
{
	return SqlMessages.GetCount() ? true : false;
}

/*************************************
 * PopTDSMessage
 *************************************/
bool SHVFreeTDSTransactionImpl::PopTDSMessage(TDSMessage& msg)
{
	if (SqlMessages.GetCount())
	{
		msg = SqlMessages.PopHead();
		
		if (SqlMessages.GetCount() == 0)
			SqlErrorLevel = LevelOK;
		return true;
	}
	return false;
}

/*************************************
 * ClearTDSMessages
 *************************************/
void SHVFreeTDSTransactionImpl::ClearTDSMessages()
{
	SqlMessages.RemoveAll();
	SqlErrorLevel = LevelOK;
}

/*************************************
 * GetValue
 *************************************/
SHVBool SHVFreeTDSTransactionImpl::GetLong(long& val, int columnIdx) const
{
	if (!CurrentRow || columnIdx < 0 || columnIdx >= CurrentColCount || CurrentRow[columnIdx].Status == -1)
		return false;
	return Connection->GetLongInternal(val,CurrentRow[columnIdx]);
}
SHVBool SHVFreeTDSTransactionImpl::GetInt64(SHVInt64Val& val, int columnIdx) const
{
	if (!CurrentRow || columnIdx < 0 || columnIdx >= CurrentColCount || CurrentRow[columnIdx].Status == -1)
		return false;
	return Connection->GetInt64Internal(val,CurrentRow[columnIdx]);
}
SHVBool SHVFreeTDSTransactionImpl::GetDouble(double& val, int columnIdx) const
{
	if (!CurrentRow || columnIdx < 0 || columnIdx >= CurrentColCount || CurrentRow[columnIdx].Status == -1)
		return false;
	return Connection->GetDoubleInternal(val,CurrentRow[columnIdx]);
}
SHVBool SHVFreeTDSTransactionImpl::GetStringUTF8(SHVStringUTF8& text, int columnIdx) const
{
	if (!CurrentRow || columnIdx < 0 || columnIdx >= CurrentColCount || CurrentRow[columnIdx].Status == -1)
		return false;
	return Connection->GetStringUTF8Internal(text,CurrentRow[columnIdx]);
}
SHVBool SHVFreeTDSTransactionImpl::GetTime(SHVTime& val, int columnIdx) const
{
	if (!CurrentRow || columnIdx < 0 || columnIdx >= CurrentColCount || CurrentRow[columnIdx].Status == -1)
		return false;
	return Connection->GetTimeInternal(val,CurrentRow[columnIdx]);
}
SHVBool SHVFreeTDSTransactionImpl::GetUUID(SHVUUID::ID& val, int columnIdx) const
{
	if (!CurrentRow || columnIdx < 0 || columnIdx >= CurrentColCount || CurrentRow[columnIdx].Status == -1)
		return false;
	return Connection->GetUUIDInternal(val,CurrentRow[columnIdx]);
}

/*************************************
 * GetColumnName
 *************************************/
SHVBool SHVFreeTDSTransactionImpl::GetColumnNameUTF8(SHVStringUTF8& name, int columnIdx) const
{
const Result* curResult = (CurrentResult < Resultset.CalculateCount() ? Resultset[CurrentResult] : NULL);
	if (!curResult || columnIdx < 0 || columnIdx >= (int)curResult->ColNames.CalculateCount())
		return false;
	name = *curResult->ColNames[columnIdx];
	return true;
}
SHVBool SHVFreeTDSTransactionImpl::GetColumnName8(SHVString8& name, int columnIdx) const
{
const Result* curResult = (CurrentResult < Resultset.CalculateCount() ? Resultset[CurrentResult] : NULL);
	if (!curResult || columnIdx < 0 || columnIdx >= (int)curResult->ColNames.CalculateCount())
		return false;
	name = curResult->ColNames[columnIdx]->ToStr8();
	return true;
}
SHVBool SHVFreeTDSTransactionImpl::GetColumnName16(SHVString16& name, int columnIdx) const
{
const Result* curResult = (CurrentResult < Resultset.CalculateCount() ? Resultset[CurrentResult] : NULL);
	if (!curResult || columnIdx < 0 || columnIdx >= (int)curResult->ColNames.CalculateCount())
		return false;
	name = curResult->ColNames[columnIdx]->ToStr16();
	return true;
}

/*************************************
 * GetColumnType
 *************************************/
SHVBool SHVFreeTDSTransactionImpl::GetColumnTypeUTF8(SHVStringUTF8& colType, int columnIdx) const
{
const Result* curResult = (CurrentResult < Resultset.CalculateCount() ? Resultset[CurrentResult] : NULL);
	if (!curResult || columnIdx < 0 || columnIdx >= (int)curResult->ColInternalTypes.CalculateCount())
		return false;
	return Connection->GetColumnTypeUTF8Internal(colType,*curResult->ColInternalTypes[columnIdx]);
}
SHVBool SHVFreeTDSTransactionImpl::GetColumnType8(SHVString8& colType, int columnIdx) const
{
const Result* curResult = (CurrentResult < Resultset.CalculateCount() ? Resultset[CurrentResult] : NULL);
	if (!curResult || columnIdx < 0 || columnIdx >= (int)curResult->ColInternalTypes.CalculateCount())
		return false;
	return Connection->GetColumnType8Internal(colType,*curResult->ColInternalTypes[columnIdx]);
}
SHVBool SHVFreeTDSTransactionImpl::GetColumnType16(SHVString16& colType, int columnIdx) const
{
const Result* curResult = (CurrentResult < Resultset.CalculateCount() ? Resultset[CurrentResult] : NULL);
	if (!curResult || columnIdx < 0 || columnIdx >= (int)curResult->ColInternalTypes.CalculateCount())
		return false;
	return Connection->GetColumnType16Internal(colType,*curResult->ColInternalTypes[columnIdx]);
}

/*************************************
 * GetColumnCount
 *************************************/
int SHVFreeTDSTransactionImpl::GetColumnCount() const
{
	return CurrentColCount;
}

///\cond INTERNAL
/*************************************
 * GetIsolationLevelString
 *************************************/
const char *SHVFreeTDSTransactionImpl::GetIsolationLevelString()
{
	switch (IsolationLevel)
	{
	case SHVFreeTDSWrapper::IsolationLevelReadUncommitted:
		return "SET TRANSACTION ISOLATION LEVEL READ UNCOMMITTED;";
	case SHVFreeTDSWrapper::IsolationLevelReadCommitted:
		return "SET TRANSACTION ISOLATION READ COMMITTED;";
	case SHVFreeTDSWrapper::IsolationLevelRepeatableRead:
		return "SET TRANSACTION ISOLATION LEVEL REPEATABLE READ;";
	case SHVFreeTDSWrapper::IsolationLevelSerializable:
		return "SET TRANSACTION ISOLATION LEVEL SERIALIZABLE;";
	case SHVFreeTDSWrapper::IsolationLevelSnapshot:
		return "SET TRANSACTION ISOLATION LEVEL SNAPSHOT;";
	};
	return "";
}

/*************************************
 * SHVFreeTDSTransactionImpl::Result
 *************************************/
SHVFreeTDSTransactionImpl::Result::Result(SHVFreeTDSConnectionImpl::TDSColumn* cols, int colCount)
{
	for (int i=0; i<colCount; i++, cols++)
	{
		ColNames.Add(new SHVStringUTF8(cols->Name));
		ColInternalTypes.Add(new int(cols->InternalType));
	}
}
SHVFreeTDSTransactionImpl::Result::~Result()
{
SHVListPos pos = NULL;
int colCount = (int)ColNames.CalculateCount();
	while (Rows.MoveNext(pos))
	{
	int i = 0;
	SHVFreeTDSConnectionImpl::TDSColumn* cols = Rows.GetAt(pos);
		for (SHVFreeTDSConnectionImpl::TDSColumn* col=cols; i<colCount; col++, i++)
		{
			if (col->Data.Buffer)
				free(col->Data.Buffer);
		}
		free(cols);
	}
}
///\endcond
