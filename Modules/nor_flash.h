/**
 * @file nor_flash.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-09-11
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef __NOR_FLASH_H__
#define __NOR_FLASH_H__

#include <stdint.h>
#include <stddef.h>

#define NOR_FLASH_PAGE_SIZE 256
#define NOR_FLASH_BLK_SIZE  4096

int nor_flash_init(void);
void nor_flash_read(uint32_t addr, size_t size, uint8_t *data);
void nor_flash_write(uint32_t addr, size_t size, uint8_t *data);
#endif