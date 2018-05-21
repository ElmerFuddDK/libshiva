#include "stdafx.h"
#include "shiva/include/platformspc.h"
#include "shiva/include/utils/shvmath.h"

#include "shvfreetdswrapperimpl.h"


// =========================================================================================================
//  SHVFreeTDSConnectionImpl class - Implements the connection via FreeTDS
// =========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVFreeTDSConnectionImpl::SHVFreeTDSConnectionImpl(SHVFreeTDSWrapperImpl* owner)
	: Owner(owner)
{
	ConnectionThreadID = SHVThreadBase::InvalidThreadID;
	DbProc = NULL;
	Columns = NULL;
	ColCount = 0;
	SqlErrorLevel = LevelOK;
	
	if (!Owner->Initialized)
	{
		OK.SetError(SHVFreeTDSWrapper::ErrInitFail);
	}
	else
	{
		Login = Owner->GetLoginStruct();
		if (Login.IsNull())
		{
			OK.SetError(SHVFreeTDSWrapper::ErrGeneric);
		}
		else
		{
			OK.SetError(SHVFreeTDSWrapper::ErrNone);
		}
	}
}

/*************************************
 * Destructor
 *************************************/
SHVFreeTDSConnectionImpl::~SHVFreeTDSConnectionImpl()
{
	Login = NULL;
	DisconnectInternal();
}

/*************************************
 * IsOK
 *************************************/
SHVBool SHVFreeTDSConnectionImpl::IsOK()
{
	if (OK)
	{
		switch (SqlErrorLevel)
		{
		case LevelOK:
			return OK;
		case LevelRetriable:
			return SHVFreeTDSWrapper::ErrQueryRetriable;
		case LevelFatal:
			return SHVFreeTDSWrapper::ErrQuery;
		}
	}
	return OK;
}

/*************************************
 * Connect
 *************************************/
SHVBool SHVFreeTDSConnectionImpl::Connect()
{
	if (!OK)
		return OK;
	
	if (!DbProc && Owner)
	{
		OK = Owner->ConnectInternal(this);
	}
	
	return OK;
}

/*************************************
 * Disconnect
 *************************************/
void SHVFreeTDSConnectionImpl::Disconnect()
{
	if (Owner && Owner->RemoveActiveConnection(this))
		DisconnectInternal();
}

/*************************************
 * IsConnected
 *************************************/
bool SHVFreeTDSConnectionImpl::IsConnected()
{
	return DbProc ? true : false;
}

/*************************************
 * Interrupt
 *************************************/
void SHVFreeTDSConnectionImpl::Interrupt()
{
	if (OK)
		OK.SetError(SHVFreeTDSWrapper::ErrInterrupted);
}

/*************************************
 * NextRow
 *************************************/
SHVBool SHVFreeTDSConnectionImpl::NextRow()
{
RETCODE rowCode;
	
	if (!OK)
		return OK;
	
	SHVTHREADCHECK(ConnectionThreadID);
	
	if ((rowCode = dbnextrow(DbProc)) == NO_MORE_ROWS)
	{
		return false;
	}
#ifdef SYBSHVFAKEUNIQUE
	else
	{
		// Run through cols and update UUIDs
		for (int col=0;col<ColCount;col++)
		{
			if (Columns[col].InternalType == TDSColumn::TypeUUID && Columns[col].Status == 0 && SHVString8C::StrLen(Columns[col].Data.String) == 32)
			{
			char* from;
			char* to;
				from = Columns[col].Data.String + 31;
				to = Columns[col].Data.String + 35;
				for (int i=0; from != to; i++, to--)
				{
					switch(i)
					{
					case 12:
					case 17:
					case 22:
					case 27:
						*to = '-';
						break;
					default:
						*to = *from;
						from--;
						break;
					}
				}
				Columns[col].Data.String[36] = 0;
			}
		}
	}
#endif
	
	switch (rowCode)
	{
	case REG_ROW:
		break;
	case BUF_FULL:
		SHVASSERT(false);
	case FAIL:
		if (OK)
			OK.SetError(SHVFreeTDSWrapper::ErrGeneric);
		break;
	}
	
	return OK;
}

