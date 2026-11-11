#ifndef RTC_H
#define RTC_H

// Headers extra
#include "../io.h"
#include <stdint.h>

#define CMOS_RTC_INFO_OUT 0x70
#define CMOS_RTC_INFO_IN 0x71

#define RTC_YEAR 0x09
#define RTC_MONTH 0x08
#define RTC_DAY 0x07
#define RTC_HOUR 0x04
#define RTC_MINUTE 0x02
#define RTC_SECON 0x00


typedef struct rtcDate{
    uint16 rtcYear;
    uint8 rtcMonth;
    uint8 rtcDay;
    uint8 rtcHour;
    uint8 rtcMinute;
    uint8 rtcSecond;
} rtcDate;


// devuelve el año actual 
uint16 rtcYear();

// devuelve el mes actual
uint8 rtcMonth();

// devuelve el dia actual
uint8 rtcDay();

// devuelve la hora actual
uint8 rtcHour();

// devuelve el minuto actual
uint8 rtcMinute();

// devuelve el segundo actual
uint8 rtcSecond();

#endif
