#include "control.h"

#include "model.h"
#include "timer.h"

uint32_t operation_ticks = 0;

void Control_Init(void) {
  Load_Data();
  Update_View();
}

/******************************************************************************/
/*                               Hardware Control                             */
/******************************************************************************/

static void BEEP_On_Mode1(void) {}
static void BEEP_On_Mode2(void) {}
static void BEEP_On_Mode3(void) {}

static void (*beep_fn[3])(void) = {
    &BEEP_On_Mode1,
    &BEEP_On_Mode2,
    &BEEP_On_Mode3,
};

static void Excute_Alarms(void) {
  uint8_t flag = 0;
  if (current_page == PAGE_HOME) {
    for (uint8_t i = 0; i < Model_GetAlarmsLen(); i++) {
      // 时间进入响铃范围
      if (alarms[i].enable &&                         //
          alarms[i].hour == G_local_time.hour &&      //
          alarms[i].minute == G_local_time.minute &&  //
          duration > G_local_time.second) {
        // 如果没有设置重复，则清除只响一次标记
        if ((alarms[i].repeat & 0x7F) == 0) {
          flag = 1;
          alarms[i].repeat &= ~(1 << 7);
        }
        // 设置了重复正常响铃
        else if (alarms[i].repeat & (1 << G_local_time.week))
          flag = 1;
      }
      // 关闭只响一次的闹钟
      else if (alarms[i].repeat == 0) {
        alarms[i].enable = 0;
      }
    }
  }
  if (flag)
    BEEP_On();
  else
    BEEP_Off();
}

/******************************************************************************/
/*                            Data Control Funtions                           */
/******************************************************************************/

static inline void Change_Page(PAGE page) {
  current_page = page;
  current_mode = MODE_LIST;
}

static inline void Change_Mode(MODE mode) { current_mode = mode; }

static inline void Toggle_Mode(void) { current_mode = (current_mode == MODE_VALUE ? MODE_LIST : MODE_VALUE); }

static void Change_Select(MENU *offset, CONTROL_TYPE type) {
  if (type == UP && offset->select != 0) offset->select--;
  if (type == DOWN && offset->select < offset->len - 1) offset->select++;
}

static void Change_Value(uint8_t *value, CONTROL_TYPE type, uint8_t min, uint8_t max) {
  if (type == UP) *value >= max ? *value = min : (*value)++;
  if (type == DOWN) *value <= min ? *value = max : (*value)--;
  saved = 0;
}

static inline void Enable_Alarm(void) {
  alarms[menu_alarm_list.select].enable = 1;
  alarms[menu_alarm_list.select].repeat |= 1 << 7;
}

static void Toggle_Alarm_Enable(void) {
  if (alarms[menu_alarm_list.select].enable)
    alarms[menu_alarm_list.select].enable = 0;
  else
    Enable_Alarm();
  saved = 0;
}

static void Toggle_Week(void) {
  uint8_t *repeat = &alarms[menu_alarm_list.select].repeat;
  uint8_t select = menu_week.select;
  if (*repeat & (1 << select))
    *repeat &= ~(1 << select);
  else
    *repeat |= 1 << select;
  saved = 0;
}

static inline void Reset_Menu(MENU *menu) { menu->start = menu->select = 0; }

/******************************************************************************/
/*                              Operation Handler                             */
/******************************************************************************/

static inline void Handle_Operation_HOME(CONTROL_TYPE type) {
  switch (type) {
    case LEFT:
      Change_Page(PAGE_SETTING);
      break;
    case RIGHT:
      Change_Page(PAGE_BEEP);
      break;
    default:
      break;
  }
}

static inline void Handle_Operation_SETTING(CONTROL_TYPE type) {
  switch (type) {
    case UP:
    case DOWN:
      switch (current_mode) {
        case MODE_LIST:
          Change_Select(&menu_setting, type);
          break;
        case MODE_VALUE:
          switch (menu_setting.select) {
            case 1:  // 设置亮度
              Change_Value(&contrast, type, 1, 5);
              break;
            case 2:  // 设置响铃时长
              Change_Value(&duration, type, 1, 60);
              break;
            default:
              break;
          }
          break;
        default:
          break;
      }
      break;
    case LEFT:
      switch (current_mode) {
        case MODE_LIST:
          Change_Page(PAGE_HOME);
          Reset_Menu(&menu_setting);
          break;
        default:
          Change_Mode(MODE_LIST);
          break;
      }
      break;
    case RIGHT:
      switch (menu_setting.select) {
        case 0:  // 闹铃列表
          Change_Page(PAGE_ALARM_LIST);
          break;
        case 1:  // 亮度调整
        case 2:  // 响铃时长
          Toggle_Mode();
          break;
        case 3:  // 保存设置
          Save_Data();
          break;
        default:
          break;
      }
      break;
    default:
      break;
  }
}

