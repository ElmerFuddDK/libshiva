/*
 *   Copyright (C) 2008 by Lars Eriksen
 *
 *
 *   This program is free software; you can redistribute it and/or modify
 *   it under the terms of the GNU Library General Public License as
 *   published by the Free Software Foundation; either version 2 of the
 *   License, or (at your option) any later version with the following
 *   exeptions:
 *
 *   1) Static linking to the library does not constitute derivative work
 *      and does not require the author to provide source code for the
 *      application.
 *      Compiling applications with the source code directly linked in is
 *      Considered static linking as well.
 *
 *   2) You do not have to provide a copy of the license with programs
 *      that are linked against this code.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU Library General Public
 *   License along with this program; if not, write to the
 *   Free Software Foundation, Inc.,
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 */

#include "stdafx.h"
#include "../../../include/platformspc.h"

#include "../../../include/utils/shvtime.h"

///\cond INTERNAL
#define __SHVTIME_MAXDATESTR 128
///\endcond


//=========================================================================================================
// SHVTime class
//=========================================================================================================

#ifdef __SHIVA_EPOC
# include <string.h>
#endif

/*************************************
 * Constructor
 *************************************/
SHVTime::SHVTime(bool localTime)
{
	::memset(&Time,0,sizeof(tm));
	Time.tm_isdst = Time.tm_wday = Time.tm_yday = -1;

	LocalTime = localTime;
}


// properties
/*************************************
 * IsLocalTime
 *************************************/
bool SHVTime::IsLocalTime() const
{
	return LocalTime;
}

/*************************************
 * SetlocalTime
 *************************************/
void SHVTime::SetLocalTime(bool local, bool convert)
{
	if (LocalTime == local)
		return;

	if (convert)
	{
	time_t ttime;

		if (local) // convert to local
		{
			ttime = TimeGm(&Time);
			LocalTime_r(&ttime,&Time);
		}
		else // convert to utc
		{
			ttime = MkTime(&Time);
			GmTime_r(&ttime,&Time);
		}
	}

	LocalTime = local;
}

/*************************************
 * IsNull
 *************************************/
bool SHVTime::IsNull() const
{
	return (Time.tm_year == 0);
}

/*************************************
 * SetNull
 *************************************/
void SHVTime::SetNull()
{
	Time.tm_year = 0;
}

/*************************************
 * Get*
 *************************************/
int SHVTime::GetYear() const
{
	return Time.tm_year+1900;
}
int SHVTime::GetMonth() const
{
	return Time.tm_mon+1;
}
int SHVTime::GetDay() const
{
	return Time.tm_mday;
}
int SHVTime::GetHour() const
{
	return Time.tm_hour;
}
int SHVTime::GetMinute() const
{
	return Time.tm_min;
}
int SHVTime::GetSecond() const
{
	return Time.tm_sec;
}

/*************************************
 * Set*
 *************************************/
void SHVTime::SetYear(int year)
{
	Time.tm_wday = -1; // weekday needs to be recalculated
	Time.tm_year = year-1900;
}
void SHVTime::SetMonth(int month)
{
	Time.tm_wday = -1; // weekday needs to be recalculated
	Time.tm_mon = month-1;
}
void SHVTime::SetDay(int day)
{
	Time.tm_wday = -1; // weekday needs to be recalculated
	Time.tm_mday = day;
}
void SHVTime::SetHour(int hour)
{
	Time.tm_wday = -1; // weekday needs to be recalculated
	Time.tm_hour = hour;
}
void SHVTime::SetMinute(int minute)
{
	Time.tm_wday = -1; // weekday needs to be recalculated
	Time.tm_min = minute;
}
void SHVTime::SetSecond(int second)
{
	Time.tm_wday = -1; // weekday needs to be recalculated
	Time.tm_sec = second;
}



// calculated properties
/*************************************
 * CalculateDayOfWeek
 *************************************/
