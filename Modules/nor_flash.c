/**
 * @file nor_flash.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-09-10
 *
 * @copyright Copyright (c) 2023
 *
 */
#include <sfud.h>
#include <stdio.h>
#include "nor_flash.h"

sfud_flash *flash = 0;

int nor_flash_init(void)
{
    sfud_err result = SFUD_SUCCESS;
    flash           = sfud_get_device(SFUD_W25Q128_DEVICE_INDEX);
    result          = sfud_init();
    if (result != SFUD_SUCCESS) {
        printf("SFUD init fail!\n");
        return -1;
    }
    printf("SFUD init success!\n");
    return 0;
}

void nor_flash_read(uint32_t addr, size_t size, uint8_t *data)
{
    sfud_err result = SFUD_SUCCESS;
    result          = sfud_read(flash, addr, size, data);
    if (result != SFUD_SUCCESS) {
        printf("SFUD read fail!\n");
    }
}

void nor_flash_write(uint32_t addr, size_t size, uint8_t *data)
{
    sfud_err result = SFUD_SUCCESS;
    result          = sfud_erase_write(flash, addr, size, data);
    if (result != SFUD_SUCCESS) {
        printf("SFUD read fail!\n");
    }
}