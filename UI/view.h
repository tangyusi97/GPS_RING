#ifndef __VIEW_H__
#define __VIEW_H__

#include <stdint.h>

#define MENU_FONT Font_7x10
#define MENU_MAX_NUM 5       // 一次显示的菜单数
#define MENU_NAME_TAB 9      // 菜单项名字制表位
#define MENU_VALUE_TAB 77    // 菜单项值制表位
#define MENU_LINE_HEIGHT 13  // 行高

typedef struct {
  void (*render_name)(void);
  void (*render_value)(uint8_t selected);
} ITEM;

void Render(void);

#endif
