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
#include "drv_gpio.h"
/* defined the LED0 pin: PB2 */
#define LED0_PIN GET_PIN(B, 2)

void test(void *p)
{
    int count = 0;
    rt_thread_t thread = rt_thread_self();
    while (1)
    {
        count++;
        printf("%d %p\n", count, count);
        printf("%s sp --> 0x%08x \n", thread->parent.name, thread->sp);

        rt_thread_mdelay(500);
    }
}

int main(void)
{
    /* set LED0 pin mode to output */
    rt_pin_mode(LED0_PIN, PIN_MODE_OUTPUT);
    rt_thread_t tt = RT_NULL;
    tt = rt_thread_create("try", test, RT_NULL, 512, 10, 15);
    rt_thread_startup(tt);
    while (1)
    {
        rt_pin_write(LED0_PIN, PIN_HIGH);
        rt_thread_mdelay(500);
        rt_pin_write(LED0_PIN, PIN_LOW);
        rt_thread_mdelay(500);
    }
}