int SHVTime::CalculateDayOfWeek() // 0 = sunday and so forth
{
	// test if the weekday needs to be calculated
	if (Time.tm_wday == -1)
	{
	time_t tmpTime = MkTime(&Time);

		GmTime_r(&tmpTime, &Time);

	}

	return Time.tm_wday;
}

/*************************************
 * CalculateLeapDayInEffect
 *************************************/
int SHVTime::CalculateLeapDayInEffect() // returns 1 if leap day is in effect at current date (02/29 excluded) or 0 if not
{
int month = GetMonth();
int year = GetYear();

	return ( month>2 && !((year-1968)%4) && ( (year%100) || !(year%400) ) ? 1 : 0 );
}

/*************************************
 * CalculateWeekNumber
 *************************************/
int SHVTime::CalculateWeekNumber() // ISO 8601
{
SHVTime firstDayOfYear(*this);
int weekDay;
int week = 0;
	
	if (IsNull())
		return -1;
	
	firstDayOfYear.SetDay(1);
	firstDayOfYear.SetMonth(1);
	
	// check for week 1 at the beginning
	weekDay = (firstDayOfYear.CalculateDayOfWeek()+6)%7;

	if (weekDay <= 3)
		week++;

	// add weeks
	week += (SHVTime::MonthToDays(GetMonth())+GetDay()+CalculateLeapDayInEffect()+6+weekDay)/7 - 1;

	// check for week 1 at the end
	if (GetMonth() == 12 && GetDay() >= 28)
	{
		if ( ((CalculateLeapDayInEffect()+6)%7) + 31-GetDay() < 3 )
			week = 1;
	}

	return ( week > 0 ? week : 53 );
}


// functions
/*************************************
 * SetFromDateString
 *************************************/
SHVBool SHVTime::SetFromDateString(const SHVStringC& dateStr)
{
SHVBool retVal(dateStr.GetLength() > 19 ? (int)SHVBool::True : (int)ErrInvalidString);

	if (retVal)
	{
		SetYear  ( dateStr.Mid( 0,4).ToLong() );
		SetMonth ( dateStr.Mid( 5,2).ToLong() );
		SetDay   ( dateStr.Mid( 8,2).ToLong() );
		SetHour  ( dateStr.Mid(11,2).ToLong() );
		SetMinute( dateStr.Mid(14,2).ToLong() );
		SetSecond( dateStr.Mid(17,2).ToLong() );

		if (IsNull())
			retVal.SetError(ErrInvalidString);
	}
	else
	{
		SetNull();
	}

	return retVal;
}

/*************************************
 * ToDateString
 *************************************/
SHVStringBuffer SHVTime::ToDateString() const
{
SHVString retVal;

	if (!IsNull())
	{
		retVal.Format(_T("%04d-%02d-%02dT%02d:%02d:%02d"), GetYear(), GetMonth(), GetDay(), GetHour(), GetMinute(), GetSecond());
	}

	return retVal.ReleaseBuffer();
}

/*************************************
 * Format
 *************************************/
SHVStringBuffer SHVTime::Format(const SHVStringC s) const
{
SHVTChar* retVal = (SHVTChar*)::malloc(__SHVTIME_MAXDATESTR*sizeof(SHVTChar));

	retVal[0] = 0;

#if defined(UNICODE)
	wcsftime(retVal,__SHVTIME_MAXDATESTR,s.GetSafeBuffer(),&Time);
#else
	strftime(retVal,__SHVTIME_MAXDATESTR,s.GetSafeBuffer(),&Time);
#endif

	return SHVStringBuffer::Encapsulate(retVal);
}

/*************************************
 * SetNow
 *************************************/
void SHVTime::SetNow(int diffInSeconds)
{
time_t now = TimeNow() + diffInSeconds;
	
	GmTime_r(&now,&Time);

	if (LocalTime)
	{
		LocalTime = false;
		SetLocalTime(true);
	}
}

/*************************************
 * AddSeconds
 *************************************/
