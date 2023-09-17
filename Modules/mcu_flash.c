/**
 * @file mcu_flash.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-09-11
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "mcu_flash.h"

void static LL_FLASH_Program_TwoBtye(uint16_t flash_addr, uint16_t data)
{
    LL_FLASH_EnableProgram(FLASH);
    *(__IO uint16_t *)(flash_addr) = data;
}

ErrorStatus LL_FLASH_Unlock(void)
{
    while (LL_FLASH_IsActiveFlag_BSY(FLASH)) {
    }
    if (LL_FLASH_LockState(FLASH)) {
        LL_FLASh_SetKey(FLASH, FLASH_KEY1);
        LL_FLASh_SetKey(FLASH, FLASH_KEY2);
    }
    return SUCCESS;
}

ErrorStatus LL_FLASH_PageErase(uint32_t page_addr, uint16_t pagenum)
{
    uint32_t End_addr   = pagenum * FLASH_PAGE_SIZE + page_addr;
    uint32_t Start_addr = page_addr;
    for (; Start_addr < End_addr; (Start_addr += FLASH_PAGE_SIZE)) {
        LL_FLASH_SetTypeErase(FLASH, FLASH_TYPEERASE_PAGES);
        LL_FLASH_SetEraseADDR(FLASH, Start_addr);
        LL_FLASH_StartErase(FLASH);
        while (LL_FLASH_IsActiveFlag_BSY(FLASH)) {
        }
        if (LL_FLASH_IsActiveFlag_EOP(FLASH)) {
            LL_FLASH_ClearFlag_EOP(FLASH);
            ;
        } else {
            return ERROR;
        }
        LL_FLASH_DisenableErase(FLASH, FLASH_TYPEERASE_PAGES);
    }
    return SUCCESS;
}

ErrorStatus LL_FLASH_Program(ProgaramDataType ProgramType, uint32_t flash_addr, uint64_t data)
{

    uint8_t index        = 0U;
    uint8_t nbiterations = 0U;

    if (ProgramType == ProgaraType_DATA16)
        nbiterations = 1U;
    else if (ProgramType == ProgaraType_DATA32)
        nbiterations = 2U;
    else
        nbiterations = 4U;
    for (index = 0U; index < nbiterations; index++) {
        LL_FLASH_Program_TwoBtye((flash_addr + (2U * index)), (uint16_t)(data >> (16U * index)));
    }

    while (LL_FLASH_IsActiveFlag_BSY(FLASH)) {
    }
    if (LL_FLASH_IsActiveFlag_EOP(FLASH)) {
        LL_FLASH_ClearFlag_EOP(FLASH);
    } else {
        return ERROR;
    }
    LL_FLASH_DisenableProgram(FLASH);
    return SUCCESS;
}
