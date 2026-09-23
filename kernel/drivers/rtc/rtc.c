#include "rtc.h"
#include <stdio.h>

// El cmos devuelve con el siguiente formato (0001,0010) para indicar por ejemplo el 12

int rtcYear(){
    outB(CMOS_RTC_INFO_OUT, RTC_YEAR);
    int in = inB(CMOS_RTC_INFO_IN);
    return ((in >> 4) * 10) + (in & 0x0F);
}

int rtcMonth(){
    outB(CMOS_RTC_INFO_OUT, RTC_MONTH);
    int in = inB(CMOS_RTC_INFO_IN);
    return ((in >> 4) * 10) + (in & 0x0F);
}

int rtcDay(){
    outB(CMOS_RTC_INFO_OUT, RTC_DAY);
    int in = inB(CMOS_RTC_INFO_IN);  
    return ((in >> 4) * 10) + (in & 0x0F);
}

int rtcHour(){
    outB(CMOS_RTC_INFO_OUT, RTC_HOUR);
    int in = inB(CMOS_RTC_INFO_IN);
    return ((in >> 4) * 10) + (in & 0x0F);
}

int rtcMinute(){
    outB(CMOS_RTC_INFO_OUT, RTC_MINUTE);
    int in = inB(CMOS_RTC_INFO_IN);
    return ((in >> 4) * 10) + (in & 0x0F);
}

int rtcSecon(){
    outB(CMOS_RTC_INFO_OUT, RTC_SECON);
    int in = inB(CMOS_RTC_INFO_IN);
    return ((in >> 4) * 10) + (in & 0x0F);
}