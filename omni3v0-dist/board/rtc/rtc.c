/*
 * Generic RTC interface
 * Partially ported from Linux Kernel
 * Inquiring RTC hardware status is not implemented.
 */

#include "rtc.h"
//Put RTC hardware header filek here
#include "RTC_WDT.h"

//Inquiring RTC hardware status
//used by local functions
static uint8_t rtcok;

//Put hardware specific function here:
uint8_t enable_rtc(void){
	WDT_1sec_wake();
	return 1;
}

uint8_t disable_rtc(void){
	WDT_stop();
	return 1;
}

static const uint8_t days_in_month[] = {
	31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31
};

#define LEAPS_THRU_END_OF(y) ((y)/4 - (y)/100 + (y)/400)
#define LEAP_YEAR(year) ((!(year % 4) && (year % 100)) || !(year % 400))


void incrementSeconds(void)
{
	n_seconds++;
}

static int32_t month_days(uint32_t month, uint32_t year)
{
	return days_in_month[month] + (LEAP_YEAR(year) && month == 1);
}

/* Converts Gregorian date to seconds since 1970-01-01 00:00:00.
 * Assumes input in normal date format, i.e. 1980-12-31 23:59:59
 * => year=1980, mon=12, day=31, hour=23, min=59, sec=59.
 *
 * [For the Julian calendar (which was used in Russia before 1917,
 * Britain & colonies before 1752, anywhere else before 1582,
 * and is still in use by some communities) leave out the
 * -year/100+year/400 terms, and add 10.]
 *
 * This algorithm was first published by Gauss (I think).
 *
 * WARNING: this function will overflow on 2106-02-07 06:28:16 on
 * machines were long is 32-bit! (However, as time_t is signed, we
 * will already get problems at other places on 2038-01-19 03:14:08)
 */
uint32_t
mktime(const uint32_t year0, const uint32_t mon0,
       const uint32_t day, const uint32_t hour,
       const uint32_t min, const uint32_t sec)
{
	uint32_t mon = mon0;
	uint32_t year = year0;

	/* 1..12 -> 11,12,1..10 */
	if (0 >= (int32_t) (mon -= 2)) {
		mon += 12;	/* Puts Feb last since it has leap day */
		year -= 1;
	}

	return ((((uint32_t)
		  (year/4 - year/100 + year/400 + 367*mon/12 + day) +
		  year*365 - 719499
	    )*24 + hour /* now have hours */
	  )*60 + min /* now have minutes */
	)*60 + sec; /* finally seconds */
}

int8_t rtc_tm_to_time(struct rtc_time *tm, uint32_t *time)
{
	*time = mktime(tm->tm_year + 1900, tm->tm_mon + 1, tm->tm_mday,
		       tm->tm_hour, tm->tm_min, tm->tm_sec);

	return 0;
}

/*
 * Convert seconds since 01-01-1970 00:00:00 to Gregorian date.
 */
void rtc_time_to_tm(uint32_t time, struct rtc_time *tm)
{
	int32_t days, month;
	int32_t year;

	days = time / 86400;
	time -= days * 86400;

	tm->tm_wday = (days + 4) % 7;

	year = 1970 + days / 365;
	days -= (year - 1970) * 365
	        + LEAPS_THRU_END_OF(year - 1)
	        - LEAPS_THRU_END_OF(1970 - 1);
	if (days < 0) {
		year -= 1;
		days += 365 + LEAP_YEAR(year);
	}
	tm->tm_year = year - 1900;
	tm->tm_yday = days + 1;

	for (month = 0; month < 11; month++) {
		int newdays;

		newdays = days - month_days(month, year);
		if (newdays < 0)
			break;
		days = newdays;
	}
	tm->tm_mon = month;
	tm->tm_mday = days + 1;

	tm->tm_hour = time / 3600;
	time -= tm->tm_hour * 3600;
	tm->tm_min = time / 60;
	tm->tm_sec = time - tm->tm_min * 60;
}

/*------------------------------------------*/
/* Initialize RTC                           */
/*1:RTC is available, 0:RTC is not available*/
/*------------------------------------------*/

uint8_t rtc_initialize (void)	/*  */
{
	//enable RTC hardware
	if (enable_rtc()) {
		rtcok = 1;		/* RTC is available */
	} else {
		rtcok = 0;		/* RTC is not available */
	}

	return rtcok;
}

/*------------------------------------------*/
/* Set time in UTC                          */
/*------------------------------------------*/

uint8_t rtc_setutc (uint32_t tmr)
{
	disable_rtc();
	n_seconds = tmr;
	enable_rtc();

	return 1;
}


/*------------------------------------------*/
/* Get time in UTC                          */
/*------------------------------------------*/

uint8_t rtc_getutc (uint32_t* tmr)
{

	if (rtcok) {
		/* Read RTC counter */
		*tmr = n_seconds;
		return 1;
	}
	return 0;
}



/*------------------------------------------*/
/* Get time in Gregorian form                */
/*------------------------------------------*/

uint8_t rtc_gettime (RTCTIME* rtc)
{
	uint32_t utc;

	if (!rtc_getutc(&utc)) return 0;

	utc += (uint32_t)(_RTC_TDIF * 3600);

	rtc_time_to_tm(utc, rtc);

	return 1;
}



/*------------------------------------------*/
/* Set time in Gregorian form                */
/*------------------------------------------*/

uint8_t rtc_settime (RTCTIME* rtc)
{
	uint32_t utc;

	rtc_tm_to_time(rtc, &utc);

	utc -= (uint32_t)(_RTC_TDIF * 3600);

	return rtc_setutc(utc);
}

