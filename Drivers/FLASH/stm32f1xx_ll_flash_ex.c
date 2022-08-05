#include "stm32f1xx_ll_flash_ex.h"

static LL_StatusTypeDef Wait_Operation_Done(void) {
  LL_FLASH_IsActiveFlag_BSY(FLASH);
  while (LL_FLASH_IsActiveFlag_BSY(FLASH)) {
  }
  if (!LL_FLASH_IsActiveFlag_EOP(FLASH)) {
    return LL_ERROR;
  }
  LL_FLASH_ClearFlag_EOP(FLASH);
  return LL_OK;
}

LL_StatusTypeDef LL_FLASH_Unlock(void) {
  while (LL_FLASH_IsActiveFlag_BSY(FLASH)) {
  }
  if (LL_FLASH_GetLockState(FLASH)) {
    LL_FLASH_SetKey(FLASH, FLASH_KEY1);
    LL_FLASH_SetKey(FLASH, FLASH_KEY2);
  }
  return LL_OK;
}

LL_StatusTypeDef LL_FLASH_PageErase(uint32_t page_addr, uint16_t size) {
  uint32_t End_addr = size * FLASH_PAGE_SIZE + page_addr;
  uint32_t Start_addr = page_addr;
  LL_FLASH_SetEraseType(FLASH, FLASH_ERASETYPE_PAGES);
  for (; Start_addr < End_addr; (Start_addr += FLASH_PAGE_SIZE)) {
    LL_FLASH_SetEraseADDR(FLASH, Start_addr);
    LL_FLASH_StartErase(FLASH);
    LL_StatusTypeDef res = Wait_Operation_Done();
    if (res != LL_OK) return res;
  }
  LL_FLASH_ClearEraseType(FLASH, FLASH_ERASETYPE_PAGES);
  return LL_OK;
}

LL_StatusTypeDef LL_FLASH_Program(uint32_t flash_addr, uint16_t data) {
  LL_FLASH_EnableProgram(FLASH);
  *(__IO uint16_t*)(flash_addr) = data;
  LL_StatusTypeDef res = Wait_Operation_Done();
  if (res != LL_OK) return res;
  LL_FLASH_DisableProgram(FLASH);
  return LL_OK;
}
