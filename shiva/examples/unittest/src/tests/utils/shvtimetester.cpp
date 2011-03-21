#include "stdafx.h"
#include "shiva/include/platformspc.h"

#include "shvtimetester.h"

#include "shiva/include/utils/shvtime.h"


/*************************************
 * GetGroup
 *************************************/
const SHVString8C SHVTimeTester::GetGroup() const
{
	return "utils";
}

/*************************************
 * GetID
 *************************************/
const SHVString8C SHVTimeTester::GetID() const
{
	return "time";
}

/*************************************
 * GetTitle
 *************************************/
const SHVStringC SHVTimeTester::GetTitle() const
{
	return _S("SHVTime test");
}

/*************************************
 * GetActions
 *************************************/
const SHVTestBase::Action* SHVTimeTester::GetActions() const
{
static const SHVTestBase::Action actions[] = {
	{ ActionUTC, "utc", _S("UTC"), _S("This will test UTC"), &SHVTimeTester::TestUTC },
	{ ActionConversion, "conversion", _S("Conversion"), _S("This will test conversion"), &SHVTimeTester::TestConversion },
	{ ActionLocalTime, "localtime", _S("Localtime"), _S("This will test localtime"), &SHVTimeTester::TestLocalTime },
	{ 0, NULL, NULL, NULL, NULL } }; // Termination
	
	return actions;
}

/*************************************
 * sub tests
 *************************************/
