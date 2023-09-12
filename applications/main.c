/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 * 2018-11-06     SummerGift   first version
 */

#include "drv_gpio.h"
#include <board.h>
#include <rtdevice.h>
#include <rtthread.h>

/* defined the LED0 pin: PB2 */
#define LED0_PIN GET_PIN(B, 2)

int main(void)
{
    /* set LED0 pin mode to output */
    rt_kprintf("here's a new app!!!!\n");
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    while (1) {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
}
