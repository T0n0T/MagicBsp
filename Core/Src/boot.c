/**
 * @file boot.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-09-11
 *
 * @copyright Copyright (c) 2023
 *
 */

#include "nor_flash.h"
#include "stmflash.h"
#include <string.h>
#include "main.h"

// APP_ADDR need to fit size of bootloader
#define APP_ADDR (0x08000000 + 0x2400)
static char update_flag[]          = "update";
uint8_t fw_buf[NOR_FLASH_BLK_SIZE] = {0};
char check_buf[20]                 = {0};

static int update_fw(uint32_t sectornum_using)
{
    int result              = 0;
    uint32_t nor_sector_num = 1;
    uint32_t addr           = 0;
    uint8_t *p_fw_buf       = fw_buf;

    stmflash_unlock();
    while (nor_sector_num < 1 + sectornum_using) {
        memset(fw_buf, 0, sizeof(fw_buf));
        nor_flash_read(nor_sector_num * NOR_FLASH_BLK_SIZE, sizeof(fw_buf), fw_buf);
        addr = APP_ADDR + NOR_FLASH_BLK_SIZE * (nor_sector_num - 1);
        stmflash_write_2bytes(addr, (uint32_t *)p_fw_buf, sizeof(fw_buf) / 4);
        nor_sector_num++;
        printf("updating fw, loading ... %0.2f%%\r", (float)(nor_sector_num - 1) / sectornum_using * 100);
    }
    stmflash_lock();
    return result;
}

void check_update(void)
{
    if (nor_flash_init() != 0) {
        printf("nor flash init failed\r\n");
        return;
    }

    nor_flash_read(0, sizeof(check_buf), check_buf);
    if (strncmp(check_buf, update_flag, sizeof(update_flag)) == 0) {
        printf("application will be updated\r\n");
        update_fw((uint32_t)check_buf[10]);
        memset(check_buf, 0, sizeof(check_buf));
    } else {
        printf("application is up to date\r\n");
        return;
    }
    nor_flash_write(0, sizeof(check_buf), check_buf);
}

void jump_to_app(void)
{
    typedef void (*app_func_t)(void);
    uint32_t app_addr   = APP_ADDR;
    uint32_t stk_addr   = *((__IO uint32_t *)app_addr);
    app_func_t app_func = (app_func_t)(*((__IO uint32_t *)(app_addr + 4)));

    if ((((uint32_t)app_func & 0xff000000) != 0x08000000) || (((stk_addr & 0x2ff00000) != 0x20000000) && ((stk_addr & 0x2ff00000) != 0x24000000))) {
        printf("No legitimate application.\r\n");
        return;
    }

    printf("Jump to application running ... \r\n");
    LL_mDelay(200);

    __disable_irq();
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_AFIO);
    LL_APB2_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_PWR);

    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOB);
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOC);
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOD);

    LL_RCC_HSE_Disable();
    LL_RCC_DeInit();

    LL_SPI_Disable(SPI1);
    LL_SPI_DeInit(SPI1);

    SysTick->CTRL = 0;
    SysTick->LOAD = 0;
    SysTick->VAL  = 0;

    for (int i = 0; i < 128; i++) {
        NVIC_DisableIRQ(i);
        NVIC_ClearPendingIRQ(i);
    }

    __set_CONTROL(0);
    __set_MSP(stk_addr);

    app_func(); // Jump to application running

    printf("Qboot jump to application fail.\r\n");
}