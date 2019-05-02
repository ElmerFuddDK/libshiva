#ifndef __SHIVA_FREETDS_RESULTSET_H
#define __SHIVA_FREETDS_RESULTSET_H

#include "shiva/include/shvtypes.h"
#include "shiva/include/utils/shvstring.h"
#include "shiva/include/utils/shvtime.h"
#include "shiva/include/utils/shvuuid.h"
#include "shiva/include/utils/shvrefobject.h"


//-=========================================================================================================
///  SHVFreeTDSResultset class - Interface for parsing results
/**
 * Common interface for parsing results from the SQL server.
 * Used through a common connection, or a transaction.
 */
	
class SHVFreeTDSResultset : public SHVRefObject
{
	
public:
	
	enum TDSErrorLevels {
		LevelOK,
		LevelRetriable,
		LevelFatal
	};
	
	struct TDSMessage {
		int Msgno, Severity, State;
		TDSErrorLevels Level;
		SHVString Message;
		inline TDSMessage() : Msgno(0), Severity(0), State(0), Level(LevelOK) {}
		inline TDSMessage(int msgno, int severity, int state, TDSErrorLevels lvl, const SHVStringC msg) : Msgno(msgno), Severity(severity), State(state), Level(lvl), Message(msg) {}
		inline TDSMessage(const TDSMessage& msg) : Msgno(msg.Msgno), Severity(msg.Severity), State(msg.State), Level(msg.Level), Message(((TDSMessage*)&msg)->Message.ReleaseBuffer()) {}
		inline TDSMessage& operator=(const TDSMessage& msg) { Msgno = msg.Msgno; Severity = msg.Severity; State = msg.State; Level = msg.Level; Message = ((TDSMessage*)&msg)->Message.ReleaseBuffer(); return *this; }
	};


	// Error handling
	virtual SHVBool IsOK() = 0;
	inline int GetError();
	
	// Iteration methods
	virtual SHVBool NextResult() = 0;
	virtual SHVBool NextRow() = 0;

	// Messages from the SQL server
	virtual bool HasTDSMessage() = 0;
	virtual bool PopTDSMessage(TDSMessage& msg) = 0;
	virtual void ClearTDSMessages() = 0;
	
	// GetValue
	inline  SHVBool GetInt(int& val, int columnIdx) const;
	virtual SHVBool GetLong(long& val, int columnIdx) const = 0;
	virtual SHVBool GetInt64(SHVInt64Val& val, int columnIdx) const = 0;
	virtual SHVBool GetDouble(double& val, int columnIdx) const = 0;
	virtual SHVBool GetStringUTF8(SHVStringUTF8& text, int columnIdx) const = 0;
	inline SHVBool GetString(SHVString& text, int columnIdx) const;
	virtual SHVBool GetTime(SHVTime& val, int columnIdx) const = 0;
	virtual SHVBool GetUUID(SHVUUID::ID& val, int columnIdx) const = 0;

	// GetColumnName
	virtual SHVBool GetColumnNameUTF8(SHVStringUTF8& name, int columnIdx) const = 0;
	virtual SHVBool GetColumnName8(SHVString8& name, int columnIdx) const = 0;
	virtual SHVBool GetColumnName16(SHVString16& name, int columnIdx) const = 0;
	inline SHVBool GetColumnName(SHVString& name, int columnIdx) const;

	// GetColumnType
	virtual SHVBool GetColumnTypeUTF8(SHVStringUTF8& colType, int columnIdx) const = 0;
	virtual SHVBool GetColumnType8(SHVString8& colType, int columnIdx) const = 0;
	virtual SHVBool GetColumnType16(SHVString16& colType, int columnIdx) const = 0;
	inline SHVBool GetColumnType(SHVString& colType, int columnIdx) const;

	// GetColumnCount
	virtual int GetColumnCount() const = 0;

};
typedef SHVRefObjectContainer<SHVFreeTDSResultset> SHVFreeTDSResultsetRef;


// ============================================ implementation ============================================ //

/*************************************
 * GetError
 *************************************/
int SHVFreeTDSResultset::GetError() { return IsOK().GetError(); }

/*************************************
 * GetValue
 *************************************/
SHVBool SHVFreeTDSResultset::GetInt(int& val, int columnIdx) const
{
long lval;
SHVBool retVal = GetLong(lval, columnIdx);
	val = (int) lval;
	return retVal;
}
SHVBool SHVFreeTDSResultset::GetString(SHVString& text, int columnIdx) const
{
#if __SHVSTRINGDEFAULT == utf8
	return GetStringUTF8(text, columnIdx);
#else
SHVStringUTF8 res(NULL);
SHVBool retVal = GetStringUTF8(res, columnIdx);
	if (retVal)
		text = res.ToStrT();
	return retVal;
#endif
}

/*************************************
 * GetColumnName
 *************************************/
SHVBool SHVFreeTDSResultset::GetColumnName(SHVString& name, int columnIdx) const
{
#if __SHVSTRINGDEFAULT == 8
	return GetColumnName8(name, columnIdx);
#elif __SHVSTRINGDEFAULT == 16
	return GetColumnName16(name, columnIdx);
#elif __SHVSTRINGDEFAULT == utf8
	return GetColumnNameUTF8(name, columnIdx);
#endif
}

/*************************************
 * GetColumnType
 *************************************/
SHVBool SHVFreeTDSResultset::GetColumnType(SHVString& colType, int columnIdx) const
{
#if __SHVSTRINGDEFAULT == 8
	return GetColumnType8(colType, columnIdx);
#elif __SHVSTRINGDEFAULT == 16
	return GetColumnType16(colType, columnIdx);
#elif __SHVSTRINGDEFAULT == utf8
	return GetColumnTypeUTF8(colType, columnIdx);
#endif
}

#endif
