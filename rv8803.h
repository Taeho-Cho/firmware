#ifndef _RV_8803_H_
#define _RV_8803_H_

#include "main.h"

#ifdef _RV_8803_C_
    #define RV_8803_EXT
#else 
    #define RV_8803_EXT extern
#endif

#define DEV_ID_RTC8803                  0x64

#define RV8803_SECONDS                  0x00 
#define RV8803_MINUTES                  0x01
#define RV8803_HOURS                    0x02
#define RV8803_WEEKDAY                  0x03
#define RV8803_DATE                     0x04
#define RV8803_MONTH                    0x05
#define RV8803_YEAR                     0x06

#define TIMER_COUNTER_0                 0x0B
#define TIMER_COUNTER_1                 0x0C
#define RV8803_EXT                      0x0D
#define RV8803_FLAG                     0x0E
#define RV8803_CTRL                     0x0F


typedef struct{
  uint8_t seconds;
  uint8_t minutes;
  uint8_t hours;
  uint8_t weekday;
  uint8_t date;    
  uint8_t month;
  uint8_t year;
} sRTC;

RV_8803_EXT void rtc_set_time();

RV_8803_EXT void rtc_read_time();

RV_8803_EXT void rtc_timer_init();

#endif