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
#include "main.h"

#define APP_ADDR (0x08000000 + 0x6400)
static char *update_flag = "update";

void check_update(void)
{
    if (nor_flash_init() != 0) {
        printf("nor flash init failed\r\n");
        return;
    }
    char read_buf[10] = {0};
    nor_flash_read(0, sizeof(read_buf), read_buf);
    if (strcmp(read_buf, update_flag) == 0) {
        printf("application will be updated\r\n");
    } else {
        printf("application is up to date\r\n");
        return;
    }
}

void jump_to_app(void)
{
    typedef void (*app_func_t)(void);
    uint32_t app_addr   = APP_ADDR;
    uint32_t stk_addr   = *((__IO uint32_t *)app_addr);
    app_func_t app_func = (app_func_t)(*((__IO uint32_t *)(app_addr + 4)));

    if ((((uint32_t)app_func & 0xff000000) != 0x08000000) || (((stk_addr & 0x2ff00000) != 0x20000000) && ((stk_addr & 0x2ff00000) != 0x24000000))) {
        printf("No legitimate application.");
        return;
    }

    printf("Jump to application running ... \n");
    LL_mDelay(200);

    __disable_irq();
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_AFIO);
    LL_APB2_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_PWR);
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOD);
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOA);
    LL_APB2_GRP1_DisableClock(LL_APB2_GRP1_PERIPH_GPIOC);

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

    printf("Qboot jump to application fail.");
}