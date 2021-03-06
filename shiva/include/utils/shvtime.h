#ifndef __SHIVA_UTILS_TIME_H
#define __SHIVA_UTILS_TIME_H

#include "shvstring.h"
#include "shvlist.h"

#ifndef __SHIVA_WINCE
#include <time.h>
#elif !defined(_TM_DEFINED)
#define _TM_DEFINED
struct tm {
        int tm_sec;     /* seconds after the minute - [0,59] */
        int tm_min;     /* minutes after the hour - [0,59] */
        int tm_hour;    /* hours since midnight - [0,23] */
        int tm_mday;    /* day of the month - [1,31] */
        int tm_mon;     /* months since January - [0,11] */
        int tm_year;    /* years since 1900 */
        int tm_wday;    /* days since Sunday - [0,6] */
        int tm_yday;    /* days since January 1 - [0,365] */
        int tm_isdst;   /* daylight savings time flag */
        };
#endif
struct SHVTimeDstBoundary;
typedef SHVList<SHVTimeDstBoundary> SHVTimeDstBoundaryList;


//-=========================================================================================================
/// SHVTime class
/**
 * This class provides functionality for time calculation and conversion. It provides a mechanism
 * for creating time strings, and creating time objects from strings.
 */

class SHVAPI SHVTime
{
public:


	// defines
	enum Errors {
		ErrInvalidString = 1
	};

	enum {
		MinuteInSeconds = 60,
		HourInSeconds = 60*60,
		DayInSeconds = 24*60*60
	};

	enum MillisecFlags {
		MillisecExclude,
		MillisecInclude,
		MillisecAuto
	};


	// constructor
	SHVTime(bool localTime=false);


	// properties
	bool IsLocalTime() const;
	SHVTime& SetLocalTime(bool local, bool convert = true);
	bool IsNull() const;
	void SetNull();

	int GetYear() const;
	int GetMonth() const;
	int GetDay() const;
	int GetHour() const;
	int GetMinute() const;
	int GetSecond() const;
	int GetMillisecond() const;
	SHVTime& SetYear(int year);
	SHVTime& SetMonth(int month);
	SHVTime& SetDay(int day);
	SHVTime& SetHour(int hour);
	SHVTime& SetMinute(int minute);
	SHVTime& SetSecond(int second);
	SHVTime& SetMillisecond(int msecond);


	// calculated properties
	int CalculateDayOfWeek(); // 0 = sunday and so forth
	int CalculateLeapDayInEffect(); // returns 1 if leap day is in effect at current date (02/29 excluded) or 0 if not
	int CalculateWeekNumber(); // ISO 8601
	bool CalculateIsDst(); // Is daylight savings


	// validation
	SHVBool CheckValid() const;
	void MakeValid();


	// functions
	SHVBool SetFromDateString(const SHVStringC& dateStr);
	SHVStringBuffer ToDateString(MillisecFlags msFlag = MillisecAuto) const;
	SHVStringBuffer Format(const SHVStringC s) const;
	void SetNow(int diffInSeconds=0);
	void SetNowWithMilliseconds(int diffInMilliseconds=0);
	void AddSeconds(int seconds);


	// Dst boundary stuff
	static void CalculateDstBoundaries(SHVTimeDstBoundaryList& list, SHVTime from, SHVTime to);


	// Set system time
	bool SetSystemTime();


	// operators
	friend SHVTime SHVAPI operator+(const SHVTime& tTime, int diffInSeconds);
	friend bool SHVAPI operator>(const SHVTime& tTime1, const SHVTime& tTime2);
	friend bool SHVAPI operator<(const SHVTime& tTime1, const SHVTime& tTime2);
	friend bool SHVAPI operator>=(const SHVTime& tTime1, const SHVTime& tTime2);
	friend bool SHVAPI operator<=(const SHVTime& tTime1, const SHVTime& tTime2);
	friend bool SHVAPI operator!=(const SHVTime& tTime1, const SHVTime& tTime2);
	friend bool SHVAPI operator==(const SHVTime& tTime1, const SHVTime& tTime2);


	// statics
	static long GetRelativeTimeInMilliSecs();
	static SHVTime FromUnixTime(SHVInt64Val unixTime);
	static SHVInt64Val ToUnixTime(const SHVTime& time);
	static SHVTime FromDateString(const SHVStringC& dateStr);
	static SHVStringBuffer CreateDateStringNow(int diffInSeconds=0);
	static int MonthToDays(int month);
	static int DaysInMonth(int month, int year);
	static int GapInSeconds(const SHVTime& from, const SHVTime& to);

private:
	///\cond INTERNAL
	tm Time;
	int Millisecond;
	bool LocalTime;

	static time_t TimeGm(struct tm *t, bool setDst = true);
	static struct tm* LocalTime_r(const time_t *timep, struct tm *result);
	static struct tm* GmTime_r(const time_t *timep, struct tm *result);
	static time_t MkTime(struct tm *t);
	static time_t TimeNow();
	static time_t TimeNowWithMilliseconds(int& msecs);
	#ifdef __SHIVA_WINCE
	static void SetDstAndWeekday(struct tm* t, time_t* ttime = NULL, bool addingBias = false);
	static void SetWeekdayAndYearday(struct tm* t); // gets called from the above function
	static bool IsLastDOW(struct tm* t, SYSTEMTIME& DOW);
	#endif
	#ifdef __SHIVA_WIN32
	static int GetTimeFormatS(LCID Locale, DWORD dwFlags, CONST SYSTEMTIME *lpTime, const SHVTChar* lpFormat, SHVTChar* lpTimeStr, int cchTime);
	static int GetDateFormatS(LCID Locale, DWORD dwFlags, CONST SYSTEMTIME *lpDate, const SHVTChar* lpFormat, SHVTChar* lpDateStr, int cchDate);
	#endif
	///\endcond
};

struct SHVTimeDstBoundary {
	SHVTime From, To;
	bool FromIsDst, ToIsDst;
	inline SHVTimeDstBoundary(SHVTime& wt) : From(wt), To(wt) { FromIsDst = From.CalculateIsDst(); }
};

#endif
