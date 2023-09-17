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

#include <stdio.h>
#include "nor_flash.h"

#define USING_SFUD 0
#if USING_SFUD
#include <sfud.h>
sfud_flash *flash = 0;

int nor_flash_init(void)
{
    sfud_err result = SFUD_SUCCESS;
    flash           = sfud_get_device(SFUD_W25Q128_DEVICE_INDEX);
    result          = sfud_device_init(flash);
    if (result != SFUD_SUCCESS) {
        printf("SFUD init fail!\r\n");
        return -1;
    }
    printf("SFUD init success!\r\n");
    return 0;
}

void nor_flash_read(uint32_t addr, size_t size, uint8_t *data)
{
    sfud_err result = SFUD_SUCCESS;
    result          = sfud_read(flash, addr, size, data);
    if (result != SFUD_SUCCESS) {
        printf("SFUD read fail!\r\n");
    }
}

void nor_flash_write(uint32_t addr, size_t size, uint8_t *data)
{
    sfud_err result = SFUD_SUCCESS;
    result          = sfud_erase_write(flash, addr, size, data);
    if (result != SFUD_SUCCESS) {
        printf("SFUD write fail!\r\n");
    }
}

#else
#include "w25qxx.h"
int nor_flash_init(void)
{
    W25QXX_Init();
    return 0;
}

void nor_flash_read(uint32_t addr, size_t size, uint8_t *data)
{
    W25QXX_Read(data, addr, size);
}

void nor_flash_write(uint32_t addr, size_t size, uint8_t *data)
{
    W25QXX_Write(data, addr, size);
}

#endif