/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2023-04-11     liwentai       the first version
 */
#include "dfs_fs.h"
#include "drv_spi.h"
#include "fal.h"
#include "spi_flash_sfud.h"
#include <rtthread.h>

static int rt_hw_spi_file_system_init(void)
{
    // 分区块绑定到w25q128
    rt_hw_spi_device_attach("spi1", "spi10", GET_PIN(C, 13));

    if (RT_NULL == rt_sfud_flash_probe(FAL_USING_NOR_FLASH_DEV_NAME, "spi10")) {
        LOG_E("Can't create flash device for '%s'.", FAL_USING_NOR_FLASH_DEV_NAME);
        return -RT_ERROR;
    }

    fal_init();
    struct rt_device* flash_dev = fal_blk_device_create(FS_PARTITION_NAME);
    if (flash_dev == NULL) {
        LOG_E("Can't create a nor flash device on '%s' partition.", FS_PARTITION_NAME);
        return -RT_ERROR;
    } else {
        LOG_D("Create a block device on the %s partition of flash successful.", FS_PARTITION_NAME);
    }
    // 初始化，格式化flash
    if (dfs_mkfs("elm", FS_PARTITION_NAME) != 0) {
        LOG_E("Can't mkfs filesystem on '%s' partition.", FS_PARTITION_NAME);
        return -RT_ERROR;
    }
    // 挂载文件系统
    if (dfs_mount(FS_PARTITION_NAME, "/", "elm", 0, 0) != 0) // 注册块设备，这一步可以将外部flash抽象为系统的块设备
    {
        LOG_E("Can't mount filesystem on '%s' partition.", FS_PARTITION_NAME);
        return -RT_ERROR;
    }
    return RT_EOK;
}
INIT_ENV_EXPORT(rt_hw_spi_file_system_init);