/*************************************
 * HasTDSMessage
 *************************************/
bool SHVFreeTDSConnectionImpl::HasTDSMessage()
{
	return SqlMessages.GetCount() ? true : false;
}

/*************************************
 * PopTDSMessage
 *************************************/
bool SHVFreeTDSConnectionImpl::PopTDSMessage(TDSMessage& msg)
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
void SHVFreeTDSConnectionImpl::ClearTDSMessages()
{
	SqlMessages.RemoveAll();
	SqlErrorLevel = LevelOK;
}

/*************************************
 * ExecQuery
 *************************************/
SHVBool SHVFreeTDSConnectionImpl::ExecQuery(const SHVStringUTF8C query, bool clearMessagesAndSqlError)
{
SHVBool retVal(OK);

	if (clearMessagesAndSqlError)
	{
		ClearTDSMessages();
	}

	if (retVal && !DbProc)
		retVal.SetError(SHVFreeTDSWrapper::ErrNotConnected);
	else if (retVal && !Owner)
		retVal.SetError(SHVFreeTDSWrapper::ErrGeneric);

	if (retVal)
	{
		SHVTHREADCHECK(ConnectionThreadID);
		
		if (dbfcmd(DbProc,"%s", query.GetSafeBuffer()) == FAIL)
			retVal.SetError(SHVFreeTDSWrapper::ErrQuery);
		else if (dbsqlexec(DbProc) == FAIL)
			retVal.SetError(SHVFreeTDSWrapper::ErrQuery);
	}
	
	return retVal;
}

/*************************************
 * ExecQueryPartial
 *************************************/
SHVBool SHVFreeTDSConnectionImpl::ExecQueryPartial(const SHVStringUTF8C query, SHVStringUTF8C* rest, bool clearMessagesAndSqlError)
{
SHVString8C restStr(query.GetSafeBuffer());
SHVString8 queryPart;
SHVBool retVal;

	if (clearMessagesAndSqlError)
	{
		ClearTDSMessages();
	}
	
	retVal = SplitQuery(query.GetBufferConst(),queryPart,restStr);
	if (rest)
		*rest = restStr.GetBufferConst();
	
	if (retVal)
		return ExecQuery(queryPart.GetBufferConst(),false);
	return retVal;
}

/*************************************
 * NextResult
 *************************************/
SHVBool SHVFreeTDSConnectionImpl::NextResult()
{
RETCODE ret;

	CleanupCols();

	if (!OK)
		return OK;
	
	SHVTHREADCHECK(ConnectionThreadID);
	
	ret = dbresults(DbProc);
	
	if (ret == FAIL)
	{
		OK.SetError(SHVFreeTDSWrapper::ErrGeneric);
	}
	else if (ret != NO_MORE_RESULTS)
	{
	int i;
	TDSColumn* col;
	
		// Parse the columns
		ColCount = dbnumcols(DbProc);
		
		if (ColCount > 0)
			Columns = (TDSColumn*)calloc(ColCount,sizeof(TDSColumn));
		
		for (col=Columns, i=0; i<ColCount && OK; col++)
		{
			i++;
			col->Name = dbcolname(DbProc, i);
			col->Type = dbcoltype(DbProc, i);
			col->Size = dbcollen(DbProc, i);
			col->Status = -1;
			
			ret = AllocateColumnBuffer(i,col);

			if (ret == FAIL)
				OK.SetError(SHVFreeTDSWrapper::ErrGeneric);
			
		}
		
		return true;
	}
	
	return false;
}

/*************************************
 * Reset
 *************************************/
SHVBool SHVFreeTDSConnectionImpl::Reset()
{
	OK.SetError(SHVFreeTDSWrapper::ErrNone);
	DisconnectInternal();
	if (Owner)
		return Owner->ConnectInternal(this);
	return false;
}

/*************************************
 * GetValue
 *************************************/