static inline void Handle_Operation_ALARM_LIST(CONTROL_TYPE type) {
  switch (type) {
    case UP:
    case DOWN:
      Change_Select(&menu_alarm_list, type);
      break;
    case LEFT:
      Change_Page(PAGE_SETTING);
      Reset_Menu(&menu_alarm_list);
      break;
    case RIGHT:
      Change_Page(PAGE_ALARM_SETTING);
      break;
    default:
      break;
  }
}

static inline void Handle_Operation_ALARM_SETTING(CONTROL_TYPE type) {
  switch (type) {
    case UP:
    case DOWN:
      switch (current_mode) {
        case MODE_LIST:
          Change_Select(&menu_alarm_setting, type);
          break;
        case MODE_HOUR:
          Change_Value(&alarms[menu_alarm_list.select].hour, type, 0, 23);
          Enable_Alarm();
          break;
        case MODE_MINUTE:
          Change_Value(&alarms[menu_alarm_list.select].minute, type, 0, 59);
          Enable_Alarm();
          break;
        default:
          break;
      }
      break;
    case LEFT:
      switch (current_mode) {
        case MODE_LIST:
          Change_Page(PAGE_ALARM_LIST);
          Reset_Menu(&menu_alarm_setting);
          break;
        case MODE_VALUE:
        case MODE_HOUR:
        case MODE_MINUTE:
          Change_Mode(MODE_LIST);
          break;
        default:
          break;
      }
      break;
    case RIGHT:
      switch (menu_alarm_setting.select) {
        case 0:  // 设置闹铃开关
          Toggle_Alarm_Enable();
          break;
        case 1:  // 设置时间
          if (current_mode == MODE_LIST)
            Change_Mode(MODE_HOUR);
          else if (current_mode == MODE_HOUR) {
            if (alarms[menu_alarm_list.select].hour == 24)
              Change_Mode(MODE_LIST);
            else
              Change_Mode(MODE_MINUTE);
          } else if (current_mode == MODE_MINUTE)
            Change_Mode(MODE_LIST);
          break;
        case 2:  // 设置重复
          Change_Page(PAGE_WEEK);
        default:
          break;
      }
      break;
    default:
      break;
  }
}

static inline void Handle_Operation_WEEK(CONTROL_TYPE type) {
  switch (type) {
    case UP:
    case DOWN:
      Change_Select(&menu_week, type);
      break;
    case LEFT:
      Change_Page(PAGE_ALARM_SETTING);
      Reset_Menu(&menu_week);
      break;
    case RIGHT:
      Toggle_Week();
      break;
    default:
      break;
  }
}

static inline void Handle_Operation_BEEP(CONTROL_TYPE type) {
  switch (type) {
    case UP:
    case DOWN:
      Change_Select(&menu_beep, type);
      break;
    case LEFT:
      Change_Page(PAGE_HOME);
      break;
    case RIGHT:
      beep_fn[menu_beep.select]();
      break;
    default:
      break;
  }
}

void Handle_Operation(CONTROL_TYPE type) {
  // 消抖
  static uint32_t interval = 0;
  if (G_ms_ticks - interval < JITTER_TIME) return;
  interval = operation_ticks = G_ms_ticks;
  // 响应
  switch (current_page) {
    case PAGE_WAIT:
      break;
    case PAGE_HOME:
      Handle_Operation_HOME(type);
      break;
    case PAGE_SETTING:
      Handle_Operation_SETTING(type);
      break;
    case PAGE_ALARM_LIST:
      Handle_Operation_ALARM_LIST(type);
      break;
    case PAGE_ALARM_SETTING:
      Handle_Operation_ALARM_SETTING(type);
      break;
    case PAGE_WEEK:
      Handle_Operation_WEEK(type);
      break;
    case PAGE_BEEP:
      Handle_Operation_BEEP(type);
      break;
    default:
      break;
  }
  Update_View();
}

/******************************************************************************/
/*                                 UI Control                                 */
/******************************************************************************/

void Refresh_Time(void) {
  if (current_page == PAGE_WAIT || current_page == PAGE_HOME) {
    if (G_local_time.year > 2050 || G_local_time.year < 2020)
      current_page = PAGE_WAIT;
    else
      current_page = PAGE_HOME;
    Update_View();
  }
}

void Auto_Back_to_Home(void) {
  if (G_ms_ticks - operation_ticks > 15000) {
    current_page = PAGE_HOME;
    Update_View();
    Reset_Menu(&menu_setting);
    Reset_Menu(&menu_alarm_list);
    Reset_Menu(&menu_alarm_setting);
    Reset_Menu(&menu_week);
    Reset_Menu(&menu_beep);
  }
}

/******************************************************************************/
/*                                 Loop Task                                  */
/******************************************************************************/

void Loop_Task_1s(void) {
  Refresh_Time();
  Auto_Back_to_Home();
  Excute_Alarms();
}

void Loop_Task_1ms(void) {}