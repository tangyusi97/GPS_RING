#include "view.h"

#include <stdio.h>

#include "model.h"
#include "ssd1306.h"
#include "timer.h"

static void Check_List_Offset(MENU* offset) {
  if (offset->len > MENU_MAX_NUM) {
    if (offset->select < offset->start) offset->start = offset->select;
    if (offset->select >= offset->start + MENU_MAX_NUM) offset->start = offset->select - MENU_MAX_NUM + 1;
  }
}

/*********************************** Render Items ***********************************/

static void Render_Item_Alarms_Name(void) { ssd1306_WriteString("Alarms", MENU_FONT, White); }

static void Render_Item_Contrast_Name(void) { ssd1306_WriteString("Contrast", MENU_FONT, White); }

static void Render_Item_Contrast_Value(uint8_t selected) {
  SSD1306_COLOR color = White;
  if (current_mode == MODE_VALUE && selected) color = Black;
  ssd1306_WriteNumber(contrast, 1, 0, MENU_FONT, color);
}

static void Render_Item_Duration_Name(void) { ssd1306_WriteString("Duration", MENU_FONT, White); }

static void Render_Item_Duration_Value(uint8_t selected) {
  SSD1306_COLOR color = White;
  if (current_mode == MODE_VALUE && selected) color = Black;
  ssd1306_WriteNumber(duration, 2, 0, MENU_FONT, color);
  ssd1306_WriteChar('s', MENU_FONT, color);
}

static void Render_Item_Alarm_Enable_Name(void) { ssd1306_WriteString("Enable", MENU_FONT, White); }

static void Render_Item_Alarm_Enable_Value(uint8_t selected) {
  SSD1306_COLOR color = White;
  if (current_mode == MODE_VALUE && selected) color = Black;
  ssd1306_WriteString(alarms[menu_alarm_list.select].enable ? "On" : "Off", MENU_FONT, color);
}

static void Render_Item_Alarm_Time_Name(void) { ssd1306_WriteString("Time", MENU_FONT, White); }

static void Render_Item_Alarm_Time_Value(uint8_t selected) {
  SSD1306_COLOR color_hour = White;
  SSD1306_COLOR color_minute = White;
  if (current_mode == MODE_HOUR && selected) color_hour = Black;
  if (current_mode == MODE_MINUTE && selected) color_minute = Black;
  ssd1306_WriteNumber(alarms[menu_alarm_list.select].hour, 2, 1, MENU_FONT, color_hour);
  ssd1306_WriteChar(':', MENU_FONT, White);
  ssd1306_WriteNumber(alarms[menu_alarm_list.select].minute, 2, 1, MENU_FONT, color_minute);
}

static void Render_Item_Alarm_Repeat_Name(void) { ssd1306_WriteString("Repeat", MENU_FONT, White); }

static void Render_Item_Alarm_Repeat_Value(uint8_t selected) {
  for (uint8_t i = 0; i < 7; i++) {
    if (alarms[menu_alarm_list.select].repeat & (1 << i)) {
      ssd1306_WriteNumber(i + 1, 1, 0, MENU_FONT, White);
    }
  }
}

static void Render_Item_Beep_Mode1_Name(void) { ssd1306_WriteString("MODE 1", MENU_FONT, White); }

static void Render_Item_Beep_Mode1_Value(uint8_t selected) {
  ssd1306_WriteString("______", MENU_FONT, White);
}

static void Render_Item_Beep_Mode2_Name(void) { ssd1306_WriteString("MODE 2", MENU_FONT, White); }

static void Render_Item_Beep_Mode2_Value(uint8_t selected) {
  ssd1306_WriteString("_-_-_-", MENU_FONT, White);
}

static void Render_Item_Beep_Mode3_Name(void) { ssd1306_WriteString("MODE 3", MENU_FONT, White); }

static void Render_Item_Beep_Mode3_Value(uint8_t selected) {
  ssd1306_WriteString("__-__-", MENU_FONT, White);
}

static void Render_Items_General(ITEM* items, uint8_t items_len, MENU* offset) {
  ssd1306_SetCursor(0, (offset->select - offset->start) * MENU_LINE_HEIGHT);
  ssd1306_WriteChar('>', MENU_FONT, White);
  for (uint8_t i = 0; i < MENU_MAX_NUM; i++) {
    if (i > items_len - 1) break;
    // render name
    if (items[offset->start + i].render_name != 0) {
      ssd1306_SetCursor(MENU_NAME_TAB, MENU_LINE_HEIGHT * i);
      (*items[offset->start + i].render_name)();
    }
    // render value
    if (items[offset->start + i].render_value != 0) {
      uint8_t selected = 0;
      if (offset->select == offset->start + i) selected = 1;
      ssd1306_SetCursor(MENU_VALUE_TAB, MENU_LINE_HEIGHT * i);
      (*items[offset->start + i].render_value)(selected);
    }
  }
}

/************************************************************************************/

/*********************************** Render Pages ***********************************/

static void Render_Page_Wait(void) {
  ssd1306_SetCursor(0, 0);
  ssd1306_WriteString("Getting start...", Font_7x10, White);
}

