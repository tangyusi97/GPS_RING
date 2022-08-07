#ifndef __CONTROL_H__
#define __CONTROL_H__

#include "main.h"

#define JITTER_TIME 10  // 消抖时间，ms

__STATIC_INLINE void LED_On(void) {
  LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin);
}
__STATIC_INLINE void LED_Off(void) {
  LL_GPIO_SetOutputPin(LED_GPIO_Port, LED_Pin);
}
__STATIC_INLINE void LED_Toggle(void) {
  LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
}

__STATIC_INLINE void BEEP_On(void) {
  LL_GPIO_SetOutputPin(BEEP_GPIO_Port, BEEP_Pin);
}
__STATIC_INLINE void BEEP_Off(void) {
  LL_GPIO_ResetOutputPin(BEEP_GPIO_Port, BEEP_Pin);
}
__STATIC_INLINE void BEEP_Toggle(void) {
  LL_GPIO_TogglePin(BEEP_GPIO_Port, BEEP_Pin);
}

typedef enum {
  UP, DOWN, LEFT, RIGHT
} CONTROL_TYPE;

void Handle_Operation(CONTROL_TYPE type);
void Refresh_Time(void);
void Control_Init(void);
void Loop_Task_1s(void);
void Loop_Task_1ms(void);

extern void (*delay_task)(void);
extern uint8_t delay_task_interupt;

#endif  /* __CONTROL_H__ */
