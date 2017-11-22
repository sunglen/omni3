#ifndef _RTC_DEFINED
#define _RTC_DEFINED

#include <stdint.h>

//set timezone here
//#define _RTC_TDIF	9	/* JST = UTC+9.0 */
#define _RTC_TDIF	8	/* CST = UTC+8.0 */

//seconds since 1970-1-1
volatile uint32_t n_seconds;

/* this is a similar copy of struct tm in <time.h> */
/*
typedef struct rtc_time {
	int tm_sec;
	int tm_min;
	int tm_hour;
	int tm_mday;
	int tm_mon;
	int tm_year;
	int tm_wday;
	int tm_yday;
	uint8_t tm_isdst;
} RTCTIME;
*/

//Gregorian date format
typedef struct rtc_time {
	int32_t tm_sec;    //second [0-59], or [0-61] 最大2秒までの閏（うるう）秒を考慮
	int32_t tm_min;    //minute [0-59]
	int32_t tm_hour;   //hour [0-23]
	int32_t tm_mday;   //day of month [1-31]
	int32_t tm_mon;    //month [0-11], ATTENTION: START FROM 0!
	int32_t tm_year;   //year [number of years start from 1900]
	int32_t tm_wday;   //day of week [0-6], 0 is Sunday, 1 is Monday
	int32_t tm_yday;   //day of year [0-365], ATTENTION: START FROM 0!
	uint8_t tm_isdst;  //Summer Time Flag, set 0 if not used.
} RTCTIME;

void incrementSeconds(void);
uint8_t rtc_initialize (void);				/* Initialize RTC */
uint8_t rtc_gettime (RTCTIME* rtc);			/* Get time */
uint8_t rtc_settime (RTCTIME* rtc);	        /* Set time */
uint8_t rtc_getutc (uint32_t* utc);			/* Get time in UTC */
uint8_t rtc_setutc (uint32_t utc);			/* Set time in UTC */

#endif