SHVBool SHVFreeTDSConnectionImpl::GetLong(long& val, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount || Columns[columnIdx].Status == -1)
		return false;
	return GetLongInternal(val,Columns[columnIdx]);
}
SHVBool SHVFreeTDSConnectionImpl::GetInt64(SHVInt64Val& val, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount || Columns[columnIdx].Status == -1)
		return false;
	return GetInt64Internal(val,Columns[columnIdx]);
}
SHVBool SHVFreeTDSConnectionImpl::GetDouble(double& val, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount || Columns[columnIdx].Status == -1)
		return false;
	return GetDoubleInternal(val,Columns[columnIdx]);
}
SHVBool SHVFreeTDSConnectionImpl::GetStringUTF8(SHVStringUTF8& text, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount || Columns[columnIdx].Status == -1)
		return false;
	return GetStringUTF8Internal(text,Columns[columnIdx]);
}
SHVBool SHVFreeTDSConnectionImpl::GetTime(SHVTime& val, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount || Columns[columnIdx].Status == -1)
		return false;
	return GetTimeInternal(val,Columns[columnIdx]);
}
SHVBool SHVFreeTDSConnectionImpl::GetUUID(SHVUUID::ID& val, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount || Columns[columnIdx].Status == -1)
		return false;
	return GetUUIDInternal(val,Columns[columnIdx]);
}

/*************************************
 * GetColumnName
 *************************************/
SHVBool SHVFreeTDSConnectionImpl::GetColumnNameUTF8(SHVStringUTF8& name, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount)
		return false;
	name = SHVStringUTF8C(Columns[columnIdx].Name);
	return true;
}
SHVBool SHVFreeTDSConnectionImpl::GetColumnName8(SHVString8& name, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount)
		return false;
	name = SHVStringUTF8C(Columns[columnIdx].Name).ToStr8();
	return true;
}
SHVBool SHVFreeTDSConnectionImpl::GetColumnName16(SHVString16& name, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount)
		return false;
	name = SHVStringUTF8C(Columns[columnIdx].Name).ToStr16();
	return true;
}

/*************************************
 * GetColumnType
 *************************************/
SHVBool SHVFreeTDSConnectionImpl::GetColumnTypeUTF8(SHVStringUTF8& colType, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount)
		return false;
	return GetColumnTypeUTF8Internal(colType,Columns[columnIdx].InternalType);
}
SHVBool SHVFreeTDSConnectionImpl::GetColumnType8(SHVString8& colType, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount)
		return false;
	return GetColumnType8Internal(colType,Columns[columnIdx].InternalType);
}
SHVBool SHVFreeTDSConnectionImpl::GetColumnType16(SHVString16& colType, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount || Columns[columnIdx].Status == -1)
		return false;
	return GetColumnType16Internal(colType,Columns[columnIdx].InternalType);
}

/*************************************
 * GetColumnCount
 *************************************/
int SHVFreeTDSConnectionImpl::GetColumnCount() const
{
	return ColCount;
}

///\cond INTERNAL
/*************************************
 * HandleTds*Internal
 *************************************/
