/**
 * @file mcu_flash.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-09-11
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __MCU_FLASH_H
#define __MCU_FLASH_H

#include "stm32f1xx.h"

#define FLASH_FLAG_BSY            FLASH_SR_BSY    /*!< FLASH Busy flag                           */
#define FLASH_FLAG_PGERR          FLASH_SR_PGERR  /*!< FLASH Programming error flag    */
#define FLASH_FLAG_WRPERR         FLASH_SR_WRPERR /*!< FLASH Write protected error flag          */
#define FLASH_FLAG_EOP            FLASH_SR_EOP    /*!< FLASH End of Operation flag               */

#define FLASH_TYPEERASE_PAGES     FLASH_CR_PER /*!< FLASH_CR_PER          */
#define FLASH_TYPEERASE_MASSERASE FLASH_CR_MER /*!< MASSERASE              */

/*!< for stm32f103rc(high-density devices)  */
#define FLASH_PAGE_SIZE    (0x800U)
#define FLASH_START_ADRESS ((uint32_t)0x08000000)
#define FLASH_SIZE         (256 * 1024)
#define FLASH_END_ADDRESS  ((uint32_t)(STM32_FLASH_START_ADRESS + STM32_FLASH_SIZE))

typedef enum {
    ProgaraType_DATA64,
    ProgaraType_DATA32,
    ProgaraType_DATA16
} ProgaramDataType;

typedef enum {
    FLASH_Lock   = 1U,
    Flash_Unlock = !FLASH_Lock
} FlashStates;

__STATIC_INLINE void LL_FLASH_Lock(FLASH_TypeDef *FLASHx)
{
    SET_BIT(FLASHx->CR, FLASH_CR_LOCK);
}

/* @brief  Set flash erase type.
 * @param  FLASH_TYPEERASE specifies the FLASH flags to clear.
 *          This parameter can be any combination of the following values:
 *            @arg @ref FLASH_TYPEERASE_PAGES         PAGES Erase
 *            @arg @ref FLASH_TYPEERASE_MASSERASE      FLASH Write protected error flag
 * @retval none*/

__STATIC_INLINE void LL_FLASH_SetTypeErase(FLASH_TypeDef *FLASHx, uint32_t FLASH_TYPEERASE)
{
    SET_BIT(FLASHx->CR, FLASH_TYPEERASE);
}
/* @brief  Set flash erase ADDR.
 *          This parameter can be any combination of the following values:
 *            @arg @ref EraseADDR         uint32_t value
 * @retval none*/

__STATIC_INLINE void LL_FLASH_SetEraseADDR(FLASH_TypeDef *FLASHx, uint32_t EraseADDR)
{
    WRITE_REG(FLASHx->AR, EraseADDR);
}
/* @brief  Set flash erase ADDR.
 *          This parameter can be any combination of the following values:
 *            @arg @ref EraseADDR         uint32_t value
 * @retval none*/

__STATIC_INLINE void LL_FLASH_StartErase(FLASH_TypeDef *FLASHx)
{
    SET_BIT(FLASHx->CR, FLASH_CR_STRT);
}

/* @brief  Clear the specified FLASH flag.
 * @param  __FLAG__ specifies the FLASH flags to clear.
 *          This parameter can be any combination of the following values:
 *            @arg @ref FLASH_FLAG_EOP         FLASH End of Operation flag
 *            @arg @ref FLASH_FLAG_WRPERR      FLASH Write protected error flag
 *            @arg @ref FLASH_FLAG_PGERR       FLASH Programming error flag
 * @retval none*/

__STATIC_INLINE void LL_FLASH_ClearFlag(FLASH_TypeDef *FLASHx, uint32_t STATE_FLAG)
{
    WRITE_REG(FLASHx->SR, STATE_FLAG);
}

/*get bit flash bsy*/
__STATIC_INLINE uint32_t LL_FLASH_IsActiveFlag_BSY(FLASH_TypeDef *FLASHx)
{
    return (READ_BIT(FLASHx->SR, FLASH_SR_BSY) == (FLASH_SR_BSY));
}
/*get end of operation bilt*/
__STATIC_INLINE uint32_t LL_FLASH_IsActiveFlag_EOP(FLASH_TypeDef *FLASHx)
{
    return (READ_BIT(FLASHx->SR, FLASH_SR_EOP) == (FLASH_SR_EOP));
}
/*clear end of operation bilt*/
__STATIC_INLINE void LL_FLASH_ClearFlag_EOP(FLASH_TypeDef *FLASHx)
{
    SET_BIT(FLASHx->SR, FLASH_SR_EOP); // EOP bit Set clear
}
/* @brief  Set flash erase type.
 * @param  FLASH_TYPEERASE specifies the FLASH flags to clear.
 *          This parameter can be any combination of the following values:
 *            @arg @ref FLASH_TYPEERASE_PAGES         PAGES Erase
 *            @arg @ref FLASH_TYPEERASE_MASSERASE      FLASH Write protected error flag
 * @retval none*/
__STATIC_INLINE void LL_FLASH_DisenableErase(FLASH_TypeDef *FLASHx, uint32_t FLASH_TYPEERASE)
{
    CLEAR_BIT(FLASHx->CR, FLASH_TYPEERASE);
}

/*EnableProgram*/
__STATIC_INLINE void LL_FLASH_EnableProgram(FLASH_TypeDef *FLASHx)
{
    SET_BIT(FLASHx->CR, FLASH_CR_PG);
}
/*DisenableProgram*/
__STATIC_INLINE void LL_FLASH_DisenableProgram(FLASH_TypeDef *FLASHx)
{
    CLEAR_BIT(FLASHx->CR, FLASH_CR_PG);
}
/*read flash's states of lock or unlock*/
__STATIC_INLINE FlashStates LL_FLASH_LockState(FLASH_TypeDef *FLASHx)
{
    return (FlashStates)(READ_BIT(FLASHx->CR, FLASH_CR_LOCK));
}
/*set key for flash*/
__STATIC_INLINE void LL_FLASh_SetKey(FLASH_TypeDef *FLASHx, uint32_t key)
{
    WRITE_REG(FLASH->KEYR, key);
}

ErrorStatus LL_Flash_Unlock(void);
ErrorStatus LL_Flash_PageErase(uint32_t page_addr, uint16_t NbPages);
ErrorStatus LL_FLASH_Program(ProgaramDataType ProgramType, uint32_t flash_addr, uint64_t data);
#endif
