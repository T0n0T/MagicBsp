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

#include <fcntl.h>
#include <stdio.h>
#include <sys/unistd.h>

// #define DRV_DEBUG
#define LOG_TAG "fs"
#include <drv_log.h>

void updateapp(int argc, char **argv)
{
    const char *cmd_info[] = {
        "Usage:\n",
        "updateapp [app]                    - set application from .bin and reboot\n",
        "\n"};

    if (argc < 2) {
        for (int i = 0; i < sizeof(cmd_info) / sizeof(char *); i++) {
            printf(cmd_info[i]);
        }
        return;
    }
    if (rt_strstr(argv[1], ".bin") == 0) {
        printf("app name must be .bin\n");
        return;
    }

    rt_device_t update_part = rt_device_find(FAL_USING_NOR_FLASH_DEV_NAME);
    if (update_part == RT_NULL) {
        printf("can not find update device\n");
        return;
    }

    if (rt_device_open(update_part, RT_DEVICE_FLAG_RDWR) != RT_EOK) {
        printf("can not open update device\n");
        return;
    }
    rt_spi_flash_device_t blk_update = (rt_spi_flash_device_t)update_part;
    uint32_t blk_sz                  = blk_update->geometry.block_size;
    int fd                           = 0;
    int n                            = 0;
    rt_off_t pos                     = 1;
    uint8_t *app_buff                = (uint8_t *)rt_malloc(blk_sz);
    if (app_buff == RT_NULL) {
        printf("malloc app_buff failed\n");
        goto err;
    }

    fd = open(argv[1], O_RDONLY);
    if (fd < 0) {
        printf("open file %s fail\n", argv[1]);
        goto err;
    }

    do {
        rt_memset(app_buff, 0, blk_sz);
        n = read(fd, app_buff, blk_sz);

        if (n < 0) {
            printf("read file %s fail\n", argv[1]);
            goto err;
        }

        if (rt_device_write(update_part, pos, app_buff, 1) != 1) {
            printf("write file %s fail\n", argv[1]);
            goto err;
        }

        if (n < blk_sz) {
            break;
        }
        pos++;
    } while (pos < blk_update->geometry.sector_count);

    // set update flag
    rt_memset(app_buff, 0, blk_sz);
    rt_memcpy(app_buff, "update", sizeof("update"));
    *(uint32_t *)(&app_buff[10]) = (uint32_t)pos;
    if (rt_device_write(update_part, 0, app_buff, 1) != 1) {
        printf("write update_flag fail\n");
        goto err;
    }

    // reboot
    printf("machine need to reboot\n");
    // rt_hw_cpu_reset();
err:
    rt_device_close(update_part);
    if (app_buff != RT_NULL) {
        rt_free(app_buff);
    }
    if (fd >= 0) {
        close(fd);
    }
}
MSH_CMD_EXPORT(updateapp, update app);

static int rt_hw_spi_file_system_init(void)
{
    // 分区块绑定到w25q128
    rt_hw_spi_device_attach("spi1", "spi10", GET_PIN(C, 13));

    if (RT_NULL == rt_sfud_flash_probe(FAL_USING_NOR_FLASH_DEV_NAME, "spi10")) {
        LOG_E("Can't create flash device for '%s'.", FAL_USING_NOR_FLASH_DEV_NAME);
        return -RT_ERROR;
    }

    fal_init();
    struct rt_device *flash_dev = fal_blk_device_create(FS_PARTITION_NAME);
    if (flash_dev == NULL) {
        LOG_E("Can't create a nor flash device on '%s' partition.", FS_PARTITION_NAME);
        return -RT_ERROR;
    } else {
        LOG_D("Create a block device on the %s partition of flash successful.", FS_PARTITION_NAME);
    }
    // 初始化，格式化flash
    // if (dfs_mkfs("elm", FS_PARTITION_NAME) != 0) {
    //     LOG_E("Can't mkfs filesystem on '%s' partition.", FS_PARTITION_NAME);
    //     return -RT_ERROR;
    // }
    // 挂载文件系统
    if (dfs_mount(FS_PARTITION_NAME, "/", "elm", 0, 0) != 0) // 注册块设备，这一步可以将外部flash抽象为系统的块设备
    {
        LOG_E("Can't mount filesystem on '%s' partition.", FS_PARTITION_NAME);
        return -RT_ERROR;
    }
    return RT_EOK;
}
INIT_ENV_EXPORT(rt_hw_spi_file_system_init);
