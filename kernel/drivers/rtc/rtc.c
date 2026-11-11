#include "rtc.h"
#include <stdio.h>

// El cmos devuelve con el siguiente formato (0001,0010) para indicar por ejemplo el 12

uint16 rtcYear(){
    outB(CMOS_RTC_INFO_OUT, RTC_YEAR);
    int in = inB(CMOS_RTC_INFO_IN);
    return ((in >> 4) * 10) + (in & 0x0F);
}

uint8 rtcMonth(){
    outB(CMOS_RTC_INFO_OUT, RTC_MONTH);
    int in = inB(CMOS_RTC_INFO_IN);
    return ((in >> 4) * 10) + (in & 0x0F);
}

uint8 rtcDay(){
    outB(CMOS_RTC_INFO_OUT, RTC_DAY);
    int in = inB(CMOS_RTC_INFO_IN);  
    return ((in >> 4) * 10) + (in & 0x0F);
}

uint8 rtcHour(){
    outB(CMOS_RTC_INFO_OUT, RTC_HOUR);
    int in = inB(CMOS_RTC_INFO_IN);
    return ((in >> 4) * 10) + (in & 0x0F);
}

uint8 rtcMinute(){
    outB(CMOS_RTC_INFO_OUT, RTC_MINUTE);
    int in = inB(CMOS_RTC_INFO_IN);
    return ((in >> 4) * 10) + (in & 0x0F);
}

uint8 rtcSecond(){
    outB(CMOS_RTC_INFO_OUT, RTC_SECON);
    int in = inB(CMOS_RTC_INFO_IN);
    return ((in >> 4) * 10) + (in & 0x0F);
}

rtcDate rtcNow(){
    rtcDate newDate;

    newDate.rtcYear = rtcYear();
    newDate.rtcMonth = rtcMonth();
    newDate.rtcDay = rtcDay();
    newDate.rtcHour = rtcHour();
    newDate.rtcMinute = rtcMinute();
    newDate.rtcSecond = rtcSecond();

    return newDate;
}