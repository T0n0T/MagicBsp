/**
 * @file fal_cfg.h
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2023-09-05
 *
 * @copyright Copyright (c) 2023
 *
 */

#ifndef _FAL_CFG_H_
#define _FAL_CFG_H_

#include <board.h>
#include <rtconfig.h>

#define UPDATE_PARTITION_NAME "update"
#define FS_PARTITION_NAME "file"
#define NOR_FLASH_DEV_NAME FAL_USING_NOR_FLASH_DEV_NAME //"norflash0"

/* ===================== Flash device Configuration ========================= */
extern const struct fal_flash_dev stm32_onchip_flash;
extern struct fal_flash_dev nor_flash0;

/* flash device table */
#define FAL_FLASH_DEV_TABLE \
    {                       \
        &nor_flash0,        \
    }
/* ====================== Partition Configuration ========================== */
#ifdef FAL_PART_HAS_TABLE_CFG
/* partition table */
#define FAL_PART_TABLE                                                                                      \
    {                                                                                                       \
        { FAL_PART_MAGIC_WORD, UPDATE_PARTITION_NAME, FAL_USING_NOR_FLASH_DEV_NAME, 0x0, 0x40000, 0 },      \
            { FAL_PART_MAGIC_WORD, FS_PARTITION_NAME, FAL_USING_NOR_FLASH_DEV_NAME, 0x40000, 0x9c0000, 0 }, \
    }
#endif /* FAL_PART_HAS_TABLE_CFG */

#endif /* _FAL_CFG_H_ */