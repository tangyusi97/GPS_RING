/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/

#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_spi.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_gpio.h"

#if defined(USE_FULL_ASSERT)
#include "stm32_assert.h"
#endif /* USE_FULL_ASSERT */

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin LL_GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define GPS_TX_Pin LL_GPIO_PIN_2
#define GPS_TX_GPIO_Port GPIOA
#define GPS_RX_Pin LL_GPIO_PIN_3
#define GPS_RX_GPIO_Port GPIOA
#define OLED_CS_Pin LL_GPIO_PIN_4
#define OLED_CS_GPIO_Port GPIOA
#define OLED_CLK_Pin LL_GPIO_PIN_5
#define OLED_CLK_GPIO_Port GPIOA
#define OLED_MOSI_Pin LL_GPIO_PIN_7
#define OLED_MOSI_GPIO_Port GPIOA
#define OLED_Res_Pin LL_GPIO_PIN_0
#define OLED_Res_GPIO_Port GPIOB
#define OLED_DC_Pin LL_GPIO_PIN_1
#define OLED_DC_GPIO_Port GPIOB
#define BTN_UP_Pin LL_GPIO_PIN_12
#define BTN_UP_GPIO_Port GPIOB
#define BTN_UP_EXTI_IRQn EXTI15_10_IRQn
#define BTN_DOWN_Pin LL_GPIO_PIN_13
#define BTN_DOWN_GPIO_Port GPIOB
#define BTN_DOWN_EXTI_IRQn EXTI15_10_IRQn
#define BTN_LEFT_Pin LL_GPIO_PIN_14
#define BTN_LEFT_GPIO_Port GPIOB
#define BTN_LEFT_EXTI_IRQn EXTI15_10_IRQn
#define BTN_RIGHT_Pin LL_GPIO_PIN_15
#define BTN_RIGHT_GPIO_Port GPIOB
#define BTN_RIGHT_EXTI_IRQn EXTI15_10_IRQn
#define BEEP_Pin LL_GPIO_PIN_3
#define BEEP_GPIO_Port GPIOB
#ifndef NVIC_PRIORITYGROUP_0
#define NVIC_PRIORITYGROUP_0         ((uint32_t)0x00000007) /*!< 0 bit  for pre-emption priority,
                                                                 4 bits for subpriority */
#define NVIC_PRIORITYGROUP_1         ((uint32_t)0x00000006) /*!< 1 bit  for pre-emption priority,
                                                                 3 bits for subpriority */
#define NVIC_PRIORITYGROUP_2         ((uint32_t)0x00000005) /*!< 2 bits for pre-emption priority,
                                                                 2 bits for subpriority */
#define NVIC_PRIORITYGROUP_3         ((uint32_t)0x00000004) /*!< 3 bits for pre-emption priority,
                                                                 1 bit  for subpriority */
#define NVIC_PRIORITYGROUP_4         ((uint32_t)0x00000003) /*!< 4 bits for pre-emption priority,
                                                                 0 bit  for subpriority */
#endif
/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
