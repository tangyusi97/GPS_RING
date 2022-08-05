#ifndef __STM32F1xx_LL_FLASH_EX_H
#define __STM32F1xx_LL_FLASH_EX_H

#include "stm32f1xx.h"

#define FLASH_ERASETYPE_PAGES          FLASH_CR_PER        /*!< FLASH_CR_PER */
#define FLASH_ERASETYPE_MASSERASE      FLASH_CR_MER        /*!< MASSERASE */


#if defined(STM32F103x6) || defined(STM32F103xB)
#define FLASH_PAGE_SIZE          0x400U
#elif defined(STM32F103xE)
#define FLASH_PAGE_SIZE          0x800U
#error Please add your device
#endif

typedef enum {
	FLASH_Lock = 1U,
  Flash_Unlock = !FLASH_Lock
} FlashStates;

typedef enum {
	LL_OK=0U,
	LL_ERROR=1U,
	LL_BUSY = 2U,
	LL_TIMEOUT=3U
} LL_StatusTypeDef;

__STATIC_INLINE void LL_FLASH_Lock(FLASH_TypeDef *FLASHx) { SET_BIT(FLASHx->CR, FLASH_CR_LOCK); }

/**
 * @param FLASH_ERASETYPE This parameter can be any combination of the following values:
 *        @arg @ref FLASH_ERASETYPE_PAGES
 *        @arg @ref FLASH_ERASETYPE_MASSERASE
 */
__STATIC_INLINE void LL_FLASH_SetEraseType(FLASH_TypeDef *FLASHx, uint32_t FLASH_ERASETYPE) {
  SET_BIT(FLASHx->CR, FLASH_ERASETYPE);
}

__STATIC_INLINE void LL_FLASH_ClearEraseType(FLASH_TypeDef *FLASHx, uint32_t FLASH_ERASETYPE) {
  CLEAR_BIT(FLASHx->CR, FLASH_ERASETYPE);
}

__STATIC_INLINE void LL_FLASH_SetEraseADDR(FLASH_TypeDef *FLASHx, uint32_t EraseADDR) {
  WRITE_REG(FLASHx->AR, EraseADDR);
}

__STATIC_INLINE void LL_FLASH_StartErase(FLASH_TypeDef *FLASHx) { SET_BIT(FLASHx->CR, FLASH_CR_STRT); }

__STATIC_INLINE uint32_t LL_FLASH_IsActiveFlag_BSY(FLASH_TypeDef *FLASHx) {
  return (READ_BIT(FLASHx->SR, FLASH_SR_BSY) == (FLASH_SR_BSY));
}

__STATIC_INLINE uint32_t LL_FLASH_IsActiveFlag_EOP(FLASH_TypeDef *FLASHx) {
  return (READ_BIT(FLASHx->SR, FLASH_SR_EOP) == (FLASH_SR_EOP));
}

__STATIC_INLINE void LL_FLASH_ClearFlag_EOP(FLASH_TypeDef *FLASHx) {
  SET_BIT(FLASHx->SR, FLASH_SR_EOP);  // EOP bit Set clear
}

__STATIC_INLINE void LL_FLASH_EnableProgram(FLASH_TypeDef *FLASHx) { SET_BIT(FLASHx->CR, FLASH_CR_PG); }

__STATIC_INLINE void LL_FLASH_DisableProgram(FLASH_TypeDef *FLASHx) { CLEAR_BIT(FLASHx->CR, FLASH_CR_PG); }

__STATIC_INLINE FlashStates LL_FLASH_GetLockState(FLASH_TypeDef *FLASHx) {
  return (FlashStates)(READ_BIT(FLASHx->CR, FLASH_CR_LOCK));
}

__STATIC_INLINE void LL_FLASH_SetKey(FLASH_TypeDef *FLASHx, uint32_t key) { WRITE_REG(FLASH->KEYR, key); }

__STATIC_INLINE uint16_t LL_FLASH_Read(uint32_t address) { return *(__IO uint16_t *)address; }

LL_StatusTypeDef LL_FLASH_Unlock(void);
LL_StatusTypeDef LL_FLASH_PageErase(uint32_t page_addr, uint16_t NbPages);
LL_StatusTypeDef LL_FLASH_Program(uint32_t flash_addr, uint16_t data);

#endif