void SHVFreeTDSConnectionImpl::HandleTdsErrorInternal(int severity, int dberr, int oserr, char* dberrstr, char* oserrstr)
{
	SHVUNUSED_PARAM(severity);
	SHVUNUSED_PARAM(dberr);
	SHVUNUSED_PARAM(dberrstr);
	// SHVTRACE(_S("FreeTDS Err(%d): %s (%d)\n"), severity, SHVString8C(dberrstr).ToStrT().GetSafeBuffer(), dberr);
	
	if (oserr != DBNOERR)
	{
		SqlErrorLevel = LevelFatal;
		if (OK.GetError() != SHVFreeTDSWrapper::ErrInterrupted)
		{
			OK.SetError(SHVFreeTDSWrapper::ErrGeneric);
			SHVTRACE(_S("  -> OS Err: %s (%d)\n"), SHVString8C(oserrstr).ToStrT().GetSafeBuffer(), oserr);
		}
		Disconnect();
	}
}
void SHVFreeTDSConnectionImpl::HandleTdsMessageInternal(DBINT msgno, int msgstate, int severity, char* msgtext, char* srvname, char* procname, int line)
{
TDSErrorLevels newLvl;
	SHVUNUSED_PARAM(srvname);
	SHVUNUSED_PARAM(procname);
	SHVUNUSED_PARAM(line);
	// SHVTRACE(_S("TDS Message(%d): Severity %d, state %d, line %d\n"), msgno, severity, msgstate, line);
	// SHVTRACE(_S("%s\n"), SHVString8C(msgtext).ToStrT().GetSafeBuffer());
	
	if (severity == 0)
	{
		newLvl = LevelOK;
	}
	else
	{
		switch(msgno)
		{
		case 701: // Out of Memory
		case 1204: // Lock Issue
		case 1205: // Deadlock Victim
		case 1222: // Lock request time out period exceeded.
		case 7214: // Remote procedure time out of %d seconds exceeded. Remote procedure '%.*ls' is canceled.
		case 7604: // Full-text operation failed due to a time out.
		case 7618: // %d is not a valid value for a full-text connection time out.
		case 8628: // A time out occurred while waiting to optimize the query. Rerun the query.
		case 8645: // A time out occurred while waiting for memory resources to execute the query. Rerun the query.
		case 8651:
			newLvl = LevelRetriable;
			break;
		default:
			newLvl = LevelFatal;
			break;
		}
	}
	
	if (newLvl > SqlErrorLevel)
		SqlErrorLevel = newLvl;

	SqlMessages.AddTail(TDSMessage(msgno,severity,msgstate,newLvl,SHVString8C(msgtext)));
}

/*************************************
 * SetInterrupted
 *************************************/
void SHVFreeTDSConnectionImpl::SetInterrupted()
{
	if (OK)
		OK.SetError(SHVFreeTDSWrapper::ErrInterrupted);
}
///\endcond

/*************************************
 * DisconnectInternal
 *************************************/
void SHVFreeTDSConnectionImpl::DisconnectInternal()
{
	if (DbProc)
	{
		dbclose(DbProc);
		DbProc = NULL;
	}
}

/*************************************
 * CleanupCols
 *************************************/
void SHVFreeTDSConnectionImpl::CleanupCols()
{
	if (Columns)
	{
	int i = 0;
		for (TDSColumn* col=Columns; i<ColCount; col++, i++)
		{
			if (col->Data.Buffer)
				free(col->Data.Buffer);
		}
		free(Columns);
		Columns = NULL;
		ColCount = 0;
	}
}

/*************************************
 * CalculateRetryDelay
 *************************************/
int SHVFreeTDSConnectionImpl::CalculateRetryDelay(int tries)
{
	if (SqlMessages.GetCount() && SqlMessages.GetLast().Msgno == 1205) // Deadlock victim
	{
		if (tries < 2)
			return SHVMath::Rand(RandomSeed)%(1500*(tries+1)) + 200*(tries+1);
		else if (tries < 5)
			return SHVMath::Rand(RandomSeed)%(5000*tries) + 1000*tries;
		else
			return SHVMath::Rand(RandomSeed)%30000 + 1000*tries;
	}
	
	return SHVMath::Rand(RandomSeed)%12000 + 3000;
}

/*************************************
 * SplitQuery
 *************************************/