void SHVTime::AddSeconds(int seconds)
{
time_t ttime;
	
	if (LocalTime)
	{
	int bias = 60*60;
	int daylightActive;
	tm tmpTime;

		ttime = MkTime(&Time);
		LocalTime_r(&ttime,&tmpTime);
		daylightActive = Time.tm_isdst;
		ttime += seconds;


		// convert to localtime
		LocalTime_r(&ttime,&Time);

		// test if we need to apply a bias (if wrapped around dst)
		if (Time.tm_isdst && !daylightActive)
			ttime -= bias;
		else if (!Time.tm_isdst && daylightActive)
			ttime += bias;
		else
			bias = 0;

		// if we applied a bias, recalculate the tm struct
		if (bias)
			LocalTime_r(&ttime,&Time);
	}
	else
	{
		ttime = TimeGm(&Time);
		ttime += seconds;
		GmTime_r(&ttime,&Time);
	}

}


// operators
/*************************************
 * add operator
 *************************************/
SHVTime operator+(const SHVTime& tTime, int diffInSeconds)
{
SHVTime retVal(tTime);
	retVal.AddSeconds(diffInSeconds);
	return retVal;
}

/*************************************
 * Comparison operators
 *************************************/
bool operator>(const SHVTime& tTime1, const SHVTime& tTime2)
{
time_t t1 = SHVTime::TimeGm((tm*)&tTime1.Time, false);
time_t t2 = SHVTime::TimeGm((tm*)&tTime2.Time, false);
	return (t1 > t2);
}
bool operator<(const SHVTime& tTime1, const SHVTime& tTime2)
{
time_t t1 = SHVTime::TimeGm((tm*)&tTime1.Time, false);
time_t t2 = SHVTime::TimeGm((tm*)&tTime2.Time, false);
	return (t1 < t2);
}
bool operator>=(const SHVTime& tTime1, const SHVTime& tTime2)
{
time_t t1 = SHVTime::TimeGm((tm*)&tTime1.Time, false);
time_t t2 = SHVTime::TimeGm((tm*)&tTime2.Time, false);
	return (t1 >= t2);
}
bool operator<=(const SHVTime& tTime1, const SHVTime& tTime2)
{
time_t t1 = SHVTime::TimeGm((tm*)&tTime1.Time, false);
time_t t2 = SHVTime::TimeGm((tm*)&tTime2.Time, false);
	return (t1 <= t2);
}
bool operator!=(const SHVTime& tTime1, const SHVTime& tTime2)
{
time_t t1 = SHVTime::TimeGm((tm*)&tTime1.Time, false);
time_t t2 = SHVTime::TimeGm((tm*)&tTime2.Time, false);
	return (t1 != t2);
}
bool operator==(const SHVTime& tTime1, const SHVTime& tTime2)
{
time_t t1 = SHVTime::TimeGm((tm*)&tTime1.Time, false);
time_t t2 = SHVTime::TimeGm((tm*)&tTime2.Time, false);
	return (t1 == t2);
}


// statics
/*************************************
 * CreateTimeStringNow
 *************************************/
SHVStringBuffer SHVTime::CreateTimeStringNow(int diffInSeconds)
{
SHVTime now;

	now.SetNow(diffInSeconds);

	return now.ToDateString();
}

/*************************************
 * MonthToDays
 *************************************/
int SHVTime::MonthToDays(int month)
{
static const int month_to_days[12] = {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};
	return month_to_days[month-1];
}

/*************************************
 * DaysInMonth
 *************************************/
int SHVTime::DaysInMonth(int month, int year)
{
	switch(month)
	{	case 4: case 6: case 9: case 11: return 30;
		case 2: return ( !(year%4) && ( (year%100) || !(year%400) ) ? 29 : 28 );
	}
	return 31;
}

/*************************************
 * GapInSeconds
 *************************************/
int SHVTime::GapInSeconds(const SHVTime& from, const SHVTime& to)
{
time_t ttime1;
time_t ttime2;

	///\todo Add handling for local time calculation

	ttime1 = TimeGm((tm*)&from.Time);
	ttime2 = TimeGm((tm*)&to.Time);

	return int(ttime2)-int(ttime1);
}



