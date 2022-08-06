#include "model.h"

#include "ssd1306.h"
#include "stm32f1xx_ll_flash_ex.h"
#include "view.h"

Alarm alarms[] = {
    1, 6,  30, 0x1F,  // Alarm1
    0, 7,  0,  0x7F,  // Alarm2
    0, 24, 0,  0x7F,  // Alarm3
    0, 24, 0,  0x7F,  // Alarm4
    0, 24, 0,  0x7F,  // Alarm5
    0, 24, 0,  0x7F,  // Alarm6
    0, 24, 0,  0x7F,  // Alarm7
    0, 24, 0,  0x7F,  // Alarm8
    0, 24, 0,  0x7F,  // Alarm9
    0, 24, 0,  0x7F,  // Alarm10
    0, 24, 0,  0x7F,  // Alarm11
    0, 24, 0,  0x7F,  // Alarm12
    0, 24, 0,  0x7F,  // Alarm13
    0, 24, 0,  0x7F,  // Alarm14
    0, 24, 0,  0x7F,  // Alarm15
    0, 24, 0,  0x7F,  // Alarm16
    0, 24, 0,  0x7F,  // Alarm17
    0, 24, 0,  0x7F,  // Alarm18
    0, 24, 0,  0x7F,  // Alarm19
    0, 24, 0,  0x7F,  // Alarm20
};

uint8_t contrast = 3;   // 屏幕亮度设置
uint8_t duration = 15;  // 打铃时长
uint8_t saved = 1;      // 是否已保存设置

PAGE current_page = PAGE_WAIT;
MODE current_mode = MODE_LIST;
uint8_t *current_value = 0;

MENU menu_setting = {0, 0, 4};
MENU menu_alarm_list = {0, 0, sizeof(alarms) / 4};
MENU menu_alarm_setting = {0, 0, 3};
MENU menu_week = {0, 0, 7};
MENU menu_beep = {0, 0, 3};

uint8_t Model_GetAlarmsLen(void) { return sizeof(alarms) / 4; }

void Update_View(void) {
  static uint8_t _contrast = 0;
  if (contrast != _contrast) {
    _contrast = contrast;
    ssd1306_SetContrast((contrast - 1) * 50);
  }
  Render();
}

void Load_Data(void) {
  uint16_t data;
  for (uint8_t i = 0; i < Model_GetAlarmsLen() * 4; i = i + 4) {
    data = LL_FLASH_Read(SAVE_DATA_ADDR + i);
    alarms[i / 4].enable = (uint8_t)data;
    alarms[i / 4].hour = (uint8_t)(data >> 8);
    data = LL_FLASH_Read(SAVE_DATA_ADDR + i + 2);
    alarms[i / 4].minute = (uint8_t)data;
    alarms[i / 4].repeat = (uint8_t)(data >> 8);
  }
  data = LL_FLASH_Read(SAVE_DATA_ADDR + Model_GetAlarmsLen() * 4);
  contrast = (uint8_t)data;
  duration = (uint8_t)(data >> 8);
}

void Save_Data(void) {
  if (saved) return;
  LL_FLASH_Unlock();
  LL_FLASH_PageErase(SAVE_DATA_ADDR, 1);
  uint16_t data;
  for (uint8_t i = 0; i < Model_GetAlarmsLen() * 4; i = i + 4) {
    data = alarms[i / 4].enable + (alarms[i / 4].hour << 8);
    LL_FLASH_Program(SAVE_DATA_ADDR + i, data);
    data = alarms[i / 4].minute + (alarms[i / 4].repeat << 8);
    LL_FLASH_Program(SAVE_DATA_ADDR + i + 2, data);
  }
  data = contrast + (duration << 8);
  LL_FLASH_Program(SAVE_DATA_ADDR + Model_GetAlarmsLen() * 4, data);
  LL_FLASH_Lock(FLASH);
  saved = 1;
}