SHVBool SHVFreeTDSConnectionImpl::SplitQuery(const SHVString8C query, SHVString8& queryPart, SHVString8C& rest)
{
const char* buf = query.GetSafeBuffer();
size_t len=0;
size_t partLen=0;
	while (*buf && !partLen)
	{
		if (*buf == '\n'
		        && *(buf+1) && (*(buf+1) == 'G' || *(buf+1) == 'g')
		        && *(buf+1) && (*(buf+2) == 'O' || *(buf+2) == 'o')
		        && (!*(buf+3) || (*(buf+3) == ';' || *(buf+3) == '\r') || *(buf+3) == '\n')
		        )
		{
			partLen = len;
			len += 3;
			buf += 3;
		}
		else
		{
			len++;
			buf++;
		}
	}
	if (partLen)
	{
	bool trimming = true;
		queryPart = query.Left(partLen);
		while (*buf && trimming)
		{
			switch (*buf)
			{
			case ';':
			case ' ':
			case '\r':
			case '\n':
				buf++;
				break;
			default:
				trimming = false;
				break;
			}
		}
		rest = buf;
	}
	else
	{
		queryPart = query;
		rest = NULL;
	}
	return true;
}

///\cond INTERNAL
/*************************************
 * AllocateColumnBuffer
 *************************************/
RETCODE SHVFreeTDSConnectionImpl::AllocateColumnBuffer(int i, SHVFreeTDSConnectionImpl::TDSColumn *col)
{
RETCODE ret;
	switch(col->Type)
	{
	// Int binds
	case SYBBIT:
	case SYBBITN:
	case SYBINT1:
	case SYBINT2:
	case SYBINT4:
		col->InternalType = TDSColumn::TypeInt;
		col->Data.Int = (int*)malloc(sizeof(int));
		ret = dbbind(DbProc, i, INTBIND, sizeof(int), (BYTE*)col->Data.Int);
		break;
	// Int64 binds
	case SYBINT8:
		col->InternalType = TDSColumn::TypeInt64;
		col->Data.Int64 = (SHVInt64Val*)malloc(sizeof(SHVInt64Val));
		ret = dbbind(DbProc, i, BIGINTBIND, sizeof(SHVInt64Val), (BYTE*)col->Data.Int64);
		break;
	// Time binds
#ifdef SYBMSDATE
	case SYBMSDATE:
#endif
#ifdef SYBMSTIME
	case SYBMSTIME:
#endif
	case SYBDATETIME:
	case SYBDATETIME4:
	case SYBDATETIMN:
		col->InternalType = TDSColumn::TypeTime;
		col->Data.Datetime = (DBDATETIME*)malloc(sizeof(DBDATETIME));
		ret = dbbind(DbProc, i, DATETIMEBIND, sizeof(DBDATETIME), (BYTE*)col->Data.String);
		break;
	// Float binds
	case SYBDECIMAL:
	case SYBFLT8:
	case SYBFLTN:
	case SYBREAL:
	case SYBNUMERIC:
		col->InternalType = TDSColumn::TypeDouble;
		col->Data.Double = (double*)malloc(sizeof(double));
		ret = dbbind(DbProc, i, FLT8BIND, sizeof(double), (BYTE*)col->Data.Double);
		break;
	// UUID binds
	case SYBUNIQUE:
		col->InternalType = TDSColumn::TypeUUID;
		col->Data.String = (char*)malloc(41);
		ret = dbbind(DbProc, i, NTBSTRINGBIND, 41, (BYTE*)col->Data.String);
		break;
#ifdef SYBSHVFAKEUNIQUE
	case SYBBINARY:
		if (col->Size == 16) // Assume it is UUID
		{
			col->InternalType = TDSColumn::TypeUUID;
			col->Data.String = (char*)malloc(41);
			ret = dbbind(DbProc, i, NTBSTRINGBIND, 41, (BYTE*)col->Data.String);
			break;
		}
		// Else continue
#endif
	// Text binds
	default:
		SHVASSERT(false); // Unknown type
	case SYBTEXT:
	case SYBNTEXT:
	case SYBCHAR:
	case SYBVARCHAR:
	case SYBNVARCHAR:
		col->InternalType = TDSColumn::TypeString;
		col->Data.String = (char*)malloc((col->Size*4)+1);
		ret = dbbind(DbProc, i, NTBSTRINGBIND, (col->Size*4)+1, (BYTE*)col->Data.String);
		break;
	}
	
	if (ret != FAIL)
		ret = dbnullbind(DbProc, i, &col->Status);
	
	return ret;
}

