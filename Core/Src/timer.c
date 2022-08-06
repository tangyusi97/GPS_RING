#include "timer.h"

#include <string.h>

#include "control.h"
#include "usart.h"

uint32_t G_ms_ticks;
TIME G_local_time;

uint8_t GPS_ready = 0;

void TIMER_Init(void) {
  while (GPS_ready == 0) {
    LL_mDelay(1);
  }
  uint8_t turn_off_msg[] = "$CCRMO,,3,*4F\r\n";
  uint8_t turn_on_RMC_msg[] = "$CCRMO,RMC,2,1*23\r\n";
  USART_Send(USART2, turn_off_msg, sizeof(turn_off_msg));
  USART_Send(USART2, turn_on_RMC_msg, sizeof(turn_on_RMC_msg));
}

/**
 * @brief 查找第n个逗号位置（地址偏移）
 *
 * @param buffer 字符串buffer
 * @param n 第n个
 * @return uint8_t
 */
static uint8_t Comma_Pos(uint8_t *buffer, uint8_t n) {
  uint8_t *p = buffer;
  while (n) {
    if (*buffer == '*' || *buffer < ' ' || *buffer > 'z')
      return 0XFF;  //遇到'*'或者非法字符,则不存在第n个逗号
    if (*buffer == ',') n--;
    buffer++;
  }
  return buffer - p;
}

// m^n函数
//返回值:m^n次方.
static uint32_t Pow(uint8_t m, uint8_t n) {
  uint32_t result = 1;
  while (n--) result *= m;
  return result;
}

// str转换为数字,以','或者'*'结束
// buffer:数字存储区
// dx:小数点位数,返回给调用函数
// 返回值:转换后的数值
static int Str2num(uint8_t *buffer, uint8_t *dx) {
  uint8_t *p = buffer;
  uint32_t ires = 0, fres = 0;
  uint8_t ilen = 0, flen = 0, i;
  uint8_t mask = 0;
  int res;
  while (1)  //得到整数和小数的长度
  {
    if (*p == '-') {
      mask |= 0x02;
      p++;
    }                                     //是负数
    if (*p == ',' || (*p == '*')) break;  //遇到结束了
    if (*p == '.') {
      mask |= 0x01;
      p++;
    }                                 //遇到小数点了
    else if (*p > '9' || (*p < '0'))  //有非法字符
    {
      ilen = 0;
      flen = 0;
      break;
    }
    if (mask & 0x01)
      flen++;
    else
      ilen++;
    p++;
  }
  if (mask & 0x02) buffer++;  //去掉负号
  for (i = 0; i < ilen; i++)  //得到整数部分数据
  {
    ires += Pow(10, ilen - 1 - i) * (buffer[i] - '0');
  }
  if (flen > 5) flen = 5;     //最多取5位小数
  *dx = flen;                 //小数点位数
  for (i = 0; i < flen; i++)  //得到小数部分数据
  {
    fres += Pow(10, flen - 1 - i) * (buffer[ilen + 1 + i] - '0');
  }
  res = ires * Pow(10, flen) + fres;
  if (mask & 0X02) res = -res;
  return res;
}

/**
 * @brief UTC时间转本地时间
 */
static void UTC_to_Local_Time(TIME *utc_time, int8_t timezone, TIME *local_time) {
  int year, month, day, hour;
  int month_day_num = 0;       // 本月天数
  int last_month_day_mun = 0;  // 上个月天数

  year = utc_time->year;  // utc time
  month = utc_time->month;
  day = utc_time->date;
  hour = utc_time->hour + timezone;

  // 1月大，2月小，3月大，4月小，5月大，6月小，7月大，8月大，9月小，10月大，11月小，12月大
  if (month == 1 || month == 3 || month == 5 || month == 7 || month == 8 || month == 10 || month == 12) {
    month_day_num = 31;
    last_month_day_mun = 30;

    if (month == 3) {                                               // 3月的上个月只有28/29天
      if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))  // 闰年
        last_month_day_mun = 29;
      else
        last_month_day_mun = 28;
    }

    if (month == 8 || month == 1)  //这里应该是8月和1月，因为8月和1月的上一个月（7月和12月）的天数是31天的
      last_month_day_mun = 31;
  } else if (month == 4 || month == 6 || month == 9 || month == 11) {
    month_day_num = 30;
    last_month_day_mun = 31;
  } else {
    last_month_day_mun = 31;

    if ((year % 400 == 0) || (year % 4 == 0 && year % 100 != 0))
      month_day_num = 29;
    else
      month_day_num = 28;
  }

  if (hour >= 24)  // if >24, day+1
  {
    hour -= 24;
    day += 1;
    if (day > month_day_num)  // next month, day-lastday of this month
    {
      day -= month_day_num;
      month += 1;
      if (month > 12)  // next year, month-12
      {
        month -= 12;
        year += 1;
      }
    }
  }

  if (hour < 0)  // if <0, day-1
  {
    hour += 24;
    day -= 1;
    if (day < 1)  // month-1, day=last day of last month
    {
      day = last_month_day_mun;
      month -= 1;
      if (month < 1)  // last year, month=12
      {
        month = 12;
        year -= 1;
      }
    }
  }

  // transfer value to local_time
  local_time->year = year;
  local_time->month = month;
  local_time->date = day;
  local_time->hour = hour;
  local_time->minute = utc_time->minute;
  local_time->second = utc_time->second;
}

/**
 * @brief 通过RMC消息取得北京时间
 *
 * @param buffer 消息字符串
 * @param local_time 传入时间结构体
 */
static void GPRMC_Analysis(uint8_t *buffer, TIME *local_time) {
  uint8_t *p1, dx;
  uint8_t posx;
  uint32_t temp;
  TIME utc_time;
  p1 = (uint8_t *)strstr((const char *)buffer, "RMC");  //"$GPRMC"
  posx = Comma_Pos(p1, 1);                              //得到UTC时间
  if (posx != 0xFF) {
    temp = Str2num(p1 + posx, &dx) / Pow(10, dx);  //得到UTC时间,去掉ms

    utc_time.hour = temp / 10000;
    utc_time.minute = (temp / 100) % 100;
    utc_time.second = temp % 100;
  }
  posx = Comma_Pos(p1, 9);  //得到UTC日期
  if (posx != 0xFF) {
    temp = Str2num(p1 + posx, &dx);  //得到UTC日期
    utc_time.date = temp / 10000;
    utc_time.month = (temp / 100) % 100;
    utc_time.year = 2000 + temp % 100;
  }

  UTC_to_Local_Time(&utc_time, 8, local_time);

  // 获取星期
  uint8_t d = local_time->date;
  uint8_t m = local_time->month;
  uint16_t y = local_time->year;
  if (m == 1 || m == 2) {
    m += 12;
    y--;
  }
  local_time->week = (d + 2 * m + 3 * (m + 1) / 5 + y + y / 4 - y / 100 + y / 400) % 7;
}

void TIMER_Get_Msg(uint8_t byte) {
  GPS_ready = 1;
  static uint8_t msg[100];
  static uint8_t size = 0;
  if (byte == 0x0A && msg[size - 1] == 0x0D) {
    // 接收完成
    GPRMC_Analysis(msg, &G_local_time);
    Loop_Task_1s();
    size = 0;
  }
  size++;
  if (size > 100) size = 1;  // 超出范围，重新接收
  msg[size - 1] = byte;
}