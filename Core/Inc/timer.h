#ifndef __TIMER_H
#define __TIMER_H

#include <stdint.h>

// UTC时间信息
typedef struct {
  uint16_t year;    //年份
  uint8_t month;    //月份
  uint8_t date;     //日期
  uint8_t hour;     //小时
  uint8_t minute;   //分钟
  uint8_t second;   //秒钟
  uint8_t week;     //星期
} TIME;

extern uint32_t G_ms_ticks;
extern TIME G_local_time;

void TIMER_Init(void);
void TIMER_Get_Msg(uint8_t byte);

#endif