/*************************************
 * ReleaseColumnsInternal
 *************************************/
SHVFreeTDSConnectionImpl::TDSColumn* SHVFreeTDSConnectionImpl::ReleaseColumnsInternal()
{
TDSColumn* retVal;
	if (ColCount > 0 && OK)
	{
	int i=0;
		retVal = Columns;
		Columns = (TDSColumn*)malloc(ColCount*sizeof(TDSColumn));
		memcpy(Columns,retVal,ColCount*sizeof(TDSColumn));
		for (TDSColumn* col=Columns; i<ColCount && OK; col++)
		{
			i++;
			if (AllocateColumnBuffer(i,col) == FAIL)
				OK.SetError(SHVFreeTDSWrapper::ErrGeneric);
		}
	}
	else
	{
		retVal = NULL;
	}
	return retVal;
}

/*************************************
 * DbDateToTime
 *************************************/
SHVTime SHVFreeTDSConnectionImpl::DbDateToTime(const DBDATETIME* dateTime) const
{
SHVTime retVal;
DBDATEREC di;

	if (dbdatecrack(DbProc,&di,(DBDATETIME*)dateTime) != FAIL)
	{
#ifdef MSDBLIB
		retVal.SetYear(di.year).SetMonth(di.month).SetDay(di.day).SetHour(di.hour).SetMinute(di.minute).SetSecond(di.second).SetMillisecond(di.millisecond);
#else
		retVal.SetYear(di.dateyear).SetMonth(di.datemonth+1).SetDay(di.datedmonth).SetHour(di.datehour).SetMinute(di.dateminute).SetSecond(di.datesecond).SetMillisecond(di.datemsecond);
#endif
	}

	return retVal;
}

/*************************************
 * GetValueInternal
 *************************************/
