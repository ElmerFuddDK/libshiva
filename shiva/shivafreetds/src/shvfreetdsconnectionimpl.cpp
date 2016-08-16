#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvfreetdswrapperimpl.h"


// =========================================================================================================
//  SHVFreeTDSConnectionImpl class - Implements the connection via FreeTDS
// =========================================================================================================

/*************************************
 * Constructor
 *************************************/
SHVFreeTDSConnectionImpl::SHVFreeTDSConnectionImpl(SHVFreeTDSWrapperImpl* owner, SHVEventSubscriberBase* subs)
	: Owner(owner), Subscriber(subs)
{
	DbProc = NULL;
	Columns = NULL;
	ColCount = 0;
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
 * GetError
 *************************************/
SHVBool SHVFreeTDSConnectionImpl::GetError()
{
	return OK;
}

/*************************************
 * GetError
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
 * ExecQuery
 *************************************/
SHVBool SHVFreeTDSConnectionImpl::ExecQuery(const SHVStringUTF8C query)
{
SHVBool retVal(OK);

	if (retVal && !DbProc)
		retVal.SetError(SHVFreeTDSWrapper::ErrNotConnected);
	else if (retVal && !Owner)
		retVal.SetError(SHVFreeTDSWrapper::ErrGeneric);

	if (retVal)
	{
		if (dbfcmd(DbProc,"%s", query.GetSafeBuffer()) == FAIL)
			retVal.SetError(SHVFreeTDSWrapper::ErrQuery);
		else if (dbsqlexec(DbProc) == FAIL)
			retVal.SetError(SHVFreeTDSWrapper::ErrQuery);
	}
	
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
			
			if (ret == FAIL)
				OK.SetError(SHVFreeTDSWrapper::ErrGeneric);
			
		}
		
		return true;
	}
	
	return false;
}

/*************************************
 * NextRow
 *************************************/