//=========================================================================================================
// non posix and non-linux functions
//=========================================================================================================

///\cond INTERNAL
time_t SHVTime::TimeGm(struct tm *t, bool setDst)
{
#ifndef __SHIVA_LINUX
time_t retVal;
short month;
short year;

	// calculate the month and year temp vars
	month = (short)t->tm_mon%12;
	year = (short)(t->tm_year + t->tm_mon / 12);
	if (month < 0)
	{
		year -= 1;
		month += 12;
	}


	// start calculation
	// -----------------

	// calculate days up to the current month
	retVal = (year - 70) * 365 + SHVTime::MonthToDays(month+1);

	// calculate leap days
	if (month <= 1) // ignore the first 2 months
		year -= 1;
	retVal += (year - 68) / 4;
	retVal -=  year / 100;
	retVal += (year +300) / 400;

	// add days in the current month
	retVal += t->tm_mday-1;

	// calculate hours
	retVal *= 24;
	retVal += t->tm_hour;

	// calculate minutes
	retVal *= 60;
	retVal += t->tm_min;

	// calculate seconds
	retVal *= 60;
	retVal += t->tm_sec;

	// all done
	return retVal;
#else
int isdst = t->tm_isdst;
int gmtoff = t->tm_gmtoff;
time_t retVal = ::timegm(t);
	if (!setDst)
	{
		t->tm_isdst = isdst;
		t->tm_gmtoff = gmtoff;
	}
	return retVal;
#endif
}

struct tm* SHVTime::LocalTime_r(const time_t *timep, struct tm *result)
{
#ifdef __SHIVA_WINCE
time_t t = *timep;
	GmTime_r(timep,result);
	SetDstAndWeekday(result,&t);
	GmTime_r(&t,result);
#elif defined(__SHIVA_EPOC)
	///TODO: Implement a proper localtime conversion for symbian, since it only knows utc
	*result = *localtime(timep);
#elif defined(__SHIVA_LINUX)
	result = localtime_r(timep,result);
#else
	*result = *localtime(timep);
#endif
	return result;
}

struct tm* SHVTime::GmTime_r(const time_t *timep, struct tm *result)
{
#if defined(__SHIVA_WINCE) || defined(__SHIVA_EPOC)
time_t t = *timep;
short year;

	result->tm_sec  = t%60;	t/=60;
	result->tm_min  = t%60;	t/=60;
	result->tm_hour = t%24;	t/=24;

	result->tm_year = t/365 + 70;

	// deduct leap days
	year = result->tm_year - 1; // ignore this year
	t -= (year - 68) / 4;
	t +=  year / 100;
	t -= (year +300) / 400;
	t%=365;

	for (result->tm_mon=0; t>DaysInMonth(result->tm_mon+1,result->tm_year+1900);)
		t-=DaysInMonth(++result->tm_mon,result->tm_year+1900);

	result->tm_mday = t+1;

# ifdef __SHIVA_WINCE
	SetDstAndWeekday(result);
# else
	mktime(result);
# endif

#elif defined(__SHIVA_LINUX)
	result = gmtime_r(timep,result);
#else
	*result = *gmtime(timep);
#endif
	return result;
}

time_t SHVTime::MkTime(struct tm *t)
{
#ifdef __SHIVA_EPOC
	///TODO: Implement a proper mktime conversion for symbian, since it only knows utc
	return mktime(t);
#elif defined(__SHIVA_WINCE)
time_t retVal = TimeGm(t);

	SetDstAndWeekday(t,&retVal,true);

	return retVal;
#else
	return mktime(t);
#endif
}