SHVBool SHVFreeTDSConnectionImpl::GetLongInternal(long& val, const SHVFreeTDSConnectionImpl::TDSColumn& col) const
{
	switch (col.InternalType)
	{
	case TDSColumn::TypeInt:	val = *col.Data.Int; break;
	case TDSColumn::TypeInt64:	val = (long)*col.Data.Int64; break;
	case TDSColumn::TypeDouble: val = (long)*col.Data.Double; break;
	case TDSColumn::TypeUUID:
	case TDSColumn::TypeTime:	val = 0; break;
	case TDSColumn::TypeString:	val = SHVStringUTF8C(col.Data.String).ToLong(); break;
	default: SHVASSERT(false); return false;
	}
	return true;
}
SHVBool SHVFreeTDSConnectionImpl::GetInt64Internal(SHVInt64Val& val, const SHVFreeTDSConnectionImpl::TDSColumn& col) const
{
	switch (col.InternalType)
	{
	case TDSColumn::TypeInt:	val = *col.Data.Int; break;
	case TDSColumn::TypeInt64:	val = *col.Data.Int64; break;
	case TDSColumn::TypeDouble: val = (long)*col.Data.Double; break;
	case TDSColumn::TypeUUID:
	case TDSColumn::TypeTime:	val = 0; break;
	case TDSColumn::TypeString:	val = SHVStringUTF8C(col.Data.String).ToInt64(); break;
	default: SHVASSERT(false); return false;
	}
	return true;
}
SHVBool SHVFreeTDSConnectionImpl::GetDoubleInternal(double& val, const SHVFreeTDSConnectionImpl::TDSColumn& col) const
{
	switch (col.InternalType)
	{
	case TDSColumn::TypeInt:	val = *col.Data.Int; break;
	case TDSColumn::TypeInt64:	val = (double)*col.Data.Int64; break;
	case TDSColumn::TypeDouble: val = *col.Data.Double; break;
	case TDSColumn::TypeUUID:
	case TDSColumn::TypeTime:	val = 0; break;
	case TDSColumn::TypeString:	val = SHVStringUTF8C(col.Data.String).ToDouble(); break;
	default: SHVASSERT(false); return false;
	}
	return true;
}
SHVBool SHVFreeTDSConnectionImpl::GetStringUTF8Internal(SHVStringUTF8& text, const SHVFreeTDSConnectionImpl::TDSColumn& col) const
{
	switch (col.InternalType)
	{
	case TDSColumn::TypeInt:	text = SHVStringUTF8C::LongToString(*col.Data.Int); break;
	case TDSColumn::TypeInt64:	text = SHVStringUTF8C::Int64ToString(*col.Data.Int64); break;
	case TDSColumn::TypeDouble: text = SHVStringUTF8C::DoubleToString(*col.Data.Double); break;
	case TDSColumn::TypeTime:	text = DbDateToTime(col.Data.Datetime).ToDateString(SHVTime::MillisecInclude).ToStrUTF8(); break;
	case TDSColumn::TypeString:	text = SHVStringUTF8C(col.Data.String); break;
	case TDSColumn::TypeUUID:	text = SHVStringUTF8C(col.Data.String); break;
	default: SHVASSERT(false); return false;
	}
	return true;
}
SHVBool SHVFreeTDSConnectionImpl::GetTimeInternal(SHVTime& val, const SHVFreeTDSConnectionImpl::TDSColumn& col) const
{
	switch (col.InternalType)
	{
	case TDSColumn::TypeInt:
	case TDSColumn::TypeInt64:
	case TDSColumn::TypeDouble:
	case TDSColumn::TypeUUID:
	case TDSColumn::TypeString:
		val.SetNull();
		return false;
	case TDSColumn::TypeTime: val = DbDateToTime(col.Data.Datetime); break;
	default: SHVASSERT(false); return false;
	}
	return true;
}
SHVBool SHVFreeTDSConnectionImpl::GetUUIDInternal(SHVUUID::ID& val, const SHVFreeTDSConnectionImpl::TDSColumn& col) const
{
	switch (col.InternalType)
	{
	case TDSColumn::TypeInt:
	case TDSColumn::TypeInt64:
	case TDSColumn::TypeDouble:
	case TDSColumn::TypeTime:
	case TDSColumn::TypeString:
		val = SHVUUID::Null();
		return false;
	case TDSColumn::TypeUUID: val = SHVUUID::FromString8(SHVString8C(col.Data.String)); break;
	default: SHVASSERT(false); return false;
	}
	return true;
}

/*************************************
 * GetColumnTypeInternal
 *************************************/
SHVBool SHVFreeTDSConnectionImpl::GetColumnTypeUTF8Internal(SHVStringUTF8& colType, int internalType) const
{
	switch (internalType)
	{
	case TDSColumn::TypeInt:	colType = "int"; break;
	case TDSColumn::TypeInt64:	colType = "int64"; break;
	case TDSColumn::TypeDouble: colType = "double"; break;
	case TDSColumn::TypeTime:	colType = "time"; break;
	case TDSColumn::TypeUUID:	colType = "uuid"; break;
	case TDSColumn::TypeString:	colType = "string"; break;
	default: SHVASSERT(false); return false;
	}
	return true;
}
SHVBool SHVFreeTDSConnectionImpl::GetColumnType8Internal(SHVString8& colType, int internalType) const
{
	switch (internalType)
	{
	case TDSColumn::TypeInt:	colType = "int"; break;
	case TDSColumn::TypeInt64:	colType = "int64"; break;
	case TDSColumn::TypeDouble: colType = "double"; break;
	case TDSColumn::TypeTime:	colType = "time"; break;
	case TDSColumn::TypeUUID:	colType = "uuid"; break;
	case TDSColumn::TypeString:	colType = "string"; break;
	default: SHVASSERT(false); return false;
	}
	return true;
}
SHVBool SHVFreeTDSConnectionImpl::GetColumnType16Internal(SHVString16& colType, int internalType) const
{
SHVString8 colType8;
bool retVal = GetColumnType8Internal(colType8,internalType);
	colType = colType8.ToStr16();
	return retVal;
}
///\endcond
