/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include <rtthread.h>
#include <rtdevice.h>
#include <board.h>
#include <sfud.h>
#include <drv_spi.h>
// #include "drv_gpio.h"

/* defined the LED0 pin: PB2 */
#define LED0_PIN GET_PIN(B, 2)

static int norflash_bind(void)
{
    rt_hw_spi_device_attach("spi1", "spi10", GET_PIN(C, 13));

    if (RT_NULL == rt_sfud_flash_probe(FAL_USING_NOR_FLASH_DEV_NAME, "spi10")) {
        return -RT_ERROR;
    }

    return RT_EOK;
}
INIT_COMPONENT_EXPORT(norflash_bind);

int main(void)
{
    /* set LED0 pin mode to output */
    return 0;
}