time_t SHVTime::TimeNow()
{
#ifdef __SHIVA_WINCE
SYSTEMTIME st;
tm t;
	GetSystemTime(&st);
	t.tm_year = st.wYear-1900;
	t.tm_mon  = st.wMonth-1;
	t.tm_mday = st.wDay;
	t.tm_hour = st.wHour;
	t.tm_min  = st.wMinute;
	t.tm_sec  = st.wSecond;

	return TimeGm(&t);
#elif defined(__SHIVA_EPOC)
TTime tt; tt.UniversalTime();
TDateTime dt(tt.DateTime());
tm t;
	t.tm_year = dt.Year()-1900;
	t.tm_mon  = dt.Month()-1;
	t.tm_mday = dt.Day();
	t.tm_hour = dt.Hour();
	t.tm_min  = dt.Minute();
	t.tm_sec  = dt.Second();

	return TimeGm(&t);
#else
	return time(NULL);
#endif
}

#ifdef __SHIVA_WINCE
void SHVTime::SetDstAndWeekday(struct tm* t, time_t* ttime, bool addingBias)
{
TIME_ZONE_INFORMATION TZ;
bool lessThanStandard, lessThanDaylight;

	SetWeekdayAndYearday(t);
	t->tm_isdst = 0;

	GetTimeZoneInformation(&TZ);

	t->tm_mon++; // for convenience

	lessThanStandard = ( (t->tm_mon <  TZ.StandardDate.wMonth) ||
						 (t->tm_mon == TZ.StandardDate.wMonth && !IsLastDOW(t,TZ.StandardDate)) );
	lessThanDaylight = ( (t->tm_mon <  TZ.DaylightDate.wMonth) ||
						 (t->tm_mon == TZ.DaylightDate.wMonth && !IsLastDOW(t,TZ.DaylightDate)) );

	t->tm_mon--; // for convenience

	if ( TZ.StandardDate.wMonth < TZ.DaylightDate.wMonth )
	{
		if ( !lessThanStandard && lessThanDaylight )
		{
			t->tm_isdst = 1;
			TZ.Bias += TZ.DaylightBias;
		}
	}
	else if ( lessThanStandard && !lessThanDaylight )
	{
		t->tm_isdst = 1;
		TZ.Bias += TZ.DaylightBias;
	}

	if (ttime)
	{
		if (addingBias)
			*ttime += TZ.Bias*60;
		else
			*ttime -= TZ.Bias*60;
	}
}
void SHVTime::SetWeekdayAndYearday(struct tm* t)
{
time_t days;
short month;
short year;

	// calculate the month and year temp vars
	month = t->tm_mon%12;
	year = t->tm_year + t->tm_mon / 12;
	if (month < 0)
	{
		year -= 1;
		month += 12;
	}

	// calculate year days
	// -------------------
	t->tm_yday = SHVTime::MonthToDays(month+1) + t->tm_mday - 1;
	if (month > 1 && ( !(year%4) && ( (year%100) || !((year+300)%400) ) ))
		t->tm_yday++;


	// calculate weekday
	// -----------------

	// calculate days up to the current month
	days = (year - 70) * 365 + SHVTime::MonthToDays(month+1);

	// calculate leap days
	if (month <= 1) // ignore the first 2 months
		year -= 1;
	days += (year - 68) / 4;
	days -=  year / 100;
	days += (year +300) / 400;

	// add days in the current month
	days += t->tm_mday-1;

	t->tm_wday = (days+4)%7;
}
bool SHVTime::IsLastDOW(struct tm* t, SYSTEMTIME& DOW)
{
WORD tmp;
int daysInMonth = DaysInMonth(t->tm_mon,t->tm_year);

	if (t->tm_mday + 7 <= daysInMonth) return false;

	tmp = t->tm_mday + ( t->tm_wday >= DOW.wDayOfWeek ? 
						DOW.wDayOfWeek + 7-t->tm_wday :
						DOW.wDayOfWeek -   t->tm_wday );
	if (tmp <= daysInMonth) return false;
	else if (DOW.wDayOfWeek == t->tm_wday) // compare time
	{
		return ( DOW.wHour < t->tm_hour || ( DOW.wHour == t->tm_hour && DOW.wMinute <= t->tm_min ) );
	}

	return true;
}
#endif
///\endcond
