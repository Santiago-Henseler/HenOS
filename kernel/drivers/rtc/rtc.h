#ifndef RTC_H
#define RTC_H

// Headers extra
#include "../io.h"

#define CMOS_RTC_INFO_OUT 0x70
#define CMOS_RTC_INFO_IN 0x71

#define RTC_YEAR 0x09
#define RTC_MONTH 0x08
#define RTC_DAY 0x07
#define RTC_HOUR 0x04
#define RTC_MINUTE 0x02
#define RTC_SECON 0x00

int rtcYear();

int rtcMonth();

int rtcDay();

int rtcHour();

int rtcMinute();

int rtcSecon();

#endif