bool SHVTimeTester::TestUTC(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok;

	ok = true;
	if (ok) // test adding time and comparison
	{
	SHVTime time, time2;
		time.SetNow();

		ok = (ok && (!time.IsNull()));
		time2 = time;
		time2.AddSeconds(SHVTime::DayInSeconds);
		ok = (ok && ( (time < time2) && (time <= time2) && !(time > time2) && !(time >= time2) && !(time == time2)));
		ok = (ok && ( (time.GetDay() < time2.GetDay()) || (time.GetMonth() < time2.GetMonth()) || (time.GetYear() < time2.GetYear()) ));
		time2.AddSeconds(-SHVTime::DayInSeconds);
		ok = (ok && ( !(time < time2) && (time <= time2) && !(time > time2) && (time >= time2) && (time == time2)));
		ok = (ok && ( (time.GetDay() == time2.GetDay()) && (time.GetMonth() == time2.GetMonth()) && (time.GetYear() == time2.GetYear()) ));
		time2.AddSeconds(-SHVTime::DayInSeconds);
		ok = (ok && ( !(time < time2) && !(time <= time2) && (time > time2) && (time >= time2) && !(time == time2)));
		ok = (ok && ( (time.GetDay() > time2.GetDay()) || (time.GetMonth() > time2.GetMonth()) || (time.GetYear() > time2.GetYear()) ));

		ok = (ok && ( (time.GetHour() == time2.GetHour()) && (time.GetMinute() == time2.GetMinute()) && (time.GetSecond() == time2.GetSecond()) ));

		time2.AddSeconds(SHVTime::DayInSeconds); // back to now
	}
	if (ok) // test adding time over dst change days
	{
	SHVTime time, time2;
		time.SetFromDateString(_S("2004-10-30T12:00"));
		ok = (ok && (!time.IsNull()));
		time2 = time;
		time2.AddSeconds(SHVTime::DayInSeconds);
		ok = (ok && ( (time.GetHour() == time2.GetHour()) && (time.GetMinute() == time2.GetMinute()) && (time.GetSecond() == time2.GetSecond()) &&
					  (time.GetYear() == time2.GetYear()) && (time.GetMonth() == time2.GetMonth()) && (time.GetDay()+1 == time2.GetDay()) ) );

		time.SetFromDateString(_S("2004-03-27T12:00:00"));
		ok = (ok && (!time.IsNull()));
		time2 = time;
		time2.AddSeconds(SHVTime::DayInSeconds);
		ok = (ok && ( (time.GetHour() == time2.GetHour()) && (time.GetMinute() == time2.GetMinute()) && (time.GetSecond() == time2.GetSecond()) &&
					  (time.GetYear() == time2.GetYear()) && (time.GetMonth() == time2.GetMonth()) && (time.GetDay()+1 == time2.GetDay()) ) );
	}

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	
	return ok;
}
bool SHVTimeTester::TestConversion(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok;

	ok = true;
	if (ok) // test conversion in dst and in standard time
	{
	SHVTime time, time2;
		time.SetFromDateString(_S("2004-06-06T12:00:00"));
		ok = (ok && (!time.IsNull()));
		time2 = time;
		time2.SetLocalTime(true);
		//log.AddLine(_S("%s"), time2.ToDateString().GetSafeBuffer());
		ok = (ok && ( (time.GetHour()+2 == time2.GetHour()) && (time.GetMinute() == time2.GetMinute()) && (time.GetSecond() == time2.GetSecond()) &&
					  (time.GetYear() == time2.GetYear()) && (time.GetMonth() == time2.GetMonth()) && (time.GetDay() == time2.GetDay()) ) );

		time.SetFromDateString(_S("2004-11-11T12:00:00"));
		ok = (ok && (!time.IsNull()));
		time2 = time;
		time2.SetLocalTime(true);
		//log.AddLine(_S("%s"), time2.ToDateString().GetSafeBuffer());
		ok = (ok && ( (time.GetHour()+1 == time2.GetHour()) && (time.GetMinute() == time2.GetMinute()) && (time.GetSecond() == time2.GetSecond()) &&
					  (time.GetYear() == time2.GetYear()) && (time.GetMonth() == time2.GetMonth()) && (time.GetDay() == time2.GetDay()) ) );
	}
	if (ok) // test conversion in dst and in standard time change days
	{
	SHVTime time, time2;
		time.SetFromDateString(_S("2004-10-30T12:00:00"));
		ok = (ok && (!time.IsNull()));
		time2 = time;
		time2.SetLocalTime(true);
		//log.AddLine(_S("%s"), time2.ToDateString().GetSafeBuffer());
		ok = (ok && ( (time.GetHour()+2 == time2.GetHour()) && (time.GetMinute() == time2.GetMinute()) && (time.GetSecond() == time2.GetSecond()) &&
					  (time.GetYear() == time2.GetYear()) && (time.GetMonth() == time2.GetMonth()) && (time.GetDay() == time2.GetDay()) ) );

		time.SetFromDateString(_S("2004-03-27T12:00:00"));
		ok = (ok && (!time.IsNull()));
		time2 = time;
		time2.SetLocalTime(true);
		//log.AddLine(_S("%s"), time2.ToDateString().GetSafeBuffer());
		ok = (ok && ( (time.GetHour()+1 == time2.GetHour()) && (time.GetMinute() == time2.GetMinute()) && (time.GetSecond() == time2.GetSecond()) &&
					  (time.GetYear() == time2.GetYear()) && (time.GetMonth() == time2.GetMonth()) && (time.GetDay() == time2.GetDay()) ) );
	}
	if (ok) // test conversion to and from utc over dst and standard change days
	{
	SHVTime time, time2;
		time.SetFromDateString(_S("2004-10-30T12:00:00"));
		ok = (ok && (!time.IsNull()));
		time2 = time;
		time2.SetLocalTime(true);
		time2.AddSeconds(SHVTime::DayInSeconds);
		time2.SetLocalTime(false);
		//log.AddLine(_S("%s"), time2.ToDateString().GetSafeBuffer());
		ok = (ok && ( (time.GetHour()+1 == time2.GetHour()) && (time.GetMinute() == time2.GetMinute()) && (time.GetSecond() == time2.GetSecond()) &&
					  (time.GetYear() == time2.GetYear()) && (time.GetMonth() == time2.GetMonth()) && (time.GetDay()+1 == time2.GetDay()) ) );

		time.SetFromDateString(_S("2004-03-27T12:00:00"));
		ok = (ok && (!time.IsNull()));
		time2 = time;
		time2.SetLocalTime(true);
		time2.AddSeconds(SHVTime::DayInSeconds);
		time2.SetLocalTime(false);
		//log.AddLine(_S("%s"), time2.ToDateString().GetSafeBuffer());
		ok = (ok && ( (time.GetHour()-1 == time2.GetHour()) && (time.GetMinute() == time2.GetMinute()) && (time.GetSecond() == time2.GetSecond()) &&
					  (time.GetYear() == time2.GetYear()) && (time.GetMonth() == time2.GetMonth()) && (time.GetDay()+1 == time2.GetDay()) ) );
	}

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	
	return ok;
}
bool SHVTimeTester::TestLocalTime(SHVModuleList& modules, SHVTestBase* self, int )
{
bool ok;

	ok = true;
	if (ok) // test adding time and comparison
	{ // test standard operations
	SHVTime time(true), time2(true);
		time.SetNow();

		ok = (ok && (!time.IsNull()));
		time2 = time;
		time2.AddSeconds(SHVTime::DayInSeconds);
		ok = (ok && ( (time < time2) && (time <= time2) && !(time > time2) && !(time >= time2) && !(time == time2)));
		ok = (ok && ( (time.GetDay() < time2.GetDay()) || (time.GetMonth() < time2.GetMonth()) || (time.GetYear() < time2.GetYear()) ));
		time2.AddSeconds(-SHVTime::DayInSeconds);
		ok = (ok && ( !(time < time2) && (time <= time2) && !(time > time2) && (time >= time2) && (time == time2)));
		ok = (ok && ( (time.GetDay() == time2.GetDay()) && (time.GetMonth() == time2.GetMonth()) && (time.GetYear() == time2.GetYear()) ));
		time2.AddSeconds(-SHVTime::DayInSeconds);
		ok = (ok && ( !(time < time2) && !(time <= time2) && (time > time2) && (time >= time2) && !(time == time2)));
		ok = (ok && ( (time.GetDay() > time2.GetDay()) || (time.GetMonth() > time2.GetMonth()) || (time.GetYear() > time2.GetYear()) ));

		ok = (ok && ( (time.GetHour() == time2.GetHour()) && (time.GetMinute() == time2.GetMinute()) && (time.GetSecond() == time2.GetSecond()) ));

		time2.AddSeconds(SHVTime::DayInSeconds); // back to now
	}
	if (ok) // test adding time over dst change days
	{
	SHVTime time(true), time2(true);
		time.SetFromDateString(_S("2004-10-30T12:00:00"));
		ok = (ok && (!time.IsNull() && time.IsLocalTime()));
		time2 = time;
		time2.AddSeconds(SHVTime::DayInSeconds);
		ok = (ok && ( (time.GetHour() == time2.GetHour()) && (time.GetMinute() == time2.GetMinute()) && (time.GetSecond() == time2.GetSecond()) &&
					  (time.GetYear() == time2.GetYear()) && (time.GetMonth() == time2.GetMonth()) && (time.GetDay()+1 == time2.GetDay()) ) );

		time.SetFromDateString(_S("2004-03-27T12:00:00"));
		ok = (ok && (!time.IsNull() && time.IsLocalTime()));
		time2 = time;
		time2.AddSeconds(SHVTime::DayInSeconds);
		ok = (ok && ( (time.GetHour() == time2.GetHour()) && (time.GetMinute() == time2.GetMinute()) && (time.GetSecond() == time2.GetSecond()) &&
					  (time.GetYear() == time2.GetYear()) && (time.GetMonth() == time2.GetMonth()) && (time.GetDay()+1 == time2.GetDay()) ) );
	}

	self->AddLine(_S("Test result: %s"), self->Success(modules,ok).GetSafeBuffer());
	
	return ok;
}
