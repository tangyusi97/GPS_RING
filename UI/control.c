#include "control.h"

#include "model.h"
#include "timer.h"

void KNOB_DetectDirection(void) {}

static void BEEP_On_Mode1(void) {}
static void BEEP_On_Mode2(void) {}
static void BEEP_On_Mode3(void) {}

static void (*beep_fn[3])(void) = {
    &BEEP_On_Mode1,
    &BEEP_On_Mode2,
    &BEEP_On_Mode3,
};

static inline void Change_Page(PAGE page) {
  current_page = page;
  current_mode = MODE_LIST;
}

static inline void Change_Mode(MODE mode) { current_mode = mode; }

static inline void Toggle_Mode_Value(void) {
  current_mode = (current_mode == MODE_VALUE ? MODE_LIST : MODE_VALUE);
}

static void Change_Select(MENU *offset, CONTROL_TYPE type) {
  if (type == UP && offset->select != 0) offset->select--;
  if (type == DOWN && offset->select < offset->len - 1) offset->select++;
}

static void Change_Value(uint8_t *value, CONTROL_TYPE type, uint8_t min, uint8_t max) {
  if (type == UP) *value >= max ? *value = min : (*value)++;
  if (type == DOWN) *value <= min ? *value = max : (*value)--;
}

static inline void Toggle_Value(uint8_t *value) { *value = !(*value); }

static void Toggle_Week(void) {
  uint8_t *repeat = &alarms[menu_alarm_list.select].repeat;
  uint8_t select = menu_week.select;
  if (*repeat & (1 << select))
    *repeat &= ~(1 << select);
  else
    *repeat |= 1 << select;
}

static void Handle_Operation(CONTROL_TYPE type) {
  switch (current_page) {
    case PAGE_WAIT:
      break;
    case PAGE_HOME:
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
      break;
    case PAGE_SETTING:
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
              break;
            default:
              Change_Mode(MODE_LIST);
              break;
          }
          break;
        case RIGHT:
          switch (menu_setting.select) {
            case 0:
              Change_Page(PAGE_ALARM_LIST);
              break;
            case 1:
            case 2:
              Toggle_Mode_Value();
            default:
              break;
          }
          break;
        default:
          break;
      }
      break;
    case PAGE_ALARM_LIST:
      switch (type) {
        case UP:
        case DOWN:
          Change_Select(&menu_alarm_list, type);
          break;
        case LEFT:
          Change_Page(PAGE_SETTING);
          break;
        case RIGHT:
          Change_Page(PAGE_ALARM_SETTING);
          break;
        default:
          break;
      }
      break;
    case PAGE_ALARM_SETTING:
      switch (type) {
        case UP:
        case DOWN:
          switch (current_mode) {
            case MODE_LIST:
              Change_Select(&menu_alarm_setting, type);
              break;
            case MODE_HOUR:
              Change_Value(&alarms[menu_alarm_list.select].hour, type, 0, 23);
              break;
            case MODE_MINUTE:
              Change_Value(&alarms[menu_alarm_list.select].minute, type, 0, 59);
              break;
            default:
              break;
          }
          break;
        case LEFT:
          switch (current_mode) {
            case MODE_LIST:
              Change_Page(PAGE_ALARM_LIST);
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
            case 0:
              Toggle_Value(&alarms[menu_alarm_list.select].enable);
              break;
            case 1:
              if (current_mode == MODE_LIST) Change_Mode(MODE_HOUR);
              else if (current_mode == MODE_HOUR) Change_Mode(MODE_MINUTE);
              else if (current_mode == MODE_MINUTE) Change_Mode(MODE_LIST);
              break;
            case 2:
              Change_Page(PAGE_WEEK);
            default:
              break;
          }
          break;
        default:
          break;
      }
      break;
    case PAGE_WEEK:
      switch (type) {
        case UP:
        case DOWN:
          Change_Select(&menu_week, type);
          break;
        case LEFT:
          Change_Page(PAGE_ALARM_SETTING);
          break;
        case RIGHT:
          Toggle_Week();
          break;
        default:
          break;
      }
      break;
    case PAGE_BEEP:
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
      break;
    default:
      break;
  }
  Update_View();
}

void BTN_Press(CONTROL_TYPE type) {
  static uint32_t interval = 0;
  if (G_ms_ticks - interval < JITTER_TIME) return;
  interval = G_ms_ticks;
  Handle_Operation(type);
}

void Refresh_Time(void) {
  if (current_page == PAGE_WAIT || current_page == PAGE_HOME) {
    if (G_local_time.year > 2050 || G_local_time.year < 2020)
      current_page = PAGE_WAIT;
    else
      current_page = PAGE_HOME;
    Update_View();
  }
}

void Control_Init(void) {
  Load_Data();
  Update_View();
}