#ifndef __MODEL_H__
#define __MODEL_H__

#include <stdint.h>

#define SAVE_DATA_ADDR 0x8007C00  // FLASH模拟EEPROM地址

typedef struct {
  uint8_t enable;
  uint8_t hour;
  uint8_t minute;
  uint8_t repeat;     // bit[0:7]为星期重复，bit[8]为只响一次标记
} Alarm;

typedef struct {
  uint8_t start;      // 从第几项开始显示
  uint8_t select;     // 选中的项
  const uint8_t len;  // 列表长度
} MENU;

typedef enum {
  PAGE_WAIT,
  PAGE_HOME,
  PAGE_SETTING,
  PAGE_ALARM_LIST,
  PAGE_ALARM_SETTING,
  PAGE_WEEK,
  PAGE_BEEP,
} PAGE;

typedef enum {
  MODE_LIST,    // 列表模式
  MODE_VALUE,   // 修改设置值
  MODE_HOUR,    // 修改闹钟小时
  MODE_MINUTE,  // 修改闹钟分钟
} MODE;

extern Alarm alarms[20];        // 闹铃数据
extern uint8_t contrast;        // 屏幕亮度
extern uint8_t duration;        // 响铃时长
extern uint8_t saved;           // 是否已保存设置
extern PAGE current_page;       // 当前页面
extern MODE current_mode;       // 当前渲染模式
extern uint8_t *current_value;  // 正在修改的数据

// 用于保存菜单光标位置
extern MENU menu_setting;
extern MENU menu_alarm_list;
extern MENU menu_alarm_setting;
extern MENU menu_week;
extern MENU menu_beep;

uint8_t Model_GetAlarmsLen(void);  // 获得闹钟组数
void Update_View(void);             // 数据更新到屏幕显示
void Load_Data(void);               // 读取FLASH数据
void Save_Data(void);               // 保存数据到FLASH
#endif