static void Render_Page_Home(void) {
  // 年月日
  ssd1306_SetCursor(8, 0);
  ssd1306_WriteNumber(G_local_time.year, 4, 0, Font_7x10, White);
  ssd1306_WriteChar('-', Font_7x10, White);
  ssd1306_WriteNumber(G_local_time.month, 2, 0, Font_7x10, White);
  ssd1306_WriteChar('-', Font_7x10, White);
  ssd1306_WriteNumber(G_local_time.date, 2, 0, Font_7x10, White);
  // 星期
  ssd1306_SetCursor(100, 0);
  char* Week[7] = {"MON", "TUE", "WED", "THU", "FRI", "SAT", "SUN"};
  ssd1306_WriteString(Week[G_local_time.week], Font_7x10, White);
  // 小时分钟
  ssd1306_SetCursor(14, 22);
  ssd1306_WriteNumber(G_local_time.hour, 2, 1, Font_16x26, White);
  ssd1306_WriteChar(':', Font_16x26, White);
  ssd1306_WriteNumber(G_local_time.minute, 2, 1, Font_16x26, White);
  // 秒
  ssd1306_SetCursor(98, 28);
  ssd1306_WriteNumber(G_local_time.second, 2, 1, Font_11x18, White);
  // 说明
  ssd1306_SetCursor(0, 56);
  ssd1306_WriteString("SET              BEEP", Font_6x8, White);
}

static void Render_Page_Setting(void) {
  Check_List_Offset(&menu_setting);
  ITEM items[3] = {
      &Render_Item_Alarms_Name,   0,                            //
      &Render_Item_Contrast_Name, &Render_Item_Contrast_Value,  //
      &Render_Item_Duration_Name, &Render_Item_Duration_Value   //
  };
  Render_Items_General(items, 3, &menu_setting);
}

static void Render_Page_Alarm_List(void) {
  Check_List_Offset(&menu_alarm_list);
  ssd1306_SetCursor(0, (menu_alarm_list.select - menu_alarm_list.start) * MENU_LINE_HEIGHT);
  ssd1306_WriteChar('>', MENU_FONT, White);
  for (uint8_t i = 0; i < MENU_MAX_NUM; i++) {
    if (i + menu_alarm_list.start > Model_GetAlarmsLen() - 1) break;
    // render name
    ssd1306_SetCursor(MENU_NAME_TAB, MENU_LINE_HEIGHT * i);
    if (alarms[i + menu_alarm_list.start].hour > 23) {
      ssd1306_WriteChar('#', MENU_FONT, White);
      ssd1306_WriteNumber(i + menu_alarm_list.start + 1, 2, 0, MENU_FONT, White);
    } else {
      ssd1306_WriteNumber(alarms[i + menu_alarm_list.start].hour, 2, 1, MENU_FONT, White);
      ssd1306_WriteChar(':', MENU_FONT, White);
      ssd1306_WriteNumber(alarms[i + menu_alarm_list.start].minute, 2, 1, MENU_FONT, White);
      ssd1306_WriteChar(' ', MENU_FONT, White);
      ssd1306_WriteString(alarms[i + menu_alarm_list.start].enable ? "On" : "Off", MENU_FONT, White);
      // render value
      ssd1306_SetCursor(MENU_VALUE_TAB, MENU_LINE_HEIGHT * i);
      for (uint8_t j = 0; j < 7; j++) {
        if (alarms[i + menu_alarm_list.start].repeat & (1 << j)) {
          ssd1306_WriteNumber(j + 1, 1, 0, MENU_FONT, White);
        }
      }
    }
  }
}

static void Render_Page_Alarm_Setting(void) {
  Check_List_Offset(&menu_alarm_setting);
  ITEM items[3] = {
      &Render_Item_Alarm_Enable_Name, &Render_Item_Alarm_Enable_Value,  //
      &Render_Item_Alarm_Time_Name,   &Render_Item_Alarm_Time_Value,    //
      &Render_Item_Alarm_Repeat_Name, &Render_Item_Alarm_Repeat_Value   //
  };
  Render_Items_General(items, 3, &menu_alarm_setting);
}

static void Render_Page_Week(void) {
  Check_List_Offset(&menu_week);
  ssd1306_SetCursor(0, (menu_week.select - menu_week.start) * MENU_LINE_HEIGHT);
  ssd1306_WriteChar('>', MENU_FONT, White);
  for (uint8_t i = 0; i < MENU_MAX_NUM; i++) {
    if (i + menu_week.start > 7) break;
    // render name
    ssd1306_SetCursor(MENU_NAME_TAB, MENU_LINE_HEIGHT * i);
    char weeks[7][10] = {"Monday", "Tuesday", "Wednesday", "Thursday", "Friday", "Saturday", "Sunday"};
    ssd1306_WriteString(weeks[i + menu_week.start], MENU_FONT, White);
    // render value
    ssd1306_SetCursor(MENU_VALUE_TAB, MENU_LINE_HEIGHT * i);
    if (alarms[menu_alarm_list.select].repeat & (1 << (i + menu_week.start)))
      ssd1306_WriteString("On", MENU_FONT, White);
    else
      ssd1306_WriteString("Off", MENU_FONT, White);
  }
}

static void Render_Page_Beep(void) {
  Check_List_Offset(&menu_beep);
  ITEM items[3] = {
      &Render_Item_Beep_Mode1_Name, &Render_Item_Beep_Mode1_Value,  //
      &Render_Item_Beep_Mode2_Name, &Render_Item_Beep_Mode2_Value,  //
      &Render_Item_Beep_Mode3_Name, &Render_Item_Beep_Mode3_Value,  //
  };
  Render_Items_General(items, 3, &menu_beep);
}

/************************************************************************************/

void Render(void) {
  ssd1306_Fill(Black);
  switch (current_page) {
    case PAGE_WAIT:
      Render_Page_Wait();
      break;
    case PAGE_HOME:
      Render_Page_Home();
      break;
    case PAGE_SETTING:
      Render_Page_Setting();
      break;
    case PAGE_ALARM_LIST:
      Render_Page_Alarm_List();
      break;
    case PAGE_ALARM_SETTING:
      Render_Page_Alarm_Setting();
      break;
    case PAGE_WEEK:
      Render_Page_Week();
      break;
    case PAGE_BEEP:
      Render_Page_Beep();
      break;
    default:
      break;
  }
  ssd1306_UpdateScreen();
}