SHVBool SHVFreeTDSConnectionImpl::NextRow()
{
RETCODE rowCode;
		
	if (!OK)
		return OK;
	
	if ((rowCode = dbnextrow(DbProc)) == NO_MORE_ROWS)
	{
		return false;
	}
	
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
	switch (Columns[columnIdx].InternalType)
	{
	case TDSColumn::TypeInt:	val = *Columns[columnIdx].Data.Int; break;
	case TDSColumn::TypeInt64:	val = (long)*Columns[columnIdx].Data.Int64; break;
	case TDSColumn::TypeDouble: val = (long)*Columns[columnIdx].Data.Double; break;
	case TDSColumn::TypeUUID:
	case TDSColumn::TypeTime:	val = 0; break;
	case TDSColumn::TypeString:	val = SHVStringUTF8C(Columns[columnIdx].Data.String).ToLong(); break;
	default: SHVASSERT(false); return false;
	}
	return true;
}
SHVBool SHVFreeTDSConnectionImpl::GetInt64(SHVInt64Val& val, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount || Columns[columnIdx].Status == -1)
		return false;
	switch (Columns[columnIdx].InternalType)
	{
	case TDSColumn::TypeInt:	val = *Columns[columnIdx].Data.Int; break;
	case TDSColumn::TypeInt64:	val = *Columns[columnIdx].Data.Int64; break;
	case TDSColumn::TypeDouble: val = (long)*Columns[columnIdx].Data.Double; break;
	case TDSColumn::TypeUUID:
	case TDSColumn::TypeTime:	val = 0; break;
	case TDSColumn::TypeString:	val = SHVStringUTF8C(Columns[columnIdx].Data.String).ToInt64(); break;
	default: SHVASSERT(false); return false;
	}
	return true;
}
SHVBool SHVFreeTDSConnectionImpl::GetDouble(double& val, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount || Columns[columnIdx].Status == -1)
		return false;
	switch (Columns[columnIdx].InternalType)
	{
	case TDSColumn::TypeInt:	val = *Columns[columnIdx].Data.Int; break;
	case TDSColumn::TypeInt64:	val = (double)*Columns[columnIdx].Data.Int64; break;
	case TDSColumn::TypeDouble: val = *Columns[columnIdx].Data.Double; break;
	case TDSColumn::TypeUUID:
	case TDSColumn::TypeTime:	val = 0; break;
	case TDSColumn::TypeString:	val = SHVStringUTF8C(Columns[columnIdx].Data.String).ToDouble(); break;
	default: SHVASSERT(false); return false;
	}
	return true;
}
SHVBool SHVFreeTDSConnectionImpl::GetStringUTF8(SHVStringUTF8& text, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount || Columns[columnIdx].Status == -1)
		return false;
	switch (Columns[columnIdx].InternalType)
	{
	case TDSColumn::TypeInt:	text = SHVStringUTF8C::LongToString(*Columns[columnIdx].Data.Int); break;
	case TDSColumn::TypeInt64:	text = SHVStringUTF8C::Int64ToString(*Columns[columnIdx].Data.Int64); break;
	case TDSColumn::TypeDouble: text = SHVStringUTF8C::DoubleToString(*Columns[columnIdx].Data.Double); break;
	case TDSColumn::TypeTime:	text = DbDateToTime(Columns[columnIdx].Data.Datetime).ToDateString(SHVTime::MillisecInclude).ToStrUTF8(); break;
	case TDSColumn::TypeString:	text = SHVStringUTF8C(Columns[columnIdx].Data.String); break;
	case TDSColumn::TypeUUID:	text = SHVStringUTF8C(Columns[columnIdx].Data.String); break;
	default: SHVASSERT(false); return false;
	}
	return true;
}
SHVBool SHVFreeTDSConnectionImpl::GetTime(SHVTime& time, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount || Columns[columnIdx].Status == -1)
		return false;
	switch (Columns[columnIdx].InternalType)
	{
	case TDSColumn::TypeInt:
	case TDSColumn::TypeInt64:
	case TDSColumn::TypeDouble:
	case TDSColumn::TypeUUID:
	case TDSColumn::TypeString:
		time.SetNull();
		return false;
	case TDSColumn::TypeTime: time = DbDateToTime(Columns[columnIdx].Data.Datetime); break;
	default: SHVASSERT(false); return false;
	}
	return true;
}
SHVBool SHVFreeTDSConnectionImpl::GetUUID(SHVUUID::ID &uuid, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount || Columns[columnIdx].Status == -1)
		return false;
	switch (Columns[columnIdx].InternalType)
	{
	case TDSColumn::TypeInt:
	case TDSColumn::TypeInt64:
	case TDSColumn::TypeDouble:
	case TDSColumn::TypeTime:
	case TDSColumn::TypeString:
		uuid = SHVUUID::Null();
		return false;
	case TDSColumn::TypeUUID: uuid = SHVUUID::FromString8(SHVString8C(Columns[columnIdx].Data.String)); break;
	default: SHVASSERT(false); return false;
	}
	return true;
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
	if (columnIdx < 0 || columnIdx >= ColCount || Columns[columnIdx].Status == -1)
		return false;
	switch (Columns[columnIdx].InternalType)
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
SHVBool SHVFreeTDSConnectionImpl::GetColumnType8(SHVString8& colType, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount || Columns[columnIdx].Status == -1)
		return false;
	switch (Columns[columnIdx].InternalType)
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
SHVBool SHVFreeTDSConnectionImpl::GetColumnType16(SHVString16& colType, int columnIdx) const
{
	if (columnIdx < 0 || columnIdx >= ColCount || Columns[columnIdx].Status == -1)
		return false;
	switch (Columns[columnIdx].InternalType)
	{
	case TDSColumn::TypeInt:	colType = SHVString8C("int").ToStr16(); break;
	case TDSColumn::TypeInt64:	colType = SHVString8C("int64").ToStr16(); break;
	case TDSColumn::TypeDouble: colType = SHVString8C("double").ToStr16(); break;
	case TDSColumn::TypeTime:	colType = SHVString8C("time").ToStr16(); break;
	case TDSColumn::TypeUUID:	colType = SHVString8C("uuid").ToStr16(); break;
	case TDSColumn::TypeString:	colType = SHVString8C("string").ToStr16(); break;
	default: SHVASSERT(false); return false;
	}
	return true;
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
	SHVTRACE(_S("FreeTDS Err(%d): %s (%d)\n"), severity, SHVString8C(dberrstr).ToStrT().GetSafeBuffer(), dberr);
	
	if (oserr != DBNOERR)
		SHVTRACE(_S("  -> OS Err: %s (%d)\n"), SHVString8C(oserrstr).ToStrT().GetSafeBuffer(), oserr);
			
	///\todo Add proper error handling
}
void SHVFreeTDSConnectionImpl::HandleTdsMessageInternal(DBINT msgno, int msgstate, int severity, char* msgtext, char* srvname, char* procname, int line)
{
	SHVUNUSED_PARAM(srvname);
	SHVUNUSED_PARAM(procname);
	SHVTRACE(_S("TDS Message(%d): Severity %d, state %d, line %d\n"), msgno, severity, msgstate, line);
	SHVTRACE(_S("%s\n"), SHVString8C(msgtext).ToStrT().GetSafeBuffer());
	///\todo Add proper message handling
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
 * DbDateToTime
 *************************************/
SHVTime SHVFreeTDSConnectionImpl::DbDateToTime(const DBDATETIME* dateTime) const
{
SHVTime retVal;
DBDATEREC di;

	if (dbdatecrack(DbProc,&di,(DBDATETIME*)dateTime) != FAIL)
	{
#ifdef MSDBLIB
		retVal.SetYear(di.year)..SetMonth(di.month).SetDay(di.day).SetHour(di.hour).SetMinute(di.minute).SetSecond(di.second).SetMillisecond(di.millisecond);
#else
		retVal.SetYear(di.dateyear).SetMonth(di.datemonth+1).SetDay(di.datedmonth).SetHour(di.datehour).SetMinute(di.dateminute).SetSecond(di.datesecond).SetMillisecond(di.datemsecond);
#endif
	}

	return retVal;
}
